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
#ifdef USE_SYZYGY
			if (SYZYGY::initSYZYGY("./Syzygy endgame tablebases/Tablebases/")) {
				std::cout << "initialised syzygy" << std::endl;
			}
			else
				std::cout << "failed to initialise syzygy" << std::endl;
#endif
		}

		SearchTree::~SearchTree()
		{
#ifdef USE_SYZYGY
			SYZYGY::freeSYZYGY();
#endif // USE_SYZYGY

		}
		
		void SearchTree::checkResources()
		{
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startPoint).count() > max_time) {
				resourcesLeft = false;
			}
		}

		inline int SearchTree::quiescenceSearch(int alpha, int beta, int plyFromRoot) {
			
			if (repeated())
				return 0;
			
			int eval = evaluate.evaluate(positions->at(ply));
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
			if (moves.size() == 0) {
				if (moves.size() == 0) {
					if (inCheck(positions->at(ply))) {
						return -MATE + plyFromRoot;
					}
					else {
						return 0;
					}
				}
			}
			int bestScore = -INFINITE;
			for (const Move& move : moves) {
				if (move.flag & CAPTURE) {
					ply++;
					positions->at(ply) = positions->at(ply - 1);
					updatePosition(positions->at(ply), move);
					int score = -quiescenceSearch(-beta, -alpha, plyFromRoot + 1);
					ply--;

					if (score >= beta) {				
						return score;
					}
					if (score > bestScore) {
						bestScore = score;
						if (score > alpha) {
							alpha = score;
						}
					}
				}
			}

			return alpha;
		}

		bool SearchTree::repeated()
		{
			if (ply < 4)
				return false;

			int count = 1;
			int compHash = positions->at(ply).hash;
			int index = ply;

			while (index >= 0) {
				if (positions->at(index).hash == 0)
					positions->at(index).hash = zobrist.generateHash(positions->at(index));
				if (positions->at(index).hash == compHash) {
					count++;
					if (count >= 3)
						return true;
				}
				index--;
			}
			return false;
			

		}

		template<int node_type>
		int SearchTree::alphaBeta(int depth, int plyFromRoot, int alpha, int beta) {

			// check for repeats
			if (repeated()) {
				return 0;
			}

			basic_score staticEval = evaluate.evaluate(positions->at(ply));

			// mate distance pruning
			alpha = std::max(alpha, -MATE + plyFromRoot);
			beta = std::min(beta, MATE - plyFromRoot - 1);
			if (alpha >= beta) {
				return alpha;
			}
			

			#ifdef USE_TRANSPOSITION_TABLE			
				u64 posHash = zobrist.generateHash(positions->at(ply));

				positions->at(ply).setHash(posHash);

				auto transEntry = transpositionTable.probeHash(posHash);
				if (transEntry.first) {
					if (transEntry.second->depth >= depth) {
						int score = HASH::TranspositionTableToScore(transEntry.second->eval, ply);
						int flag = transEntry.second->flag();
							if ((flag  == (int)HASH::BOUND::EXACT ||
						        (flag  == (int)HASH::BOUND::BETA  && score >= beta) ||
						        (flag  == (int)HASH::BOUND::ALPHA && score <= alpha))) {
							return score;
						}
					}
				}
			#endif // USE_TRANSPOSITION_TABLE

			#ifdef USE_SYZYGY
				if (plyFromRoot > 0)
				{
					int wdl = SYZYGY::probeWDL(&positions->at(ply));
					if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL)
					{
						int score = wdl == (int)SYZYGY::SyzygyResult::SYZYGY_LOSS ? MATED_IN_MAX_PLY + ply + 1
							: wdl == (int)SYZYGY::SyzygyResult::SYZYGY_WIN ? MATE_IN_MAX_PLY - ply - 1 : 0;

						transEntry.second->saveEntry(posHash, Move(), std::min(depth + SYZYGY::SYZYGYLargest, MAX_PLY - 1), HASH::ScoreToTranpositionTable(score, ply), UNDEFINED, (u8)HASH::BOUND::EXACT, transpositionTable.getGeneration());
						return score;
					}
				}
			#endif


			checkResources();
			if (!resourcesLeft)
				return CANCELLED;

			if (depth == 0) {
				return quiescenceSearch(alpha, beta, plyFromRoot + 1);
			}

			if (node_type != PV_NODE)
			{

			}

			std::vector<Move> moves;
			generateMoves(positions->at(ply).status.isWhite, positions->at(ply).board, positions->at(ply).status, &moves);
			sortMoves(&positions->at(ply), &moves, (transEntry.first && transEntry.second->bestMove != 0) ? MoveIntToMove(transEntry.second->bestMove, &positions->at(ply)) : NULL_MOVE);
			
			if (moves.size() == 0) {
				if (moves.size() == 0) {
					if (inCheck(positions->at(ply))) {
						return -MATE + plyFromRoot;
					}
					else {
						return 0;
					}
				}
			}

			HASH::BOUND bound = HASH::BOUND::ALPHA;
			Move bestMoveInThisPosition;
			basic_score bestScore = -INFINITE;
			int index = 0;

			for (const Move& move : moves) {
				// make move
				ply++;
				positions->at(ply) = positions->at(ply - 1);
				updatePosition(positions->at(ply), move);
				
				int score = 0;
				if (index == 0) {
					score = -alphaBeta<-node_type>(depth - 1, plyFromRoot + 1, -beta, -alpha);
				}
				else {
					score = -alphaBeta<CUT_NODE>(depth - 1, plyFromRoot + 1, -alpha - 1, -alpha);
					if (score > alpha && score < beta) {
						score = -alphaBeta<PV_NODE>(depth - 1, plyFromRoot + 1, -beta, -alpha);
					}
				}
				
				// undo move
				ply--;

				if (!resourcesLeft)
					return CANCELLED;

				if (score >= beta)
				{
#ifdef USE_TRANSPOSITION_TABLE
					bound = HASH::BOUND::BETA;
					transEntry.second->saveEntry(posHash, move, depth, HASH::ScoreToTranpositionTable(score, ply), staticEval, (int)bound, transpositionTable.getGeneration());
#endif				
					return score;
				}
				if (score > bestScore)
				{
					bestScore = score;
					if (score > alpha) {
						alpha = score;
						bestMoveInThisPosition = move;
					}
				}
			}

			cut:
#ifdef USE_TRANSPOSITION_TABLE
			transEntry.second->saveEntry(posHash, bestMoveInThisPosition, depth, HASH::ScoreToTranpositionTable(bestScore, ply), staticEval, (int)bound, transpositionTable.getGeneration());
#endif // USE_TRANSPOSITION_TABLE

			return bestScore;
		}

		inline int SearchTree::searchRoot(int depth, int alpha, int beta)
		{

#ifdef USE_TRANSPOSITION_TABLE			
			u64 posHash = zobrist.generateHash(positions->at(ply));

			positions->at(ply).setHash(posHash);

			auto transEntry = transpositionTable.probeHash(posHash);
			if (transEntry.first) {
				if (transEntry.second->depth >= depth) {
					int score = HASH::TranspositionTableToScore(transEntry.second->eval, ply);
					int flag = transEntry.second->flag();
					
				}
			}
#endif // USE_TRANSPOSITION_TABLE

			std::vector<Move> moves;
			generateMoves(positions->at(ply).status.isWhite, positions->at(ply).board, positions->at(ply).status, &moves);
			sortMoves(&positions->at(ply), &moves, (transEntry.first == true) ? MoveIntToMove(transEntry.second->bestMove, &positions->at(ply)) : NULL_MOVE);
			
			int bestScore = -INFINITE;
			HASH::BOUND bound = HASH::BOUND::ALPHA;

			for (int i = 0; i < moves.size(); i++)
			{
				// make the move
				ply++;
				positions->at(ply) = positions->at(ply - 1);
				updatePosition(positions->at(ply), moves[i]);

				int score = 0;
				// introduces principal variation
				if (i == 0) {
					score = -alphaBeta<PV_NODE>(depth - 1, 1, -beta, -alpha);
				}
				else {
					score = -alphaBeta<CUT_NODE>(depth - 1, 1, -alpha - 1, -alpha) > alpha;
					if (score > alpha && score < beta) {
						score = -alphaBeta<PV_NODE>(depth - 1, 1, -beta, -alpha);
					}
				}

				// undo the move
				ply--;

				if (!resourcesLeft)
					return CANCELLED;

				if (score > bestScore) {
					bestScore = score;
					if (score > alpha) {
						bound = HASH::BOUND::EXACT;
						alpha = score;
						bestMoveThisIteration = moves[i];
					}
				}

			}

		}

		inline void SearchTree::iterativeDeepening(int targetDepth) {
			targetDepth = std::min(targetDepth, MAX_ITERATIVE_DEPTH);

#ifdef USE_TRANSPOSITION_TABLE
			transpositionTable.startSearch();
#endif // USE_TRANSPOSITION_TABLE

			int wdl = SYZYGY::probeDTZ(&positions->at(ply), &bestMove);
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL) {
				return;
			}

			resourcesLeft = true;
			startPoint = clock.now();
			for (int depth = 1; depth <= targetDepth; depth++) {

				int eval = searchRoot(depth, -INFINITE, INFINITE);

				if (!resourcesLeft)
					break;

				bestMove = bestMoveThisIteration;
			}

		}

		Move SearchTree::search(std::vector<Position>* positions, int ply, int depth, int MAX_TIME) {
			
#ifdef USE_OPENING_BOOK
			bestMove = openingBook.getBookMove(&positions->at(ply));
			if (bestMove != Move())
				return bestMove;
#endif // USE_OPENING_BOOK

			this->positions = positions;
			this->ply = ply;
			max_time = MAX_TIME;
			iterativeDeepening(depth);
			return bestMove;
		}

	} // namespace search
	
} // namespace kronos