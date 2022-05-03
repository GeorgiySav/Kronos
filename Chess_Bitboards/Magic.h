#pragma once


#include "utility.h"
#include "BitBoard.h"

namespace CHENG {
	
	struct Magic {
		u64* attackPtr;
		u64 mask;
		u64 magic;
		int shift;
	};

	Magic rookTable[64];
	Magic bishopTable[64];

	static void initMagics() {
		
	}
	

}