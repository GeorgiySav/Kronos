#include "Search.h"

#include <map>

#include "Move_Generation.h"
#include "Magic.h"

#include "Zobrist_Hashing.h"

#include "SyzygyTB.h"
#include "Move_Ordering.h"

namespace KRONOS {
	
	namespace SEARCH {

		SearchTree::SearchTree()
		{
			resourcesLeft = false;
			positions = nullptr;
			ply = 0;
			bestEvaluation = -INFINITE;
			max_time = 10000;
#ifdef USE_TRANSPOSITION_TABLE
			transpositionTable.setSize(75);
#endif // USE_TRANSPOSITION_TABLE
#ifdef USE_OPENING_BOOK
			openingBook.readBook("./Opening Books/komodo.bin");
			openingBook.setMode(POLY::POLY_MODE::BEST_WEIGHT);
#endif // USE_OPENING_BOOK
		}

		SearchTree::~SearchTree()
		{

		}
		
		void SearchTree::checkResources()
		{
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startPoint).count() > max_time) {
				resourcesLeft = false;
			}
		}

		inline int SearchTree::quiescenceSearch(int alpha, int beta, int plyFromRoot, bool inPV) {
			
			u64 posHash = HASH::zobrist.generateHash(positions->at(ply));
			positions->at(ply).setHash(posHash);

			if (repeated())
				return 0;
			
#ifdef USE_TRANSPOSITION_TABLE			
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
#endif // USE_TRANSPOSITION_TABLE

#ifdef USE_SYZYGY
			int wdl = SYZYGY::probeWDL(&positions->at(ply));
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL)
			{
				int score = wdl == (int)SYZYGY::SyzygyResult::SYZYGY_LOSS ? MATED_IN_MAX_PLY + plyFromRoot + 1
					: wdl == (int)SYZYGY::SyzygyResult::SYZYGY_WIN ? MATE_IN_MAX_PLY - plyFromRoot - 1 : 0;

				return score;
			}
#endif

			int eval = evaluate.evaluate(positions->at(ply));
			if (ply >= MAX_PLY - 1)
				return eval;

			if (eval >= beta) {
				return beta;
			}
			if (alpha < eval) {
				alpha = eval;
			}

			checkResources();
			if (!resourcesLeft)
				return CANCELLED;

			std::vector<Move> moves;
			generateMoves(positions->at(ply).status.isWhite, positions->at(ply).board, positions->at(ply).status, &moves);
			sortMoves(&positions->at(ply),
				&moves,
#ifdef USE_TRANSPOSITION_TABLE			     
				MoveIntToMove(tte.move, &positions->at(ply))
#else
				NULL_MOVE
#endif

			);

			if (moves.size() == 0) {
				if (inCheck(positions->at(ply))) {
					return -MATE + plyFromRoot;
				}
				else {
					return 0;
				}
			}
			
			int bestScore = -INFINITE;

			for (const Move& move : moves) {
				if (move.flag & CAPTURE) {

					if (move.flag & PROMOTION) {
						if ((move.flag & QUEEN_PROMOTION) != QUEEN_PROMOTION)
							continue;
					}
					
					ply++;
					positions->at(ply) = positions->at(ply - 1);
					updatePosition(positions->at(ply), move);
					int score = -quiescenceSearch(-beta, -alpha, plyFromRoot + 1, inPV);
					ply--;

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

		bool SearchTree::repeated()
		{
			if (ply < 4)
				return false;

			int count = 1;

			u64 compHash = positions->at(ply).hash;
			int index = ply - 1;

			while (index >= 0) {
				if (positions->at(index).hash == compHash) {
					count++;
					if (count >= 3)
						return true;
				}
				index--;
			}
			return false;
			

		}

		int SearchTree::alphaBeta(int depth, int plyFromRoot, int alpha, int beta, bool inPV) {

			u64 posHash = HASH::zobrist.generateHash(positions->at(ply));
			positions->at(ply).setHash(posHash);
			
			// check for repeats
			if (repeated()) {
				return 0;
			}

			basic_score staticEval = evaluate.evaluate(positions->at(ply));
			if (ply >= MAX_PLY - 1)
				return staticEval;

			// mate distance pruning
			alpha = std::max(alpha, -MATE + plyFromRoot);
			beta = std::min(beta, MATE - plyFromRoot - 1);
			if (alpha >= beta) {
				return alpha;
			}
			
			#ifdef USE_TRANSPOSITION_TABLE			

			HASH::transEntry tte;
			tte.move = NULL;
			if (transpositionTable.probe(posHash, tte)) {
				tte.eval = HASH::TranspositionTableToScore(tte.eval, plyFromRoot);
				if (!inPV && tte.depth >= depth 
					&& (tte.getBound() == (int)HASH::BOUND::EXACT
					|| (tte.getBound() == (int)HASH::BOUND::ALPHA && tte.eval <= alpha)
					|| (tte.getBound() == (int)HASH::BOUND::BETA  && tte.eval >= beta ))) {
					return tte.eval;
				}
			}
			
				
			#endif // USE_TRANSPOSITION_TABLE

			#ifdef USE_SYZYGY
			int wdl = SYZYGY::probeWDL(&positions->at(ply));
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL)
			{
				int score = wdl == (int)SYZYGY::SyzygyResult::SYZYGY_LOSS ? MATED_IN_MAX_PLY + plyFromRoot + 1
					       : wdl == (int)SYZYGY::SyzygyResult::SYZYGY_WIN ? MATE_IN_MAX_PLY - plyFromRoot - 1 : 0;

				#ifdef USE_TRANSPOSITION_TABLE
					transpositionTable.saveEntry(posHash, NULL, std::min(depth + SYZYGY::SYZYGYLargest, MAX_PLY - 1), HASH::ScoreToTranpositionTable(score, plyFromRoot), (int)HASH::BOUND::EXACT);
				#endif
				return score;
			}			
			#endif


			checkResources();
			if (!resourcesLeft)
				return CANCELLED;

			if (depth == 0) {
				return quiescenceSearch(alpha, beta, plyFromRoot + 1, inPV);
			}

			std::vector<Move> moves;
			generateMoves(positions->at(ply).status.isWhite, positions->at(ply).board, positions->at(ply).status, &moves);
			sortMoves(&positions->at(ply), 
				      &moves, 
					  #ifdef USE_TRANSPOSITION_TABLE			     
						MoveIntToMove(tte.move, &positions->at(ply))
					  #else
						NULL_MOVE
					  #endif
					   );
			
			
			if (moves.size() == 0) {
				if (inCheck(positions->at(ply))) {
					return -MATE + plyFromRoot;
				}
				else {
					return 0;
				}
			}

			HASH::BOUND bound = HASH::BOUND::ALPHA;
			Move bestMoveInThisPosition;
			int bestScore = -INFINITE;

			ply++;
			positions->at(ply) = positions->at(ply - 1);
			updatePosition(positions->at(ply), moves[0]);
			bestScore = -alphaBeta(depth - 1, plyFromRoot + 1, -beta, -alpha, true);
			ply--;
			if (bestScore > alpha) {
				bestMoveInThisPosition = moves[0];
				if (bestScore >= beta) {
					bound = HASH::BOUND::BETA;
					goto finish;
				}
				alpha = bestScore;
			}

			for (int i = 1; i < moves.size(); i++) {
				
				if (moves[i].flag & PROMOTION) {
					if ((moves[i].flag & QUEEN_PROMOTION) != QUEEN_PROMOTION)
						continue;
				}

				// make move
				ply++;
				positions->at(ply) = positions->at(ply - 1);
				updatePosition(positions->at(ply), moves[i]);
				
				int score = -alphaBeta(depth - 1, plyFromRoot + 1, -alpha - 1, -alpha, false);
				if (score > alpha && score < beta) {
					score = -alphaBeta(depth - 1, plyFromRoot + 1, -beta, -alpha, true);				
				}
				
				// undo move
				ply--;

				if (!resourcesLeft)
					return CANCELLED;

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						bound = HASH::BOUND::EXACT;
						bestMoveInThisPosition = moves[i];
						if (score >= beta) {
							bound = HASH::BOUND::BETA;
							break;
						}
						alpha = score;
					}
				}
			}
			
			finish:

#ifdef USE_TRANSPOSITION_TABLE
			transpositionTable.saveEntry(posHash, bestMoveInThisPosition.toIntMove(), depth, HASH::ScoreToTranpositionTable(bestScore, plyFromRoot), (int)bound);
#endif // USE_TRANSPOSITION_TABLE

			return bestScore;
		}

		inline int SearchTree::searchRoot(int depth, int alpha, int beta)
		{
			u64 posHash = HASH::zobrist.generateHash(positions->at(ply));
			positions->at(ply).setHash(posHash);

#ifdef USE_TRANSPOSITION_TABLE			

			HASH::transEntry tte;
			tte.move = NULL;
			if (transpositionTable.probe(posHash, tte)) {
				tte.eval = HASH::TranspositionTableToScore(tte.eval, 0);
				if (tte.depth >= depth && tte.move != NULL) {
					bestMoveThisIteration = MoveIntToMove(tte.move, &positions->at(ply));
					return tte.eval;
				}
			}
			
#endif // USE_TRANSPOSITION_TABLE

			std::vector<Move> moves;
			generateMoves(positions->at(ply).status.isWhite, positions->at(ply).board, positions->at(ply).status, &moves);
			sortMoves(&positions->at(ply),
				&moves,
#ifdef USE_TRANSPOSITION_TABLE			     
				MoveIntToMove(tte.move, &positions->at(ply))
#else
				NULL_MOVE
#endif
			);

			int bestScore = -INFINITE;
			HASH::BOUND bound = HASH::BOUND::ALPHA;

			ply++;
			positions->at(ply) = positions->at(ply - 1);
			updatePosition(positions->at(ply), moves[0]);
			bestScore = -alphaBeta(depth - 1, 1, -beta, -alpha, true);
			ply--;
			if (bestScore > alpha) {
				bestMoveThisIteration = moves[0];
				alpha = bestScore;
				// dont need to check if the score is greater than beta, because the beta will always be infinite
			}

			bool searchPV = true;
			for (int i = 1; i < moves.size(); i++)
			{
				
				if (moves[i].flag & PROMOTION) {
					if ((moves[i].flag & QUEEN_PROMOTION) != QUEEN_PROMOTION)
						continue;
				}

				// make the move
				ply++;
				positions->at(ply) = positions->at(ply - 1);
				updatePosition(positions->at(ply), moves[i]);

				// introduces principal variation
				int score = -alphaBeta(depth - 1, 1, -alpha - 1, -alpha, false);
				if (score > alpha && score < beta) {
					score = -alphaBeta(depth - 1, 1, -beta, -alpha, true);
				}
				

				// undo the move
				ply--;

				if (!resourcesLeft)
					return CANCELLED;

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						bound = HASH::BOUND::EXACT;
						bestMoveThisIteration = moves[i];
						if (score >= beta) {
							bound = HASH::BOUND::BETA;
							break;
						}
						alpha = score;
					}
				}

			}

			transpositionTable.saveEntry(posHash, bestMoveThisIteration.toIntMove(), depth, HASH::ScoreToTranpositionTable(bestScore, 0), (int)bound);

			return bestScore;

		}

		inline void SearchTree::iterativeDeepening() {
			int targetDepth = MAX_ITERATIVE_DEPTH;

#ifdef USE_TRANSPOSITION_TABLE
			transpositionTable.updateAge();
#endif // USE_TRANSPOSITION_TABLE

			int wdl = SYZYGY::probeDTZ(&positions->at(ply), &bestMove);
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL) {
				return;
			}

			resourcesLeft = true;
			startPoint = clock.now();
			for (int depth = 2; depth <= targetDepth; depth++) {

				int eval = searchRoot(depth, -INFINITE, INFINITE);

				if (!resourcesLeft)
					break;

				bestMove = bestMoveThisIteration;
			}

		}

		Move SearchTree::search(std::vector<Position>* positions, int ply, int MAX_TIME) {

#ifdef USE_OPENING_BOOK
			bestMove = openingBook.getBookMove(&positions->at(ply));
			if (bestMove != NULL_MOVE)
				return bestMove;
#endif // USE_OPENING_BOOK

			if (ply >= MAX_PLY - 1)
				return NULL_MOVE;

			this->positions = positions;
			this->ply = ply;
			max_time = MAX_TIME;
			iterativeDeepening();
			return bestMove;
		}

	} // namespace search
	
} // namespace kronos