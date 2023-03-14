#pragma once

#include "utility.h"

#include "Move_Generation.h"

namespace KRONOS
{
	namespace SEARCH
	{
		namespace SYZYGY
		{
			extern bool syzygyInitialised;
			extern int SYZYGYLargest;

			enum class SyzygyResult : int {
				SYZYGY_LOSS,
				SYZYGY_DRAW,
				SYZYGY_WIN,
				SYZYGY_FAIL
			};

			// constructor and destsructor for the SYZYGY tablebases
			extern bool initSYZYGY(const char* filePath);
			extern void freeSYZYGY();

			// probes the score of the current position
			extern inline int probeWDL(const Position* position);
			// probes the distance to zero and returns the best move
			extern inline int probeDTZ(const Position* position , Move* bestMove);	
		}
	}
}


