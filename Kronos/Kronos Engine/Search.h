#pragma once
#include <array>
#include <chrono>
#include <optional>
#include <cmath>

#include "consts.h"
#include "utility.h"
#include "Transposition_Table.h"
#include "Evaluation.h"

#include "Zobrist_Hashing.h"

#include "polyBook.h"

namespace KRONOS {
	
	namespace SEARCH {

#define USE_TRANSPOSITION_TABLE 
#define USE_OPENING_BOOK
#define USE_SYZYGY

		enum NODE_TYPE : u8 {
			PV_NODE = 0,
			CUT_NODE = 1,
			ALL_NODE = -1
		};

		class SearchTree {
		private:

			std::vector<Position>* positions;

			int ply;

			HASH::ZobristGenerator zobrist;

			HASH::Transposition_Table transpositionTable;

			Move bestMoveThisIteration;
			
			int  bestEvaluation;
			Move bestMove;

			std::chrono::steady_clock clock;
			std::chrono::steady_clock::time_point startPoint;

			int max_time;

			POLY::Opening_Book openingBook;

			bool resourcesLeft;

			void checkResources();
			inline int quiescenceSearch(int alpha, int beta, int plyFromRoot);
			template <int node_type>
			int alphaBeta(int depth, int plyFromRoot, int alpha, int beta);
			int searchRoot(int depth, int alpha, int beta);
		public:
			EVALUATION::Evaluation evaluate;
			SearchTree();
			~SearchTree();

			bool repeated();

			inline void iterativeDeepening(int targetDepth);

			Move search(std::vector<Position>* position, int ply, int depth, int MAX_TIME);

			//inline int searchWithAlphaBeta(std::vector<Position>* positions, int ply, int depth) {
			//	max_time = INFINITE;
			//	this->positions = positions;
			//	this->ply = ply;
			//	startPoint = clock.now();
			//	return alphaBeta(depth, 0, -INFINITE, INFINITE);
			//}

		};

	}


}