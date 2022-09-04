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
#include "Thread_Manager.h"

namespace KRONOS {
	
	namespace SEARCH {

#define DEFERRED_DEPTH 3
#define CUTOFF_DEPTH_CHECK 4

		class Search_Tree {
		private:

			HASH::Transposition_Table transpositionTable;
			HASH::ABDADA_TABLE ABDADATable;
			HASH::Eval_Table evalTable;

			POLY::Opening_Book openingBook;
			Thread_Manager* manager;

			Move bestMove;
			
			std::atomic<bool> resourcesLeft;
			std::atomic<int> ALPHA;
			std::atomic<int> BETA;
			std::atomic<int> DEPTH;
			std::atomic<bool> resolveIter;
			std::atomic<bool> updateLock;

			void checkResources();
			bool repeated(Search_Thread& sData);
			inline int quiescenceSearch(Search_Thread& sData, int alpha, int beta, int plyFromRoot, bool inPV);
			int alphaBeta(Search_Thread& sData, int depth, int plyFromRoot, int alpha, int beta, bool inPV);
			int searchRoot(Search_Thread& sData, int depth, int alpha, int beta);

		public:
			Search_Tree();
			~Search_Tree();

			void iterativeDeepening(Search_Thread& sData);

			Move search(std::vector<Position>* position, int ply, int MAX_TIME, Thread_Manager* manager);

			void stop() { resourcesLeft = false; }

		};

		extern void initLMR();

	}


}