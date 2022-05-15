#pragma once
#include <memory>

#include "utility.h"
#include "BitBoard.h"
#include "Rays.h"
#include "Magic_Vars.h"

namespace KRONOS {
	
	static u64 bishopAttacks[64][512] = { 0ULL };
	static u64 rookAttacks[64][4096] = { 0ULL };

	struct Magic {
		BitBoard mask;
		uShort shift;
	};

	static Magic mRookTable[64];
	static Magic mBishopTable[64];

	static u64 generateBishopMask(int tile) {
		u64 mask = 0;

		mask |= rays[tile][NORTH_EAST];
		mask |= rays[tile][NORTH_WEST];
		mask |= rays[tile][SOUTH_EAST];
		mask |= rays[tile][SOUTH_WEST];

		mask &= ~(fileMask[A] | fileMask[H] | rankMask[RANK_1] | rankMask[RANK_8]);

		return mask;
	}
	
	static u64 generateRookMask(int tile) {
		u64 mask = 0;
		
		mask |= rays[tile][NORTH] & ~(rankMask[RANK_8]);
		mask |= rays[tile][EAST] & ~(fileMask[H]);
		mask |= rays[tile][SOUTH] & ~(rankMask[RANK_1]);
		mask |= rays[tile][WEST] & ~(fileMask[A]);
		
		return mask;
	}

	static u64 generateRookAttack(BitBoard blockers, int tile) {
		u64 attacks = EMPTY;

		
		/*
		
			+ represents the rook
			1 represent occupied squares
			| ray 


			occupied & ray      ray                   not ray with bitscan
			. . . . . . . .     . . . . | . . .       | | | | . | | |
			. . . . 1 . . .		. . . . | . . .		  | | | | . | | |
			. . . . . . . .		. . . . | . . .		  | | | | | | | |
			. . . . . . . .		. . . . | . . .		  | | | | | | | |
			. . . . + . . .		. . . . + . . .		  | | | | | | | |
			. . . . . . . .		. . . . . . . .		  | | | | | | | |
			. . . . . . . .		. . . . . . . .		  | | | | | | | |
			. . . . . . . .		. . . . . . . .		  | | | | | | | |
		
		*/

		attacks |= rays[tile][NORTH];
		if (rays[tile][NORTH] & blockers) {
			attacks &= ~rays[bitScanForward((rays[tile][NORTH] & blockers))][NORTH];
		}

		attacks |= rays[tile][EAST];
		if (rays[tile][EAST] & blockers) {
			attacks &= ~rays[bitScanForward((rays[tile][EAST] & blockers))][EAST];
		}

		attacks |= rays[tile][SOUTH];
		if (rays[tile][SOUTH] & blockers) {
			attacks &= ~rays[bitScanReverse((rays[tile][SOUTH] & blockers))][SOUTH];
		}

		attacks |= rays[tile][WEST];
		if (rays[tile][WEST] & blockers) {
			attacks &= ~rays[bitScanReverse((rays[tile][WEST] & blockers))][WEST];
		}

		return attacks;

	}

	static u64 generateBishopAttack(BitBoard blockers, int tile) {
		u64 attacks = EMPTY;

		attacks |= rays[tile][NORTH_EAST];
		if (rays[tile][NORTH_EAST] & blockers) {
			attacks &= ~rays[bitScanForward((rays[tile][NORTH_EAST] & blockers))][NORTH_EAST];
		}

		attacks |= rays[tile][SOUTH_EAST];
		if (rays[tile][SOUTH_EAST] & blockers) {
			attacks &= ~rays[bitScanReverse((rays[tile][SOUTH_EAST] & blockers))][SOUTH_EAST];
		}

		attacks |= rays[tile][SOUTH_WEST];
		if (rays[tile][SOUTH_WEST] & blockers) {
			attacks &= ~rays[bitScanReverse((rays[tile][SOUTH_WEST] & blockers))][SOUTH_WEST];
		}

		attacks |= rays[tile][NORTH_WEST];
		if (rays[tile][NORTH_WEST] & blockers) {
			attacks &= ~rays[bitScanForward((rays[tile][NORTH_WEST] & blockers))][NORTH_WEST];
		}

		return attacks;

	}

	static BitBoard getRookAttacks(BitBoard occ, int tile) {
		occ &= mRookTable[tile].mask;
		occ *= rookMagics[tile];
		occ >>= mRookTable[tile].shift;
		return rookAttacks[tile][occ];
	}
	
	static BitBoard getBishopAttacks(BitBoard occ, int tile) {
		occ &= mBishopTable[tile].mask;
		occ *= bishopMagics[tile];
		occ >>= mBishopTable[tile].shift;
		return bishopAttacks[tile][occ];
	}

	static BitBoard occupancyFromIndex(int index, u64 mask) {
		u64 blockers = EMPTY;
		int bits = populationCount(mask);
		for (int i = 0; i < bits; i++) {
			int bitPos = bitScanForward(mask);
			popBit(mask, bitPos);
			if (index & (1 << i)) {
				blockers |= (1ULL << bitPos);
			}
		}
		return blockers;
	}



	// initialises the magics and bitboards
	static void initMagics() {

		u64 mask;
		int bitCount;
		int occVariations;

		for (int tile = 0; tile < 64; tile++) {

			// bishops
			mBishopTable[tile].mask = generateBishopMask(tile);
			mask = mBishopTable[tile].mask;
			bitCount = populationCount(mask);
			occVariations = 1 << bitCount;
			for (int count = 0; count < occVariations; count++) {
				u64 occ = occupancyFromIndex(count, mask);
				bishopAttacks[tile][(occ * bishopMagics[tile]) >> (64 - bishopIndexBits[tile])] = generateBishopAttack(occ, tile);
			}
			mBishopTable[tile].shift = (64 - bishopIndexBits[tile]);

			// rooks
			mRookTable[tile].mask = generateRookMask(tile);
			mask = mRookTable[tile].mask;
			bitCount = populationCount(mask);
			occVariations = 1 << bitCount;
			for (int count = 0; count < occVariations; count++) {
				u64 occ = occupancyFromIndex(count, mask);
				rookAttacks[tile][(occ * rookMagics[tile]) >> (64 - rookIndexBits[tile])] = generateRookAttack(occ, tile);
			}
			mRookTable[tile].shift = (64 - rookIndexBits[tile]);

		}
	}

}