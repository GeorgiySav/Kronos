#pragma once
#include "Move_Generation.h"

namespace KRONOS
{
	namespace SEARCH
	{
		extern inline void sortMoves(Position* position, std::vector<Move>* moves, Move HashMove);	
	}
} // KRONOS