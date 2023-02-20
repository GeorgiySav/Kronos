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
			transTable.setSize(175);
			transTable.resetAge();
			evalTable.setSize(5);
			infiniteSearching = false;
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

		void Search_Manager::stopIteration() {
			for (auto& thread : threads) {
				thread.stopIteration();
			}
		}

		void Search_Manager::waitForThreads() {
			for (auto& thread : threads) {
				while (!thread.isSleeping())
					std::this_thread::sleep_for(std::chrono::milliseconds(0));
				std::cout << "" << thread.getID() << " is sleeping\n";
			}
			std::cout << "All threads are sleeping" << std::endl;
		}

		Move Search_Manager::getBestMove(std::vector<Position>* positions, int curPly, int timeMS, int MAX_DEPTH)
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

			currentDepth = 1;
			bestMove.depth = 0;
			bestMove.move = NULL_MOVE;
			timedSearching = true;

			transTable.updateAge();

			setThreads(positions, curPly);

			beginSearch();

			auto start = std::chrono::high_resolution_clock::now();

			while (currentDepth <= MAX_DEPTH && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < std::chrono::milliseconds(timeMS).count() && timedSearching)
				std::this_thread::sleep_for(std::chrono::milliseconds(1));

			stopSearch();
			waitForThreads();

			std::cout << "Best Thread Info: " << " { Depth: " << bestMove.depth << " | Score: " << bestMove.score << " | Move: " << boardTilesStrings[bestMove.move.from] << " " << boardTilesStrings[bestMove.move.to] << " }" << std::endl;

			return bestMove.move;

		}

		bool Search_Manager::infiniteSearch(std::vector<Position>* positions, int curPly, int MAX_DEPTH)
		{
			currentDepth = 1;
			transTable.updateAge();

			currentDepth = 1;
			bestMove.depth = 0;
			bestMove.move = NULL_MOVE;

			setThreads(positions, curPly);
			beginSearch();
			
			infiniteSearching = true;

			while (true) {
				if (!infiniteSearching) {
					std::cout << "Stopped search as it was cancelled" << std::endl;
					break;
				}
				if (currentDepth > MAX_DEPTH) {
					std::cout << "Stopped search as reached max depth" << std::endl;
					break;
				}
			}

			stopSearch();
			waitForThreads();

			infiniteSearching = false;
			
			return true;
		}

	}// SEARCH
} // KRONOS