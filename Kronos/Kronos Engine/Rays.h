#pragma once

#include "utility.h"

namespace KRONOS {
	
	enum dir {
		NORTH, EAST, SOUTH, WEST,
		NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST
	};

	static dir getDirectionFromOffset(int offset) {
		if (!(offset % 8))
			if (offset > 0)
				return NORTH;
			else
				return SOUTH;
		if (!(offset % 7))
			if (offset > 0)
				return NORTH_WEST;
			else
				return SOUTH_EAST;
		if (!(offset % 9))
			if (offset > 0)
				return NORTH_EAST;
			else
				return SOUTH_WEST;
		if (offset > 0)
			return EAST;
		return WEST;
	}

	static uint64_t rays[64][8];

	static void initRays() {
		
		for (int tile = 0; tile < 64; tile++) {

            // North
			rays[tile][NORTH] = (0x0101010101010100ULL << tile);

            // South
			rays[tile][SOUTH] = (0x0080808080808080ULL >> (63 - tile));

            // East
			rays[tile][EAST] = 2 * ((1ULL << (tile | 7)) - (1ULL << tile));

            // West
            rays[tile][WEST] = (1ULL << tile) - (1ULL << (tile & 56));

            // North West
			u64 t = EMPTY;
			for (int i = tile + 7; (i % 8 < 7) && (i < 64); i += 7) {
				setBit(t, i);
			}
			rays[tile][NORTH_WEST] = t;	
			
            // North East
			t = EMPTY;
			for (int i = tile + 9; (i % 8 > 0) && (i < 64); i += 9) {
				setBit(t, i);
			}
			rays[tile][NORTH_EAST] = t;

            // South West
			t = EMPTY;
			for (int i = tile - 9; (i % 8 < 7) && (i >= 0); i -= 9) {
				setBit(t, i);
			}
			rays[tile][SOUTH_WEST] = t;

            // South East
			t = EMPTY;
			for (int i = tile - 7; (i % 8 > 0) && (i >= 0); i -= 7) {
				setBit(t, i);
			}
			rays[tile][SOUTH_EAST] = t;

		}

	}
	
}
