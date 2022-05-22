#pragma once
#include <array>
#include <chrono>
#include <optional>
#include <cmath>

#include "Evaluation.h"
#include "Move_Generation.h"
#include "Magic.h"

namespace KRONOS {
	
	namespace SEARCH {
		
#define ITERATIVE_DEPTH 8
#define MAX_SEARCH_DEPTH 50
#define MAX_TRANSPOSITION_TABLE_SIZE 1000000
#define DEFAULT_TRANSPOSITION_TABLE_SIZE 521447

#define immediateMateScore 10000000

		static const int infinity = std::numeric_limits<int>::max();
		
		enum class EvalType : int {
			BETA,
			ALPHA,
			EXACT
		};

		class TranspositionTable {
		private:
			struct TranspositionCell {
				u64 hash;
				EvalType evalFlag;
				int evaluation;
				int depth;
				bool ancient;
				Move bestMove;

				constexpr TranspositionCell() : hash(0), evalFlag(EvalType::ALPHA), evaluation(0), depth(0), ancient(true), bestMove(Move()) {}
				constexpr TranspositionCell(u64 hash, EvalType evalFlag, int evaluation, int depth, bool ancient, Move bestMove) : hash(hash), evalFlag(evalFlag), evaluation(evaluation), depth(depth), ancient(ancient), bestMove(bestMove) {}
			};

			int TABLE_SIZE;
			std::vector<TranspositionCell> table;

		public:

			TranspositionTable(int size = DEFAULT_TRANSPOSITION_TABLE_SIZE) : TABLE_SIZE(size) {
				table.resize(TABLE_SIZE);
			}

			~TranspositionTable() { table.clear(); }

			constexpr void addCell(u64 hash, EvalType evalFlag, int evaluation, int depth, bool ancient, Move bestMove) {
				int index = hash % TABLE_SIZE;
				if (table[index].ancient || table[index].depth < depth || evalFlag > table[index].evalFlag) {
					table[index] = TranspositionCell(hash, evalFlag, evaluation, depth, ancient, bestMove);
				}
			}

			std::optional<TranspositionCell> getCell(u64 hash) {
				if ((hash % TABLE_SIZE) < TABLE_SIZE && table[hash % TABLE_SIZE].hash == hash) {
					return table[hash % TABLE_SIZE];
				}
				else {
					return {};
				}
			}

			void clear() {
				table.clear();
				table.resize(TABLE_SIZE);
			}

		};

		class SearchTree {
		private:

			int ply;
			std::array<Position, MAX_SEARCH_DEPTH> positions;
			TranspositionTable transpositionTable;

			Move bestMoveThisIteration;
			
			int  bestEvaluation;
			Move bestMove;

			std::chrono::steady_clock clock;
			std::chrono::steady_clock::time_point startPoint;

			int max_time;

			EVALUATION::Evaluation evaluate;

		public:
			SearchTree() {
				ply = 0;
				positions[0] = Position();
				bestEvaluation = -infinity;
				max_time = 10000;
			}
			~SearchTree() {

			}

			inline int quiescenceSearch(int alpha, int beta) {
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
					if (inCheck(positions[ply])) {
						return -immediateMateScore;
					}
					else {
						return 0;
					}
				}

				for (const Move& move : moves) {
					if (move.flag & CAPTURE) {
						ply++;
						positions[ply] = positions[ply - 1];
						updatePosition(positions[ply], move);
						int score = -quiescenceSearch(-beta, -alpha);
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

			inline int alphaBeta(int depth, int plyFromRoot, int alpha, int beta) {
				if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startPoint).count() > max_time) {
					return 0;
				}
				
				u64 posHash = ZOBRIST::getHash(positions[ply]);
				
				if (plyFromRoot) {
					alpha = std::max(alpha, -immediateMateScore + plyFromRoot);
					beta = std::min(beta, immediateMateScore - plyFromRoot);
					if (alpha >= beta) {
						return alpha;
					}
				}

				if (auto transposition = transpositionTable.getCell(posHash)) {
					if (transposition.value().ancient && transposition.value().depth >= depth) {
						transposition.value().ancient = false;
						if (plyFromRoot == 0)
							bestMoveThisIteration = transposition.value().bestMove;
						return transposition.value().evaluation;
					}
				}
				
				if (depth == 0) {
					return quiescenceSearch(alpha, beta);
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

				EvalType evalFlag = EvalType::ALPHA;
				Move bestMoveInThisPosition;

				for (const Move& move : moves) {
					ply++;
					positions[ply] = positions[ply - 1];
					updatePosition(positions[ply], move);
					int score = -alphaBeta(depth - 1, plyFromRoot + 1, -beta, -alpha);
					ply--;
					
					if (score >= beta) {
						transpositionTable.addCell(posHash, EvalType::BETA, beta, depth, true, move);
						// beta cut off
						return beta;
					}
					if (score > alpha) {
						evalFlag = EvalType::EXACT;
						alpha = score;
						bestMoveInThisPosition = move;
						if (plyFromRoot == 0) {
							bestMoveThisIteration = move;
						}
					}
				}

				transpositionTable.addCell(posHash, evalFlag, alpha, depth, true, bestMoveInThisPosition);

				return alpha;	
			}

			inline void iterativeDeepening(int targetDepth) {
				targetDepth = targetDepth < ITERATIVE_DEPTH ? targetDepth : ITERATIVE_DEPTH;
				
				startPoint = clock.now();

				for (int depth = 1; depth <= targetDepth; depth++) {
					alphaBeta(depth, 0, -infinity, infinity);
					
					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startPoint).count() > max_time) {
						break;
					}

					bestMove = bestMoveThisIteration;
					
				}

			}

			inline Move search(Position position, int depth, int MAX_TIME) {
				ply = 0;
				positions[0] = position;
				max_time = MAX_TIME;
				iterativeDeepening(depth);
				return bestMove;
			}



		};

	}


}