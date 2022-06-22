#pragma once
#include <array>
#include <chrono>
#include <optional>
#include <cmath>

#include "utility.h"
#include "Transposition_Table.h"
#include "Evaluation.h"

#include "Zobrist_Hashing.h"

#include "polyBook.h"

namespace KRONOS {
	
	namespace SEARCH {
		
#define MAX_ITERATIVE_DEPTH 7
#define MAX_SEARCH_DEPTH 30
#define MAX_TRANSPOSITION_TABLE_SIZE 1000003
#define DEFAULT_TRANSPOSITION_TABLE_SIZE 1000003

#define immediateMateScore 1000000

#define USE_TRANSPOSITION_TABLE 
#define USE_OPENING_BOOK

		static const int infinity = std::numeric_limits<basic_score>::max();

		class SearchTree {
		private:

			int ply;
			std::array<Position, MAX_SEARCH_DEPTH> positions;

			HASH::ZobristGenerator zobrist;

			HASH::Transposition_Table transpositionTable;

			Move bestMoveThisIteration;
			
			int  bestEvaluation;
			Move bestMove;

			std::chrono::steady_clock clock;
			std::chrono::steady_clock::time_point startPoint;

			int max_time;

			POLY::Opening_Book openingBook;

			inline int quiescenceSearch(int alpha, int beta, int plyFromRoot);
			int alphaBeta(int depth, int plyFromRoot, int alpha, int beta);
		public:
			EVALUATION::Evaluation evaluate;
			SearchTree();
			~SearchTree();

			bool repeated();

			inline void iterativeDeepening(int targetDepth);

			Move search(Position position, int depth, int MAX_TIME);

			inline int searchWithAlphaBeta(int depth) {
				max_time = infinity;
				//transpositionTable.setAncient();
				startPoint = clock.now();
				return alphaBeta(depth, 0, -infinity, infinity);
			}

			constexpr void setPosition(const Position& pos) {
				positions[0] = pos;
			}

		};

	}


}