#include "Search.h"

#include "Move_Generation.h"
#include "Magic.h"

#include "Zobrist_Hashing.h"

namespace KRONOS {
	
	namespace SEARCH {

		SearchTree::SearchTree()
		{
			ply = 0;
			positions[0] = Position();
			bestEvaluation = -infinity;
			max_time = 10000;
#ifdef USE_TRANSPOSITION_TABLE
			transpositionTable.setSize(75);
#endif // USE_TRANSPOSITION_TABLE
#ifdef USE_OPENING_BOOK
			openingBook.readBook("./Opening Books/codekiddy.bin");
			openingBook.setMode(POLY::POLY_MODE::BEST_WEIGHT);
#endif // USE_OPENING_BOOK

		}

		SearchTree::~SearchTree()
		{
			
		}
		
		inline int SearchTree::quiescenceSearch(int alpha, int beta, int plyFromRoot) {
			
			if (repeated())
				return 0;
			
			int eval = evaluate.evaluate(positions[ply]);
			if (eval >= beta) {
				return beta;
			}
			if (alpha < eval) {
				alpha = eval;
			}

			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startPoint).count() > max_time) {
				return alpha;
			}

			std::vector<Move> moves;
			generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, &moves);
			if (moves.size() == 0) {
				if (moves.size() == 0) {
					if (inCheck(positions[ply])) {
						return -immediateMateScore + plyFromRoot;
					}
					else {
						return 0;
					}
				}
			}

			for (const Move& move : moves) {
				if (move.flag & CAPTURE) {
					ply++;
					positions[ply] = positions[ply - 1];
					updatePosition(positions[ply], move);
					int score = -quiescenceSearch(-beta, -alpha, plyFromRoot + 1);
					ply--;

					if (score >= beta) {
						// beta cut off
						return beta;
					}
					if (score > alpha) {
						alpha = score;
					}
				}
			}

			return alpha;
		}

		bool SearchTree::repeated()
		{
			if (ply < 4)
				return false;

			std::vector<Position*> positionsToCheck;
			int index = 0;
			while (positions[index].hash) {
				positionsToCheck.push_back(&positions[index]);
				index++;
			}
			std::sort(positionsToCheck.begin(), positionsToCheck.end());
			for (int i = 0; i < positionsToCheck.size() - 1; i++) {
				if (positionsToCheck[i] == positionsToCheck[i + 1] && positionsToCheck[i] == positionsToCheck[i + 2]) {
					return true;
				}
			}
			return false;

		}

		int SearchTree::alphaBeta(int depth, int plyFromRoot, int alpha, int beta) {
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startPoint).count() > max_time) {
				return 0;
			}

			#ifdef USE_TRANSPOSITION_TABLE			
			u64 posHash = zobrist.generateHash(positions[ply]);

			positions[ply].setHash(posHash);

			// check for repeats
			if (repeated()) {
				return 0;
			}

			auto transEntry = transpositionTable.probeHash(posHash);
			if (transEntry.first) {
				if (depth <= transEntry.second->depth) {
					if (plyFromRoot == 0) {
						bestMoveThisIteration = transEntry.second->bestMove;
					}
					return transEntry.second->eval;
				}
			}
			#endif // USE_TRANSPOSITION_TABLE

			if (plyFromRoot) {
				alpha = std::max(alpha, -immediateMateScore + plyFromRoot);
				beta = std::min(beta, immediateMateScore - plyFromRoot - 1);
				if (alpha >= beta) {
					return alpha;
				}
			}


			if (depth == 0) {
				return quiescenceSearch(alpha, beta, plyFromRoot + 1);
			}



			std::vector<Move> moves;
			generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, &moves);
			if (moves.size() == 0) {
				if (moves.size() == 0) {
					if (inCheck(positions[ply])) {
						return -immediateMateScore + plyFromRoot;
					}
					else {
						return 0;
					}
				}
			}

			HASH::BOUND bound = HASH::BOUND::ALPHA;
			Move bestMoveInThisPosition;

			for (const Move& move : moves) {
				ply++;
				positions[ply] = positions[ply - 1];
				updatePosition(positions[ply], move);
				int score = -alphaBeta(depth - 1, plyFromRoot + 1, -beta, -alpha);
				ply--;

				if (score >= beta) {
#ifdef USE_TRANSPOSITION_TABLE
					transEntry.second->saveEntry(move, (HASH::HashLower)(posHash), beta, depth, HASH::BOUND::BETA);
#endif // USE_TRANSPOSITION_TABLE

					// beta cut off
					return beta;
				}
				if (score > alpha) {
					bound = HASH::BOUND::EXACT;
					alpha = score;
					bestMoveInThisPosition = move;
					if (plyFromRoot == 0) {
						bestMoveThisIteration = move;
					}
				}
			}

#ifdef USE_TRANSPOSITION_TABLE
			transEntry.second->saveEntry(bestMoveInThisPosition, (HASH::HashLower)(posHash), alpha, depth, bound);
#endif // USE_TRANSPOSITION_TABLE

			return alpha;
		}

		inline void SearchTree::iterativeDeepening(int targetDepth) {
			targetDepth = std::min(targetDepth, MAX_ITERATIVE_DEPTH);

			startPoint = clock.now();

			for (int depth = 1; depth <= targetDepth; depth++) {
				alphaBeta(depth, 0, -infinity, infinity);
				bestMove = bestMoveThisIteration;

				if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startPoint).count() > max_time) {
					break;
				}


			}

		}

		Move SearchTree::search(Position position, int depth, int MAX_TIME) {
			
#ifdef USE_OPENING_BOOK
			bestMove = openingBook.getBookMove(position);
			if (bestMove != Move())
				return bestMove;
#endif // USE_OPENING_BOOK


			ply = 0;
			positions[0] = position;
			max_time = MAX_TIME;
			iterativeDeepening(depth);
			transpositionTable.setAncient();
			return bestMove;
		}

	} // namespace search
	
} // namespace kronos