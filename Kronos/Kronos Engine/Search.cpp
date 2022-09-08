#include "Search.h"

#include "consts.h"
#include "Search_Manager.h"
#include "Zobrist_Hashing.h"

namespace KRONOS
{
	namespace SEARCH
	{

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
			evalTable.setSize(2);
		}

		Search_Thread::Search_Thread(const Search_Thread& other) : Thread(other.ID), SM(other.SM) {
			sleep();
			clearData();
			thread.join();
			thread = std::thread(&Search_Thread::think, this);
			evalTable.setSize(2);
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

		int16_t Search_Thread::quiescence(int alpha, int beta, int plyFromRoot)
		{
			Position& nodePosition = threadPositions.at(threadPly);
			u64& nodeHash = nodePosition.hash;

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
			tte.eval = UNDEFINED;
			if (SM.transTable.probe(nodeHash, tte)) {
				tte.eval = TranspositionTableToScore(tte.eval, plyFromRoot);
				if (tte.getBound() == (int)BOUND::EXACT
					|| (tte.getBound() == (int)BOUND::ALPHA && tte.eval <= alpha)
					|| (tte.getBound() == (int)BOUND::BETA && tte.eval >= beta)) {
					return tte.eval;
				}
			}

			int16_t staticEval = evalTable.getEval(nodePosition, eval);

			if (staticEval >= beta) {
				return beta;
			}
			if (alpha < staticEval) {
				alpha = staticEval;
			}

			Move_List<256> moves;
			generateMoves(nodePosition.status.isWhite, nodePosition.board, nodePosition.status, moves);

			if (moves.size == 0) {
				if (inCheck(nodePosition))
					return -MATE + plyFromRoot;
				else
					return 0;
			}

			int bestScore = -INFINITE;

			for (int i = 0; i < moves.size; i++) {
				Move& move = moves.at(i);

				if (move.flag & CAPTURE) {
					// make the move
					threadPly++;
					threadPositions.at(threadPly) = nodePosition;
					updatePosition(threadPositions.at(threadPly), move);
					zobrist.updateHash(nodePosition, threadPositions.at(threadPly), move);

					int score = -quiescence(-beta, -alpha, plyFromRoot + 1);

					// undo move
					threadPly--;

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
			}

			return bestScore;
		}

		int16_t Search_Thread::alphaBeta(int depth, int alpha, int beta, int plyFromRoot)
		{
			if (depth < 1) {
				return quiescence(alpha, beta, plyFromRoot);
			}

			Position& nodePosition = threadPositions.at(threadPly);
			u64& nodeHash = nodePosition.hash;

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
			tte.eval = UNDEFINED;
			if (SM.transTable.probe(nodeHash, tte)) {
				tte.eval = HASH::TranspositionTableToScore(tte.eval, plyFromRoot);
				if (tte.depth >= depth
					&& (tte.getBound() == (int)BOUND::EXACT
						|| (tte.getBound() == (int)BOUND::ALPHA && tte.eval <= alpha)
						|| (tte.getBound() == (int)BOUND::BETA && tte.eval >= beta))) {
					return tte.eval;
				}
			}


			Move_List<256> moves;
			generateMoves(nodePosition.status.isWhite, nodePosition.board, nodePosition.status, moves);

			if (moves.size == 0) {
				if (inCheck(nodePosition))
					return -MATE + plyFromRoot;
				else
					return 0;
			}

			int16_t bestScore = -INFINITE;
			Move bestMoveInThisPosition;
			BOUND bound = BOUND::ALPHA;

			for (int i = 0; i < moves.size; i++) {
				Move& move = moves.at(i);

				// make the move
				threadPly++;
				threadPositions.at(threadPly) = nodePosition;
				updatePosition(threadPositions.at(threadPly), move);
				zobrist.updateHash(nodePosition, threadPositions.at(threadPly), move);

				int score = -alphaBeta(depth - 1, -beta, -alpha, plyFromRoot + 1);

				// undo move
				threadPly--;

				if (stop)
					return 0;

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						bound = BOUND::EXACT;
						bestMoveInThisPosition = move;
						if (score >= beta) {
							bound = BOUND::BETA;
							break;
						}
						alpha = score;
					}
				}

			}

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

			transEntry tte;
			tte.move = NULL;
			tte.eval = UNDEFINED;
			if (SM.transTable.probe(nodeHash, tte)) {
				tte.eval = TranspositionTableToScore(tte.eval, 0);
				if (tte.depth >= depth
					&& (tte.getBound() == (int)BOUND::EXACT
						|| (tte.getBound() == (int)BOUND::ALPHA && tte.eval <= alpha)
						|| (tte.getBound() == (int)BOUND::BETA && tte.eval >= beta))
					&& tte.move != NULL) {
					bestMoveThisIteration = MoveIntToMove(tte.move, &nodePosition);
					return tte.eval;
				}
			}

			Move_List<256> moves;
			generateMoves(nodePosition.status.isWhite, nodePosition.board, nodePosition.status, moves);

			if (moves.size == 0) {
				if (inCheck(nodePosition))
					return -MATE;
				else
					return 0;
			}

			int16_t bestScore = -INFINITE;
			BOUND bound = BOUND::ALPHA;

			for (int i = 0; i < moves.size; i++) {
				Move& move = moves.at(i);

				// make the move
				threadPly++;
				threadPositions.at(threadPly) = nodePosition;
				updatePosition(threadPositions.at(threadPly), move);
				zobrist.updateHash(nodePosition, threadPositions.at(threadPly), move);

				int score = -alphaBeta(depth - 1, -beta, -alpha, 1);

				// undo move
				threadPly--;

				if (stop)
					return 0;

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						bound = BOUND::EXACT;
						bestMoveThisIteration = move;
						if (score >= beta) {
							bound = BOUND::BETA;
							break;
						}
						alpha = score;
					}
				}

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
			int MAX_ITERATIVE_DETPH = 8;

			for (int depth = 1; depth < MAX_ITERATIVE_DETPH; depth++) 
			{
				root(depth, -INFINITE, INFINITE);

				if (stop)
					break;

				bestMove = bestMoveThisIteration;
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
			numNodes = 0;
		}

		void Search_Thread::setData(std::vector<Position>* prevPoss, int curPly) {
			previousPositions = prevPoss;
			gamePly = curPly;
			threadPositions.resize(MAX_PLY - curPly);
			threadPositions.at(0) = previousPositions->at(curPly);
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