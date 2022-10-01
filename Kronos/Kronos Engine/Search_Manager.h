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

			Move bestMove;

		public:
			Search_Manager();
			~Search_Manager();

			void initSearchThreads(int numThreads);
			void setThreads(std::vector<Position>* positions, int curPly);
			void beginSearch();
			void stopSearch();
			void stopIteration();
			void waitForThreads();

			Move getBestMove(std::vector<Position>* positions, int curPly, int timeMS);

			void updateBestMove(Move& newMove) {
				bestMove = newMove;
			}

			HASH::Transposition_Table transTable;
			HASH::Eval_Table evalTable;
		};

	} // SEARCH
} // KRONOS