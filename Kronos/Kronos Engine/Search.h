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

		enum NODE_TYPE {
			PV_NODE = 0,
			CUT_NODE = 1,
			ALL_NODE = -1
		};

		class SearchTree {
		private:

			std::vector<Position>* positions;

			int ply;

			HASH::Transposition_Table transpositionTable;

			Move bestMoveThisIteration;
			
			int  bestEvaluation;
			Move bestMove;

			std::chrono::steady_clock clock;
			std::chrono::steady_clock::time_point startPoint;

			int max_time;

			POLY::Opening_Book openingBook;

			EVALUATION::Evaluation evaluate;
			
			bool resourcesLeft;

			void checkResources();
			inline int quiescenceSearch(int alpha, int beta, int plyFromRoot, bool inPV);
			int alphaBeta(int depth, int plyFromRoot, int alpha, int beta, bool inPV);
			int searchRoot(int depth, int alpha, int beta);
		public:
			SearchTree();
			~SearchTree();

			bool repeated();

			inline void iterativeDeepening();

			Move search(std::vector<Position>* position, int ply, int MAX_TIME);

			void setEvalParams(EVALUATION::PARAMS::Eval_Parameters* params) {
				evaluate.setParams(params);
			}

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