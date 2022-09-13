#pragma once
#include "Move_Generation.h"

namespace KRONOS {
	namespace SEARCH {

		enum MOVE_PICKER_STAGES {
			STAGE_HASH_MOVE,
			STAGE_FILTER_MOVES,
			STAGE_WINNING_TACTICALS,
			STAGE_QUIETS,
			STAGE_LOSING_TACTICALS,
			STAGE_FINISHED
		};

		class Search_Thread;
		class Move_Picker {
		private:
			int stage;

			Move_List<256> moves;

			Move_List<200> quiets;
			Move_List<64> tacticals;
			Move_List<32> badTacticals;

			int index = 0;

			Position& position;

			Move hashMove;
			bool quiescenceSearch;

			int MVV_LVA(Move& move);

		public:
			Move_Picker(Position& position, bool isQuie, Move hashMove);
			~Move_Picker();

			bool nextMove(Search_Thread& sData, Move& nextMove);
			bool hasMoves() { return moves.size; }
		};

	} // SEARCH
} // KRONOS