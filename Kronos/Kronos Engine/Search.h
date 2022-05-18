#pragma once
#include <array>
#include <chrono>

#include "Move_Generation.h"
#include "Magic.h"

namespace KRONOS {
	
	namespace SEARCH {
		
#define MAX_SEARCH_DEPTH 10
#define MAX_TRANSPOSITION_TABLE_SIZE 500000
#define DEFAULT_TRANSPOSITION_TABLE_SIZE 10000

		struct TranspositionCell {
			u64 hash;
			int evaluation;
			int depth;
			bool ancient;

			TranspositionCell() : hash(0), evaluation(0), depth(0), ancient(false) {}
		};

		class TranspositionTable {
		private:
			int MAX_SIZE;
			std::vector<TranspositionCell> table;

		public:

			TranspositionTable(int size = DEFAULT_TRANSPOSITION_TABLE_SIZE) : MAX_SIZE(size) {
				table.resize(MAX_SIZE);
			}

			~TranspositionTable() { table.clear(); }

			TranspositionCell& operator [] (int hash) {
				int key = hash % MAX_SIZE;
				if (key > MAX_SIZE) {
					exit(0);
				}
				return table[key];
			}
		};

		class SearchTree {
		private:

			int ply;
			std::array<Position, MAX_SEARCH_DEPTH> positions;
			TranspositionTable transpositionTable;

			std::chrono::steady_clock clock;

		public:
			SearchTree() {
				ply = 0;
				positions[0] = Position();
			}
			~SearchTree() {

			}

			inline int alphaBeta(int alpha, int beta, int depth) {
				if (depth == 0) {
					//return evaluate(p);
				}

				std::vector<Move> moves;
				generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, &moves);
				if (moves.size() == 0) {
					return -INFINITY;
				}

				for (Move move : moves) {
					ply++;
					positions[ply] = positions[ply - 1];
					int score = alphaBeta(-alpha, -beta, depth - 1);
					if (score >= beta) {
						return beta;
					}
					else if (score > alpha) {
						alpha = score;
					}
				}
				return alpha;
			}

			inline void search(Position position, int depth, int MAX_TIME) {
				ply = 0;
				positions[0] = position;
				alphaBeta(-INFINITY, +INFINITY, depth);
			}



		};

	}


}