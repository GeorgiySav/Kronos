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

			extern bool initSYZYGY(const char* filePath);
			extern void freeSYZYGY();

			extern inline int probeWDL(const Position* position);
			extern inline int probeDTZ(const Position* position , Move* bestMove);
		
		}
	}
}


