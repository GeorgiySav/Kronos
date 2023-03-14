#pragma once

#include "utility.h"
#include "Move_Generation.h"

namespace KRONOS
{
	// processes the FEN and returns a position	
	extern Position    FENtoBoard(std::string FEN);
	// process the position and returns a FEN
	extern std::string BoardToFEN(const Position* position);
	// the FEN for the starting position
	static const std::string FEN_START_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
} // KRONOS