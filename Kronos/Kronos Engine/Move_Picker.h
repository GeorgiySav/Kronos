#pragma once
#include "Move_Generation.h"
#include "Threads.h"

namespace KRONOS
{
	namespace SEARCH
	{

		enum STAGES {
			STAGE_HASH_MOVE,
			STAGE_FILTER_MOVES,
			STAGE_WINNING_CAPTURES,
			STAGE_KILLER_1,
			STAGE_KILLER_2,
			STAGE_RATE_QUIETS,
			STAGE_QUIETS,
			STAGE_LOSING_CAPTURES,
			STAGE_DEFERRED,
			STAGE_FINISHED
		};

		static const int PieceValues[6] = { 100, 300, 300, 500, 900 };

		class MOVE_PICKER
		{
		private:
			Move_List<256> moves;
			
			Move_List<200> quiets;
			int16_t quietValues[200] = { 0 };
			
			Move_List<64> captures;
			Move_List<32> badCaptures;
			int16_t values[64] = {0};
			
			Move_List<128> deferredMoves;

			int index = 0;

			int threshold = 0;
			
			const Position* position;
			Move HashMove;
			Move killer1;
			Move killer2;

			Search_Thread* thread;
			
			int MVVLVA(Move& move);

			void rateCaptures();
			void rateQuiets();

			bool skipQuiets;
		public:
			MOVE_PICKER(const Position* pos, bool isQui, int margin, Search_Thread* t, Move hMove = NULL_MOVE, Move killer1 = NULL_MOVE, Move killer2 = NULL_MOVE);
			~MOVE_PICKER();
			
			bool getMove(Move& move);

			bool hasMoves() { return moves.size > 0; }

			void addDeferredMove(Move& move) {  
				if (deferredMoves.size < 127)
					deferredMoves.add(move); 
			}
			
			int getDeferredSize() { return deferredMoves.size; }

			int stage;
		};
	}
} // KRONOS