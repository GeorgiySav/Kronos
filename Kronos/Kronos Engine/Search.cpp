#include "Search.h"

#include "consts.h"
#include "Search_Manager.h"
#include "Zobrist_Hashing.h"
#include "Move_Picker.h"
#include "SyzygyTB.h"

#include <mutex>

namespace KRONOS
{
	namespace SEARCH
	{

		int LMR[64][64];
		int LMP[2][9];

		const int LATE_MOVE_PRUNING_DEPTH = 8;

		void initVars() {
			for (int d = 1; d < 64; d++) {
				for (int m = 1; m < 64; m++) {
					LMR[d][m] = log(d) * log(m) / 2 - 0.2;
				}
			}
			for (int d = 1; d < 9; d++) {
				LMP[0][d] = 2.5 + 2 * d * d / 4.5;
				LMP[1][d] = 4.0 + 4 * d * d / 4.5;
			}
		}

		using namespace HASH;

		Thread::Thread(int id) {
			ID = id;
			exitFlag = false;
			sleepFlag = true;

		}
		Thread::~Thread() {
			exitFlag = true;
			wakeup();
			thread.join();
		}

		void Thread::sleep() {
			sleepFlag = true;
		}

		void Thread::wakeup() {
			sleepFlag = false;
			sleepCondition.notify_one();
		}

		void Thread::wait() {
			std::unique_lock<std::mutex> lk(threadLock);
			sleepCondition.wait(lk);
		}

		Search_Thread::Search_Thread(int ID, Search_Manager& sm) : Thread(ID), SM(sm)
		{
			sleep();
			thread = std::thread(&Search_Thread::think, this);
		}

		Search_Thread::Search_Thread(const Search_Thread& other) : Thread(other.ID), SM(other.SM) {
			sleep();
			clearData();
			if (thread.joinable())
				thread.join();
			thread = std::thread(&Search_Thread::think, this);
		}

		Search_Thread::~Search_Thread()
		{
			clearData();
		}

		bool Search_Thread::repeatedDraw() {

			u64& hash = threadPositions.at(threadPly).hash;
			int index = threadPly - 1;
			int count = 1;

			while (index >= 0) {
				if (threadPositions.at(index).hash == hash) {
					count++;
					if (count >= 3)
						return true;
				}
				index--;
			}
			index = gamePly - 1;
			while (index >= 0) {
				if (previousPositions->at(index).hash == hash) {
					count++;
					if (count >= 3)
						return true;
				}
				index--;
			}

			return false;
		}

		void updateHistoryValue(int16_t& value, int delta) {
			value = delta - (value * abs(delta)) / 800;
		}

		void Search_Thread::updateHistory(Move& newMove, Move_List& quiets, bool side, int depth)
		{
			int16_t bonus = depth * depth;
			updateHistoryValue(historyTable[side][newMove.moved_Piece][newMove.to], bonus);
			for (int i = 0; i < quiets.size(); i++) {
				Move& move = quiets.at(i);
				if (move == newMove) continue;
				updateHistoryValue(historyTable[side][move.moved_Piece][move.to], -bonus);
			}
		}

		int16_t Search_Thread::getHistoryValue(bool side, Move& move) {
			return historyTable[side][move.moved_Piece][move.to];
		}

		void Search_Thread::updateKillers(Move& newMove)
		{
			if (newMove != killer1.at(threadPly)) {
				killer2.at(threadPly) = killer1.at(threadPly);
				killer1.at(threadPly) = newMove;
			}
		}

		int16_t Search_Thread::quiescence(int alpha, int beta, int plyFromRoot, bool inPV)
		{
			Position& nodePosition = threadPositions.at(threadPly);
			u64& nodeHash = nodePosition.hash;
			bool nodeCheck = inCheck(nodePosition);

			if (stop)
				return 0;
			if (repeatedDraw())
				return 0;
			if (nodePosition.halfMoves > 99)
				return 0;
			if (threadPly + gamePly >= MAX_PLY - 1)
				return 0;
			// mate distance pruning
			alpha = std::max(alpha, -MATE + plyFromRoot);
			beta = std::min(beta, MATE - plyFromRoot - 1);
			if (alpha >= beta) {
				return alpha;
			}

			transEntry tte;
			tte.move = NULL;
			int16_t tteEval = UNDEFINED;
			if (SM.transTable.probe(nodeHash, tte)) {
				tteEval = TranspositionTableToScore(tte.eval, plyFromRoot);
				if (!inPV && (tte.getBound() == (int)BOUND::EXACT
					|| (tte.getBound() == (int)BOUND::ALPHA && tteEval <= alpha)
					|| (tte.getBound() == (int)BOUND::BETA && tteEval >= beta))) {
					return tteEval;
				}
			}

			int wdl = SYZYGY::probeWDL(&nodePosition);
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL)
			{
				int score = wdl == (int)SYZYGY::SyzygyResult::SYZYGY_LOSS ? MATED_IN_MAX_PLY + plyFromRoot + 1
					: wdl == (int)SYZYGY::SyzygyResult::SYZYGY_WIN ? MATE_IN_MAX_PLY - plyFromRoot - 1 : 0;

				return score;
			}

			int16_t staticEval = SM.evalTable.getEval(nodePosition, eval);

			if (staticEval >= beta) {
				return beta;
			}
			if (alpha < staticEval) {
				alpha = staticEval;
			}

			Move_Picker movePicker(
				nodePosition, 
				true, 
				MoveIntToMove(tte.move, &nodePosition),
				NULL_MOVE,
				NULL_MOVE);

			int16_t bestScore = -INFINITE;
			Move move;
			int movesSearched = 0;

			while (movePicker.nextMove(*this, move)) {

				// make the move
				threadPly++;
				threadPositions.at(threadPly) = nodePosition;
				updatePosition(threadPositions.at(threadPly), move);
				zobrist.updateHash(nodePosition, threadPositions.at(threadPly), move);

				int score = -quiescence(-beta, -alpha, plyFromRoot + 1, inPV);

				// undo move
				threadPly--;

				movesSearched++;

				if (stop)
					return 0;

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						if (score >= beta) {
							break;
						}
						alpha = score;
					}
				}

			}
			if (movesSearched == 0) {
				if (nodeCheck)
					return -MATE + plyFromRoot;
				else
					return 0;
			}

			return bestScore;
		}

		int16_t Search_Thread::alphaBeta(int depth, int alpha, int beta, int plyFromRoot, bool inPV)
		{

			Position& nodePosition = threadPositions.at(threadPly);
			u64& nodeHash = nodePosition.hash;
			bool nodeCheck = inCheck(nodePosition);
			int16_t nodeEval = UNDEFINED;

			if (depth < 1) {
				return quiescence(alpha, beta, plyFromRoot, inPV);
			}

			if (stop)
				return 0;
			if (repeatedDraw())
				return 0;
			if (nodePosition.halfMoves > 99)
				return 0;
			if (threadPly + gamePly >= MAX_PLY - 1)
				return 0;
			// mate distance pruning
			alpha = std::max(alpha, -MATE + plyFromRoot);
			beta = std::min(beta, MATE - plyFromRoot - 1);
			if (alpha >= beta) {
				return alpha;
			}

			transEntry tte;
			tte.move = NULL;
			if (SM.transTable.probe(nodeHash, tte)) {
				nodeEval = HASH::TranspositionTableToScore(tte.eval, plyFromRoot);
				if (!inPV && tte.depth >= depth
					&& (tte.getBound() == (int)BOUND::EXACT
						|| (tte.getBound() == (int)BOUND::ALPHA && nodeEval <= alpha)
						|| (tte.getBound() == (int)BOUND::BETA && nodeEval >= beta))) {
					return nodeEval;
				}
			}

			int wdl = SYZYGY::probeWDL(&nodePosition);
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL)
			{
				int score = wdl == (int)SYZYGY::SyzygyResult::SYZYGY_LOSS ? MATED_IN_MAX_PLY + plyFromRoot + 1
					: wdl == (int)SYZYGY::SyzygyResult::SYZYGY_WIN ? MATE_IN_MAX_PLY - plyFromRoot - 1 : 0;

				SM.transTable.saveEntry(
					nodeHash, 
					NULL, 
					std::min(depth + SYZYGY::SYZYGYLargest, MAX_PLY - 1), 
					ScoreToTranpositionTable(score, plyFromRoot), 
					(int)BOUND::EXACT);

				return score;
			}

			Move_Picker movePicker(
				nodePosition,
				false,
				MoveIntToMove(tte.move, &nodePosition),
				killer1.at(threadPly),
				killer2.at(threadPly));

			int16_t bestScore = -INFINITE;
			Move bestMoveInThisPosition;
			BOUND bound = BOUND::ALPHA;
			Move move;
			int movesSearched = 0;
			Move_List quiets;

			evalHistory.at(threadPly) = nodeEval = nodeCheck ? UNDEFINED : (nodeEval == UNDEFINED ? SM.evalTable.getEval(nodePosition, eval) : nodeEval);

			// a position is improving if the eval has gone up compared to the position 2 ply ago
			bool improving = !nodeCheck && threadPly >= 2 && (evalHistory.at(threadPly) > evalHistory.at(threadPly - 2) || evalHistory.at(threadPly - 2) == UNDEFINED);

			// reduction based on pv, improving for quieet moves
			int baseReduction = 0;
			baseReduction += (!inPV) + (!improving);

			while (movePicker.nextMove(*this, move)) { 
				int score = 0;

				// make the move
				threadPly++;
				threadPositions.at(threadPly) = nodePosition;
				updatePosition(threadPositions.at(threadPly), move);
				zobrist.updateHash(nodePosition, threadPositions.at(threadPly), move);

				if (movesSearched == 0) {

					score = -alphaBeta(depth - 1, -beta, -alpha, plyFromRoot + 1, inPV);

				}
				else {

					// Late Move Reductions
					int R = 1;
					if (!move.isTactical() && depth > 2) {
						R = LMR[std::min(depth, 63)][std::min(movesSearched, 63)];

						R += baseReduction;

						// reduce killers less
						if (move == killer1.at(threadPly - 1) || move == killer2.at(threadPly - 1))
							R -= 2;

						// make sure that we don't extend or drop in quiescence search
						R = std::min(depth - 1, std::max(R, 1));
					}

					// Princpial Variation
					score = -alphaBeta(depth - R, -alpha - 1, -alpha, plyFromRoot + 1, false);

					// if a reduced move fails high, research
					if (score > alpha && R != 1)
						score = -alphaBeta(depth - 1, -alpha - 1, -alpha, plyFromRoot + 1, false);

					if (score > alpha && score < beta) {
						score = -alphaBeta(depth - 1, -beta, -alpha, plyFromRoot + 1, inPV);
					}

				}
				// undo move
				threadPly--;

				movesSearched++;

				if (stop)
					return 0;

				if (!move.isTactical() && quiets.size() < 64)
					quiets.add(move);

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						bound = BOUND::EXACT;
						bestMoveInThisPosition = move;
						if (score >= beta) {
							if (!move.isTactical())
								updateKillers(move);
							bound = BOUND::BETA;
							break;
						}
						alpha = score;
					}
				}

			}
			if (movesSearched == 0) {
				if (nodeCheck)
					return -MATE + plyFromRoot;
				else
					return 0;
			}

			if (!bestMoveInThisPosition.isTactical()) {
				updateHistory(bestMoveInThisPosition, quiets, nodePosition.status.isWhite, depth);
			}

			if (bound == HASH::BOUND::EXACT && !inPV)
				bound = HASH::BOUND::ALPHA;

			SM.transTable.saveEntry(
				nodeHash,
				bestMoveInThisPosition.toIntMove(),
				depth,
				ScoreToTranpositionTable(bestScore, plyFromRoot),
				(int)bound);

			return bestScore;
		}

		int16_t Search_Thread::root(int depth, int alpha, int beta)
		{
			Position& nodePosition = threadPositions.at(0);
			u64& nodeHash = nodePosition.hash;
			bool nodeCheck = inCheck(nodePosition);
			int16_t nodeEval = UNDEFINED;

			transEntry tte;
			tte.move = NULL;
			if (SM.transTable.probe(nodeHash, tte)) {
				nodeEval = HASH::TranspositionTableToScore(tte.eval, 0);
			}

			Move_Picker movePicker(
				nodePosition,
				false,
				MoveIntToMove(tte.move, &nodePosition),
				killer1.at(0),
				killer2.at(0));

			if (!movePicker.hasMoves()) {
				if (nodeCheck)
					return -MATE;
				else
					return 0;
			}

			int16_t bestScore = -INFINITE;
			BOUND bound = BOUND::ALPHA;
			Move move;
			int movesSearched = 0;
			Move_List quiets;

			evalHistory.at(threadPly) = nodeEval = nodeCheck ? UNDEFINED : (nodeEval == UNDEFINED ? SM.evalTable.getEval(nodePosition, eval) : nodeEval);

			while (movePicker.nextMove(*this, move)) {
				int score = 0;

				// make the move
				threadPly++;
				threadPositions.at(threadPly) = nodePosition;
				updatePosition(threadPositions.at(threadPly), move);
				zobrist.updateHash(nodePosition, threadPositions.at(threadPly), move);

				if (movesSearched == 0) {

					score = -alphaBeta(depth - 1, -beta, -alpha, 1, true);

				}
				else {

					// Late Move Reductions
					int R = 1;
					if (!move.isTactical() && depth > 2) {
						R = LMR[std::min(depth, 63)][std::min(movesSearched, 63)];

						R++;

						// reduce killers less
						if (move == killer1.at(0) || move == killer2.at(0)) 
							R -= 2;

						// make sure that we don't extend or drop in quiescence search
						R = std::min(depth - 1, std::max(R, 1));
					}

					// Princpial Variation
					score = -alphaBeta(depth - 1, -alpha - 1, -alpha, 1, false);

					// if a reduced move fails high, research
					if (score > alpha && R != 1)
						score = -alphaBeta(depth - 1, -alpha - 1, -alpha, 1, false);

					if (score > alpha && score < beta) {
						score = -alphaBeta(depth - 1, -beta, -alpha, 1, true);
					}
				}
				// undo move
				threadPly--;

				movesSearched++;

				if (stop)
					return 0;
				
				if (!move.isTactical() && quiets.size() < 64)
					quiets.add(move);

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						bestMoveThisIteration = move;
						bound = BOUND::EXACT;
						if (score >= beta) {
							if (!move.isTactical())
								updateKillers(move);
							bound = BOUND::BETA;
							break;
						}
						alpha = score;
					}
				}

			}

			if (!bestMoveThisIteration.isTactical()) {
				updateHistory(bestMoveThisIteration, quiets, nodePosition.status.isWhite, depth);
			}

			SM.transTable.saveEntry(
				nodeHash,
				bestMoveThisIteration.toIntMove(),
				depth,
				ScoreToTranpositionTable(bestScore, 0),
				(int)bound);

			return bestScore;
		}

		void Search_Thread::interativeDeepening()
		{
			int MAX_ITERATIVE_DETPH = 20;
			int depth;
			int score;

			for (depth = 1; depth <= MAX_ITERATIVE_DEPTH; depth++)
			{

				score = root(depth, -INFINITE, INFINITE);

				if (stop)
					break;

				bestMove = Search_Move(bestMoveThisIteration, depth, score);

			}

			SM.stopSearch();
		}

		void Search_Thread::think()
		{
			while (!exitFlag) {
				if (sleepFlag) {
					wait();
				}
				else {
					interativeDeepening();
					sleep();
				}
			}
		}

		void Search_Thread::clearData()
		{
			previousPositions = nullptr;
			threadPositions.clear();
			gamePly = 0;
			threadPly = 0;
			bestMoveThisIteration = NULL_MOVE;
			bestMove = Search_Move();
			numNodes = 0;
			memset(historyTable, 0, sizeof(historyTable));
			killer1.clear();
			killer2.clear();
			evalHistory.clear();
		}

		void Search_Thread::setData(std::vector<Position>* prevPoss, int curPly) {
			previousPositions = prevPoss;
			gamePly = curPly;
			threadPositions.resize(MAX_PLY - curPly);
			threadPositions.at(0) = previousPositions->at(curPly);
			killer1.resize(MAX_PLY - curPly);
			killer2.resize(MAX_PLY - curPly);
			evalHistory.resize(MAX_PLY - curPly);
		}

		void Search_Thread::setPosition(std::vector<Position>* prevPoss, int curPly)
		{
			clearData();
			setData(prevPoss, curPly);
		}

		void Search_Thread::beginThink()
		{
			stop = false;
			wakeup();
		}

		void Search_Thread::stopSearch()
		{
			stop = true;
		}
	}
} // KRONOS