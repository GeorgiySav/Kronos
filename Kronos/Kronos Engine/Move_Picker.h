#pragma once
#include "Move_Generation.h"

namespace KRONOS {
	namespace SEARCH {

		class Search_Thread;
		class Move_Picker {
		private:
			Move_List<256> moves;
			int tacticalSize;
			int badTacticalIndex;

		public:
			Move_Picker(bool isQuie, Move hashMove);
			~Move_Picker();

			bool nextMove(Search_Thread& sData, Move& nextMove);
		};

	} // SEARCH
} // KRONOS