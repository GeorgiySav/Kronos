#include "Search_Manager.h"

#include "SyzygyTB.h"
#include "consts.h"

namespace KRONOS
{
	namespace SEARCH
	{
		Search_Manager::Search_Manager()
		{
			threads.emplace_back(0, *this);
			openingBook.readBook("./Opening Books/gm2600.bin");
			openingBook.setMode(POLY::POLY_MODE::BEST_WEIGHT);
			transTable.setSize(100);
			transTable.resetAge();
		}
		
		Search_Manager::~Search_Manager()
		{

		}

		void Search_Manager::initSearchThreads(int numThreads)
		{
			threads.clear();
			for (int i = 0; i < numThreads; i++) {
				threads.emplace_back(i, *this);
			}
		}

		void Search_Manager::setThreads(std::vector<Position>* positions, int curPly)
		{
			for (auto& thread : threads) {
				thread.setPosition(positions, curPly);
			}
		}

		void Search_Manager::beginSearch()
		{
			for (auto& thread : threads) {
				thread.beginThink();
			}
		}

		void Search_Manager::stopSearch()
		{
			for (auto& thread : threads) {
				thread.stopSearch();
			}
		}

		Move Search_Manager::getBestMove(std::vector<Position>* positions, int curPly, int timeMS)
		{
			Move m;
			m = openingBook.getBookMove(&positions->at(curPly));
			if (m != NULL_MOVE)
				return m;

			int wdl = SYZYGY::probeDTZ(&positions->at(curPly), &m);
			if (wdl != (int)SYZYGY::SyzygyResult::SYZYGY_FAIL)
				return m;

			if (curPly >= MAX_PLY)
				return NULL_MOVE;

			transTable.updateAge();

			setThreads(positions, curPly);

			beginSearch();

			std::this_thread::sleep_for(std::chrono::milliseconds(timeMS));

			stopSearch();

			return threads.at(0).getBestMove();

		}

	}// SEARCH
} // KRONOS