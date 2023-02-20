#pragma once
#include "Search.h"
#include "polyBook.h"

namespace KRONOS
{
	namespace SEARCH
	{

		class Search_Manager
		{
		private:
			std::vector<Search_Thread> threads;

			POLY::Opening_Book openingBook;

			Search_Move bestMove;

			std::atomic<int> currentDepth;

			bool infiniteSearching;
			bool timedSearching;

		public:
			Search_Manager();
			~Search_Manager();

			void initSearchThreads(int numThreads);
			void setThreads(std::vector<Position>* positions, int curPly);
			void beginSearch();
			void stopSearch();
			void stopIteration();
			void waitForThreads();

			Move getBestMove(std::vector<Position>* positions, int curPly, int timeMS, int MAX_DEPTH);

			void updateBestMove(Move& newMove, int depth, int score) { 
				bestMove = Search_Move(newMove, depth, score); 
				currentDepth = depth + 1;
			}

			int getCurrentDepth() { return currentDepth; }
			void updateDepth(int newDepth) { currentDepth = newDepth + 1; }
			int getBestDepth() { return bestMove.depth; }

			void callWorseThreads() {
				for (auto& thread : threads)
					if (thread.getIterDepth() < currentDepth)
						thread.stopIteration();
			}

			bool infiniteSearch(std::vector<Position>* positions, int curPly, int MAX_DEPTH);
			void cancelInfiniteSearch() { infiniteSearching = false; }
			bool isInfiniteSearching() { return infiniteSearching; }

			void cancelTimedSearch() { timedSearching = false; }

			int getCurrentScore() { return bestMove.score; }
			Move getBestMoveSoFar() { return bestMove.move; }
			int getNumberOfCores() { return threads.size(); }

			void changeTransTableSize(size_t newMB) {
				transTable.setSize(newMB);
			}

			void changeEvalTableSize(size_t newMB) {
				evalTable.setSize(newMB);
			}

			size_t getTransSize() {
				return transTable.getSizeMB();
			}

			HASH::Transposition_Table transTable;
			HASH::Eval_Table evalTable;
		};

	} // SEARCH
} // KRONOS