#pragma once

#include "utility.h"
#include "BitBoard.h"

namespace CHENG {
	
	static u64 bishopAttacks[64][512];
	static u64 rookAttacks[64][4096];

	struct Magic {
		BitBoard* attackPtr;
		BitBoard mask;
		u64 magic;
		uShort shift;
	};

	static Magic mRookTable[64];
	static Magic mBishopTable[64];

	static u64 generateBishopMask(uShort tile) {
		u64 mask = 0;

		// north east
		for (uShort i = tile; (i % 8 < 7) && (i < 56); i += 9) {
			setBit(mask, i);
		}

		// north west
		for (uShort i = tile; (i % 8 > 0) && (i < 56); i += 7) {
			setBit(mask, i);
		}

		// south east
		for (uShort i = tile; (i % 8 < 7) && (i > 7); i -= 7) {
			setBit(mask, i);
		}

		// south west
		for (uShort i = tile; (i % 8 > 0) && (i > 7); i -= 9) {
			setBit(mask, i);
		}

		popBit(mask, tile);

		return mask;
	}
	
	static u64 generateRookMask(uShort tile) {
		u64 mask = 0;
		
		for (uShort i = tile; i % 8 < 7; ++i) {
			setBit(mask, i);
		}

		for (uShort i = tile; i % 8 > 0; --i) {
			setBit(mask, i);
		}

		for (uShort i = tile; i < 56; i += 8) {
			setBit(mask, i);
		}

		for (uShort i = tile; i > 7; i -= 8) {
			setBit(mask, i);
		}

		popBit(mask, tile);
		
		return mask;
	}


	static BitBoard getRookAttacks(BitBoard occ, uShort tile) {
		u64* ptr = mRookTable[tile].attackPtr;
		occ &= mRookTable[tile].mask;
		occ *= mRookTable[tile].magic;
		occ >>= mRookTable[tile].shift;
		return ptr[occ];
	}
	
	static BitBoard getBishopAttacks(BitBoard occ, uShort tile) {
		u64* ptr = mBishopTable[tile].attackPtr;
		occ &= mBishopTable[tile].mask;
		occ *= mBishopTable[tile].magic;
		occ >>= mBishopTable[tile].shift;
		return ptr[occ];
	}

	// initialises the magics and bitboards
	static void initSliders() {

		for (int tile = 0; tile < 64; tile++) {

			// bishops
			mBishopTable[tile].mask = generateBishopMask(tile);


			// rooks
			mRookTable[tile].mask = generateRookMask(tile);

		}
	}

}