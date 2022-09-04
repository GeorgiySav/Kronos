#include "Search.h"

#include <queue>

#include "Move_Generation.h"
#include "Magic.h"

#include "Zobrist_Hashing.h"

#include "SyzygyTB.h"
#include "Move_Picker.h"

namespace KRONOS {
	
	namespace SEARCH {

		// late mate reduction values indexed by depth and number of moves processed in a position
		int LMR_table[64][64];

		void initLMR() 
		{
			for (int d = 0; d < 64; d++) {
				for (int n = 0; n < 64; n++) {
					LMR_table[d][n] = 0.75 + log(d) * log(n) / 2.1;
				}
			}
		}

		Search_Tree::Search_Tree()
		{
			resourcesLeft = false;
			transpositionTable.setSize(100);
			openingBook.readBook("./Opening Books/Performance.bin");
			openingBook.setMode(POLY::POLY_MODE::BEST_WEIGHT);
			ABDADATable.setSize(3);
			evalTable.setSize(2);
		}

		Search_Tree::~Search_Tree()
		{

		}
		
		void Search_Tree::checkResources()
		{
			
		}

		inline int Search_Tree::quiescenceSearch(Search_Thread& sData, int alpha, int beta, int plyFromRoot, bool inPV) {
			Position& curPosition = sData.threadPositions[sData.threadPly];
			u64 posHash = sData.threadPositions[sData.threadPly].hash;
			int16_t eval = evalTable.getEval(sData.threadPositions[sData.threadPly], sData.eval);
			sData.numNodes++;

			if (!resourcesLeft || sData.stopIteration)
				return CANCELLED;
			if (sData.gamePly + sData.threadPly >= MAX_PLY - 1)
				return eval;
			if (repeated(sData) || curPosition.halfMoves > 99)
				return 0;
				
			HASH::transEntry tte;
			tte.move = NULL;
			if (transpositionTable.probe(posHash, tte)) {
				tte.eval = HASH::TranspositionTableToScore(tte.eval, plyFromRoot);
				if (!inPV && (tte.getBound() == (int)HASH::BOUND::EXACT
					|| (tte.getBound() == (int)HASH::BOUND::ALPHA && tte.eval <= alpha)
					|| (tte.getBound() == (int)HASH::BOUND::BETA && tte.eval >= beta))) {
					return tte.eval;
				}
			}
			int wdl = SYZYGY::probeWDL(&curPosition);
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL)
			{
				int score = wdl == (int)SYZYGY::SyzygyResult::SYZYGY_LOSS ? MATED_IN_MAX_PLY + plyFromRoot + 1
					: wdl == (int)SYZYGY::SyzygyResult::SYZYGY_WIN ? MATE_IN_MAX_PLY - plyFromRoot - 1 : 0;

				return score;
			}

			if (eval >= beta) {
				return beta;
			}
			if (alpha < eval) {
				alpha = eval;
			}
			
			MOVE_PICKER movePicker(
				&curPosition,
				true,
				1,
				&sData,
				MoveIntToMove(tte.move, &curPosition),
				sData.killerMoves1[sData.threadPly + 1],
				sData.killerMoves2[sData.threadPly + 1]
			);

			if (!movePicker.hasMoves()) {
				if (inCheck(curPosition)) {
					return -MATE + plyFromRoot;
				}
				else {
					return 0;
				}
			}
			
			int bestScore = -INFINITE;
			Move move;

			while (movePicker.getMove(move)) {
				if (move.flag & CAPTURE) {

					sData.threadPly++;
					sData.threadPositions[sData.threadPly] = sData.threadPositions[sData.threadPly - 1];
					updatePosition(sData.threadPositions[sData.threadPly], move);
					int score = -quiescenceSearch(sData, -beta, -alpha, plyFromRoot + 1, inPV);
					sData.threadPly--;

					if (!resourcesLeft || sData.stopIteration)
						return CANCELLED;

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

		bool Search_Tree::repeated(Search_Thread& sData)
		{
			if (sData.gamePly + sData.threadPly + 1 < 4)
				return false;

			int count = 1;

			u64 compHash = sData.threadPositions[sData.threadPly].hash;
			int index = sData.threadPly - 1;

			while (index >= 0) {
				if (sData.threadPositions[index].hash == compHash) {
					count++;
					if (count >= 3)
						return true;
				}
				index--;
			}
			index = sData.gamePly;
			while (index >= 0) {
				if (sData.previousPositions->at(index).hash == compHash) {
					count++;
					if (count >= 3)
						return true;
				}
				index--;
			}

			return false;
			

		}

		int Search_Tree::alphaBeta(Search_Thread& sData, int depth, int plyFromRoot, int alpha, int beta, bool inPV) {
			Position& curPosition = sData.threadPositions[sData.threadPly];
			u64 posHash = curPosition.hash;

			sData.numNodes++;

			int16_t staticEval = evalTable.getEval(curPosition, sData.eval);;
			
			if (!resourcesLeft || sData.stopIteration)
				return CANCELLED;
			if (sData.gamePly + sData.threadPly >= MAX_PLY - 1)
				return staticEval;
			if (repeated(sData) || curPosition.halfMoves > 99)
				return 0;			
			// mate distance pruning
			alpha = std::max(alpha, -MATE + plyFromRoot);
			beta = std::min(beta, MATE - plyFromRoot - 1);
			if (alpha >= beta) {
				return alpha;
			}	

			HASH::transEntry tte;
			tte.move = NULL;
			tte.eval = UNDEFINED;
			if (transpositionTable.probe(posHash, tte)) {
				tte.eval = HASH::TranspositionTableToScore(tte.eval, plyFromRoot);
				if (!inPV && tte.depth >= depth 
					&& (tte.getBound() == (int)HASH::BOUND::EXACT
					|| (tte.getBound() == (int)HASH::BOUND::ALPHA && tte.eval <= alpha)
					|| (tte.getBound() == (int)HASH::BOUND::BETA  && tte.eval >= beta ))) {
					return tte.eval;
				}
			}

			int wdl = SYZYGY::probeWDL(&curPosition);
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL)
			{
				int score = wdl == (int)SYZYGY::SyzygyResult::SYZYGY_LOSS ? MATED_IN_MAX_PLY + plyFromRoot + 1
					       : wdl == (int)SYZYGY::SyzygyResult::SYZYGY_WIN ? MATE_IN_MAX_PLY - plyFromRoot - 1 : 0;
				
				transpositionTable.saveEntry(posHash, NULL, std::min(depth + SYZYGY::SYZYGYLargest, MAX_PLY - 1), HASH::ScoreToTranpositionTable(score, plyFromRoot), (int)HASH::BOUND::EXACT);
					
				return score;
			}			

			if (depth < 1) {
				return quiescenceSearch(sData, alpha, beta, plyFromRoot + 1, inPV);
			}


			int16_t eval = sData.evalHistory[sData.threadPly] = (tte.eval != UNDEFINED) ? tte.eval : staticEval;
			bool incheck = inCheck(curPosition);
			bool nonPawnMaterial = curPosition.board.pieceLocations[curPosition.status.isWhite][BISHOP] | curPosition.board.pieceLocations[curPosition.status.isWhite][KNIGHT] | curPosition.board.pieceLocations[curPosition.status.isWhite][ROOK] | curPosition.board.pieceLocations[curPosition.status.isWhite][QUEEN];

			if (!inPV && !incheck) {
				// razoring
				if (depth <= 7 && eval < alpha - 348) {
					int value = quiescenceSearch(sData, alpha - 1, alpha, plyFromRoot + 1, false);
					if (value < alpha)
						return value;
				}
				// futility pruning
				if (depth < 7 * eval - 80 * depth >= beta && nonPawnMaterial) {
					return eval;
				}
			}

			MOVE_PICKER movePicker(
				&curPosition,
				false,
				1,
				&sData,
				MoveIntToMove(tte.move, &curPosition),
				sData.killerMoves1[sData.threadPly + 1],
				sData.killerMoves2[sData.threadPly + 1]
			);
			
			if (!movePicker.hasMoves()) {
				if (incheck) {
					return -MATE + plyFromRoot;
				}
				else {
					return 0;
				}
			}

			HASH::BOUND bound = HASH::BOUND::ALPHA;
			Move bestMoveInThisPosition;
			int bestScore = -INFINITE;
			u64 moveHash = 0ULL;
			u16 intMove = 0;
			bool DEFERRED_STAGE = false;
			Move move;
			Move_List<64> quiets;
			int movesSearched = 0;
			bool improving = sData.threadPly > 1 && sData.evalHistory[sData.threadPly] > sData.evalHistory[sData.threadPly - 2];

			while(movePicker.getMove(move)) {

				int score = 0;

				if (movesSearched == 0) {
					sData.threadPly++;
					sData.threadPositions[sData.threadPly] = curPosition;
					updatePosition(sData.threadPositions[sData.threadPly], move);
					HASH::zobrist.updateHash(curPosition, sData.threadPositions[sData.threadPly], move);
					sData.lastMove = move;

					bestScore = -alphaBeta(sData, depth - 1, plyFromRoot + 1, -beta, -alpha, inPV);

					sData.threadPly--;
				}
				else {
					bool doABDADA = depth > DEFERRED_DEPTH && movePicker.stage != STAGE_DEFERRED && !incheck;

					if (doABDADA) {
						if (!inPV && movePicker.getDeferredSize() > 0 && depth >= CUTOFF_DEPTH_CHECK) {
							tte.move = NULL;
							if (transpositionTable.probe(posHash, tte)) {
								tte.eval = HASH::TranspositionTableToScore(tte.eval, plyFromRoot);
								if (tte.depth >= depth
									&& (tte.getBound() == (int)HASH::BOUND::EXACT
										|| (tte.getBound() == (int)HASH::BOUND::ALPHA && tte.eval <= alpha)
										|| (tte.getBound() == (int)HASH::BOUND::BETA && tte.eval >= beta))) {
									return tte.eval;
								}
							}
						}

						moveHash = posHash;
						intMove = move.toIntMove();
						moveHash ^= (intMove * 1664525) + 1013904223;
						if (ABDADATable.isBusy(moveHash, intMove, depth)) {
							movePicker.addDeferredMove(move);
							continue;
						}
					}

					// make move
					sData.threadPly++;
					sData.threadPositions[sData.threadPly] = curPosition;
					updatePosition(sData.threadPositions[sData.threadPly], move);
					HASH::zobrist.updateHash(curPosition, sData.threadPositions[sData.threadPly], move);

					if (doABDADA) ABDADATable.setBusy(moveHash, intMove, depth);
					score = -alphaBeta(sData, depth - 1, plyFromRoot + 1, -alpha - 1, -alpha, false);
					if (doABDADA) ABDADATable.resetBusy(moveHash, intMove, depth);

					if (score > alpha && resourcesLeft && !sData.stopIteration ) { //&& score < beta) {
						score = -alphaBeta(sData, depth - 1, plyFromRoot + 1, -beta, -alpha, true);
					}

					// undo move
					sData.threadPly--;
				}
				movesSearched++;

				if (!resourcesLeft || sData.stopIteration)
					return CANCELLED;

				if (!(move.flag & PROMOTION) && !(move.flag & CAPTURE) && quiets.size < 63) {
					quiets.add(move);
				}

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						bound = HASH::BOUND::EXACT;
						bestMoveInThisPosition = move;
						if (score >= beta) {
							bound = HASH::BOUND::BETA;
							break;
						}
						alpha = score;
					}
				}
			}
			
			if (!inCheck && (!(bestMoveInThisPosition.flag & PROMOTION) && !(bestMoveInThisPosition.flag & CAPTURE))) {
				if (sData.killerMoves1[sData.threadPly + 1] != bestMoveInThisPosition) {
					sData.killerMoves2[sData.threadPly + 1] = sData.killerMoves1[sData.threadPly + 1];
					sData.killerMoves1[sData.threadPly + 1] = bestMoveInThisPosition;
				}
				sData.history[curPosition.status.isWhite][bestMoveInThisPosition.moved_Piece][bestMoveInThisPosition.to] += depth * depth;
				for (int i = 0; i < quiets.size - 1; i++)
					sData.history[curPosition.status.isWhite][quiets.at(i).moved_Piece][quiets.at(i).to] -= (depth * depth) / 10;
			}

			if (bound == HASH::BOUND::EXACT && !inPV)
				bound = HASH::BOUND::ALPHA;
			transpositionTable.saveEntry(posHash, bestMoveInThisPosition.toIntMove(), depth, HASH::ScoreToTranpositionTable(bestScore, plyFromRoot), (int)bound);

			return bestScore;
		}

		inline int Search_Tree::searchRoot(Search_Thread& sData, int depth, int alpha, int beta)
		{
			u64 posHash = sData.previousPositions->at(sData.gamePly).hash;
			sData.numNodes++;

			HASH::transEntry tte;
			tte.move = NULL;
			if (transpositionTable.probe(posHash, tte)) {
				tte.eval = HASH::TranspositionTableToScore(tte.eval, 0);
			}

			MOVE_PICKER movePicker(
				&sData.previousPositions->at(sData.gamePly),
				false,
				1,
				&sData,
				MoveIntToMove(tte.move, &sData.previousPositions->at(sData.gamePly)),
				sData.killerMoves1[0],
				sData.killerMoves2[0]
			);

			if (!movePicker.hasMoves()) {
				if (inCheck(sData.previousPositions->at(sData.gamePly))) {
					return -MATE;
				}
				else {
					return 0;
				}
			}

			int bestScore = -INFINITE;
			HASH::BOUND bound = HASH::BOUND::ALPHA;
			Move move;
			u64 moveHash;
			u16 intMove;
			Move_List<64> quiets;

			sData.threadPly++;
			sData.threadPositions[0] = sData.previousPositions->at(sData.gamePly);
			movePicker.getMove(move);
			updatePosition(sData.threadPositions[0], move);
			HASH::zobrist.updateHash(sData.previousPositions->at(sData.gamePly), sData.threadPositions[0], move);
			sData.lastMove = move;

			bestScore = -alphaBeta(sData, depth - 1, 1, -beta, -alpha, true);
			
			sData.threadPly--;
			if (bestScore > alpha) {
				bound = HASH::BOUND::EXACT;
				sData.bestMoveThisIteration = move;
				if (bestScore >= beta) {
					bound = HASH::BOUND::BETA;
					movePicker.stage = STAGE_FINISHED;
				}
				else
					alpha = bestScore;
			}

			while (movePicker.getMove(move))
			{

				if (depth > DEFERRED_DEPTH && movePicker.stage != STAGE_DEFERRED) {
					moveHash = posHash;
					intMove = move.toIntMove();
					moveHash ^= (intMove * 1664525) + 1013904223;
					if (ABDADATable.isBusy(moveHash, intMove, depth)) {
						movePicker.addDeferredMove(move);
						continue;
					}
				}

				// make the move
				sData.threadPly++;
				sData.threadPositions[0] = sData.previousPositions->at(sData.gamePly);
				updatePosition(sData.threadPositions[0], move);
				HASH::zobrist.updateHash(sData.previousPositions->at(sData.gamePly), sData.threadPositions[0], move);

				// introduces principal variation
				if (depth > DEFERRED_DEPTH && movePicker.stage != STAGE_DEFERRED) ABDADATable.setBusy(moveHash, intMove, depth);
				int score = -alphaBeta(sData, depth - 1, 1, -alpha - 1, -alpha, false);
				if (depth > DEFERRED_DEPTH && movePicker.stage != STAGE_DEFERRED) ABDADATable.resetBusy(moveHash, intMove, depth);
				if (score > alpha) { //&& score < beta) {
					score = -alphaBeta(sData, depth - 1, 1, -beta, -alpha, true);
				}
				

				// undo the move
				sData.threadPly--;

				if (!resourcesLeft || sData.stopIteration)
					return CANCELLED;

				if (!(move.flag & PROMOTION) && !(move.flag & CAPTURE) && quiets.size < 63) {
					quiets.add(move);
				}

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						bound = HASH::BOUND::EXACT;
						sData.bestMoveThisIteration = move;
						if (score >= beta) {
							bound = HASH::BOUND::BETA;
							break;
						}
						alpha = score;
					}
				}

			}

			if (!inCheck && (!(sData.bestMoveThisIteration.flag & PROMOTION) && !(sData.bestMoveThisIteration.flag & CAPTURE))) {
				if (sData.killerMoves1[0] != sData.bestMoveThisIteration) {
					sData.killerMoves2[0] = sData.killerMoves1[0];
					sData.killerMoves1[0] = sData.bestMoveThisIteration;
				}
				sData.history[sData.previousPositions->at(sData.gamePly).status.isWhite][sData.bestMoveThisIteration.moved_Piece][sData.bestMoveThisIteration.to] += depth * depth;
				for (int i = 0; i < quiets.size - 1; i++)
					sData.history[sData.previousPositions->at(sData.gamePly).status.isWhite][quiets.at(i).moved_Piece][quiets.at(i).to] -= (depth * depth) * 0.1;
			}

			transpositionTable.saveEntry(posHash, sData.bestMoveThisIteration.toIntMove(), depth, HASH::ScoreToTranpositionTable(bestScore, 0), (int)bound);

			return bestScore;

		}

		void Search_Tree::iterativeDeepening(Search_Thread& sData) {
			int targetDepth = MAX_ITERATIVE_DEPTH;

			int eval;

			for (int depth = DEPTH; depth <= targetDepth; depth = DEPTH) {
				int ASPIRATION = 10;
				while (true) {
					sData.stopIteration = false;

					eval = searchRoot(sData, depth, ALPHA, BETA);

					if (!resourcesLeft) {
						break;
					}
					else if (sData.stopIteration) {
						if (resolveIter) continue;
						else break;
					}
					else {

						if (sData.stopIteration) {
							if (resolveIter) continue;
							else break;
						}

						if (eval <= ALPHA) {
							BETA = (ALPHA + BETA) / 2;
							ALPHA = std::max(eval - ASPIRATION, -MATE);
						}
						else if (eval >= BETA) {
							BETA = std::min(eval + ASPIRATION, MATE);
						}
						else {
							resolveIter = false;
							if (sData.bestMoveThisIteration != NULL_MOVE)
								bestMove = sData.bestMoveThisIteration;
							DEPTH = ++depth;
							if (depth >= 5) {
								ALPHA = std::max(eval - ASPIRATION, -MATE);
								BETA = std::min(eval + ASPIRATION, MATE);
							}
							else {
								ALPHA = -MATE;
								BETA = MATE;
							}
							manager->stopIteration();
							break;
						}

						ASPIRATION += ASPIRATION / 2;
						resolveIter = true;
						manager->stopIteration();
					}

				}

				if (!resourcesLeft)
					break;

			}

			resourcesLeft = false;

		}

		Move Search_Tree::search(std::vector<Position>* positions, int ply, int MAX_TIME, Thread_Manager* manager) {

			bestMove = openingBook.getBookMove(&positions->at(ply));
			if (bestMove != NULL_MOVE)
				return bestMove;

			int wdl = SYZYGY::probeDTZ(&positions->at(ply), &bestMove);
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL) {
				return bestMove;
			}

			if (ply >= MAX_PLY - 1)
				return NULL_MOVE;

			this->manager = manager;
			resourcesLeft = true;
			DEPTH = 1;
			ALPHA = -INFINITE;
			BETA = INFINITE;
			resolveIter = false;
			transpositionTable.updateAge();

			manager->initSearch(positions, ply);
			manager->beginSearch();
			
			std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIME));
			resourcesLeft = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));


			std::cout << "nodes searched: " << manager->countNodes() << std::endl;
			
			if (bestMove == NULL_MOVE) {
				bestMove = manager->getFirstValidMove();
			}

			this->manager = nullptr;

			return bestMove;
		}

	} // namespace search
	
} // namespace kronos