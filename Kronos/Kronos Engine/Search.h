#pragma once
#include <thread>
#include <condition_variable>

#include "utility.h"
#include "Transposition_Table.h"
#include "Move_Generation.h"

namespace KRONOS
{
	namespace SEARCH
	{
		class Thread {
		protected:
			std::thread thread;
			std::condition_variable sleepCondition;
			std::mutex threadLock;
			int ID;
			bool exitFlag;
			bool sleepFlag;

		public:
			Thread(int id);
			~Thread();

			void sleep();
			void wakeup();
			void wait();

			int getID() { return ID; }
		};

		class Search_Manager;
		class Search_Thread : public Thread {
		private:
			Search_Manager& SM;

			HASH::Eval_Table evalTable;

			std::vector<Position>* previousPositions;
			int gamePly;
			std::vector<Position> threadPositions;
			int threadPly;

			Move bestMoveThisIteration;
			Move bestMove;

			EVALUATION::Evaluation eval;

			int numNodes;

			bool stop;

			bool repeatedDraw();

			int16_t quiescence(int alpha, int beta, int plyFromRoot);
			int16_t alphaBeta(int depth, int alpha, int beta, int plyFromRoot);
			int16_t root(int depth, int alpha, int beta);
			void interativeDeepening();
			void think();
			
			void setData(std::vector<Position>* prevPoss, int curPly);

		public:
			Search_Thread(int ID, Search_Manager& sm);
			Search_Thread(const Search_Thread& other);
			~Search_Thread();

			void clearData();
			void setPosition(std::vector<Position>* prevPoss, int curPly);
			void beginThink();
			void stopSearch();

			Move getBestMove() { return bestMove; }
		};
	} // SEARCH
} // KRONOS