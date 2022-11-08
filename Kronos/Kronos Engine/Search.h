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
			bool isSleeping() { return sleepFlag; }

			int getID() { return ID; }
		};

		extern void initVars();

		struct Search_Move {
			Move move;
			int depth;
			int score;

			Search_Move() : move(NULL_MOVE), depth(0), score(0) {}
			Search_Move(Move move, int depth, int score) : move(move), depth(depth), score(score) {}
		};

		class Search_Manager;
		class Search_Thread : public Thread {
		private:
			Search_Manager& SM;

			std::vector<Position>* previousPositions;
			int gamePly;
			std::vector<Position> threadPositions;
			int threadPly;

			Move bestMoveThisIteration;
			Search_Move bestMove;

			int16_t historyTable[2][6][64];
			std::vector<Move> killer1;
			std::vector<Move> killer2;

			std::vector<int16_t> evalHistory;

			EVALUATION::Evaluation eval;

			int numNodes;

			bool stop;
			bool stopIter;

			bool repeatedDraw();
			void updateHistory(Move& newMove, Move_List<64>& quiets, bool side, int depth);
			void updateKillers(Move& newMove);

			int16_t quiescence(int alpha, int beta, int plyFromRoot, bool inPV);
			int16_t alphaBeta(int depth, int alpha, int beta, int plyFromRoot, bool inPV);
			int16_t root(int depth, int alpha, int beta);
			void interativeDeepening();
			void think();
			
			void setData(std::vector<Position>* prevPoss, int curPly);

		public:
			Search_Thread(int ID, Search_Manager& sm);
			Search_Thread(const Search_Thread& other);
			~Search_Thread();

			int16_t getHistoryValue(bool side, Move& move);

			void clearData();
			void setPosition(std::vector<Position>* prevPoss, int curPly);
			void beginThink();
			void stopSearch();
			void stopIteration() { stopIter = true; };

			Search_Move getBestMove() { return bestMove; }
		};
	} // SEARCH
} // KRONOS