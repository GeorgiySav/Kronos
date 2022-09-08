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

		public:
			Search_Manager();
			~Search_Manager();

			void initSearchThreads(int numThreads);
			void setThreads(std::vector<Position>* positions, int curPly);
			void beginSearch();
			void stopSearch();

			Move getBestMove(std::vector<Position>* positions, int curPly, int timeMS);
			
			HASH::Transposition_Table transTable;
		};

	} // SEARCH
} // KRONOS