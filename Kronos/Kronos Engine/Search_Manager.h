#pragma once
#include "Search.h"
#include "polyBook.h"

namespace KRONOS
{
	namespace SEARCH
	{

		// manages the search threads
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

			// resizes the number of threads
			void initSearchThreads(int numThreads);
			// prepares threads for a search
			void setThreads(std::vector<Position>* positions, int curPly);
			// wakes up all threads
			void beginSearch();
			// puts all threads to sleep
			void stopSearch();
			// calls all threads back to the deepening loop
			void stopIteration();
			// waits for threads to sleep
			void waitForThreads();

			// timed search
			Move getBestMove(std::vector<Position>* positions, int curPly, int timeMS, int MAX_DEPTH);
			// updates the best move when a new move is found at a higher depth
			void updateBestMove(Move& newMove, int depth, int score) { 
				bestMove = Search_Move(newMove, depth, score); 
				currentDepth = depth + 1;
			}

			int getCurrentDepth() { return currentDepth; }
			void updateDepth(int newDepth) { currentDepth = newDepth + 1; } // increments the interative deepening depth
			int getBestDepth() { return bestMove.depth; }

			// tells all threads that are searching a depth lower than the best depth searched to stop and begin searching a larger depth
			void callWorseThreads() {
				for (auto& thread : threads)
					if (thread.getIterDepth() < currentDepth)
						thread.stopIteration();
			}

			// passive search
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