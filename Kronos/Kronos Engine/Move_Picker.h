#pragma once
#include "Move_Generation.h"

namespace KRONOS {
	namespace SEARCH {

		enum MOVE_PICKER_STAGES {
			STAGE_HASH_MOVE,
			STAGE_FILTER_MOVES,
			STAGE_WINNING_TACTICALS,
			STAGE_KILLER_1,
			STAGE_KILLER_2,
			STAGE_QUIETS,
			STAGE_LOSING_TACTICALS,
			STAGE_FINISHED
		};

		class Search_Thread;
		class Move_Picker {
		private:
			int stage;

			Move_List moves;

			Move_List quiets;
			Move_List tacticals;
			Move_List badTacticals;

			int index = 0;

			Position& position;

			Move hashMove;
			Move killer1;
			Move killer2;

			bool quiescenceSearch;

			int MVV_LVA(Move& move);

		public:
			Move_Picker(Position& position, bool isQuie, Move hashMove, Move k1, Move k2);
			~Move_Picker();

			bool nextMove(Search_Thread& sData, Move& nextMove);
			bool hasMoves() { return moves.size(); }

			int getStage() { return stage; }


		};

	} // SEARCH
} // KRONOS