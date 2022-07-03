#include "Magic.h"

#include "BitBoard.h"
#include "Rays.h"
#include "Magic_Vars.h"

namespace KRONOS
{
	
	u64 generateBishopMask(int tile) {
		u64 mask = 0ULL;

		mask |= rays[rayIndex(tile, NORTH_EAST)];
		mask |= rays[rayIndex(tile, NORTH_WEST)];
		mask |= rays[rayIndex(tile, SOUTH_EAST)];
		mask |= rays[rayIndex(tile, SOUTH_WEST)];

		mask &= ~(fileMask[A] | fileMask[H] | rankMask[RANK_1] | rankMask[RANK_8]);

		return mask;
	}

	u64 generateRookMask(int tile) {
		u64 mask = 0ULL;

		mask |= rays[rayIndex(tile, NORTH)] & ~(rankMask[RANK_8]);
		mask |= rays[rayIndex(tile, EAST)] & ~(fileMask[H]);
		mask |= rays[rayIndex(tile, SOUTH)] & ~(rankMask[RANK_1]);
		mask |= rays[rayIndex(tile, WEST)] & ~(fileMask[A]);

		return mask;
	}

	u64 generateRookAttack(BitBoard blockers, int tile) {
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

		attacks |= rays[rayIndex(tile, NORTH)];
		if (rays[rayIndex(tile, NORTH)] & blockers) {
			attacks &= ~rays[rayIndex(bitScanForward((rays[rayIndex(tile, NORTH)] & blockers)), NORTH)];
		}

		attacks |= rays[rayIndex(tile, EAST)];
		if (rays[rayIndex(tile, EAST)] & blockers) {
			attacks &= ~rays[rayIndex(bitScanForward((rays[rayIndex(tile, EAST)] & blockers)), EAST)];
		}

		attacks |= rays[rayIndex(tile, SOUTH)];
		if (rays[rayIndex(tile, SOUTH)] & blockers) {
			attacks &= ~rays[rayIndex(bitScanReverse((rays[rayIndex(tile, SOUTH)] & blockers)), SOUTH)];
		}

		attacks |= rays[rayIndex(tile, WEST)];
		if (rays[rayIndex(tile, WEST)] & blockers) {
			attacks &= ~rays[rayIndex(bitScanReverse((rays[rayIndex(tile, WEST)] & blockers)), WEST)];
		}

		return attacks;

	}

	u64 generateBishopAttack(BitBoard blockers, int tile) {
		u64 attacks = EMPTY;

		attacks |= rays[rayIndex(tile, NORTH_EAST)];
		if (rays[rayIndex(tile, NORTH_EAST)] & blockers) {
			attacks &= ~rays[rayIndex(bitScanForward((rays[rayIndex(tile, NORTH_EAST)] & blockers)), NORTH_EAST)];
		}

		attacks |= rays[rayIndex(tile, SOUTH_EAST)];
		if (rays[rayIndex(tile, SOUTH_EAST)] & blockers) {
			attacks &= ~rays[rayIndex(bitScanReverse((rays[rayIndex(tile, SOUTH_EAST)] & blockers)), SOUTH_EAST)];
		}

		attacks |= rays[rayIndex(tile, SOUTH_WEST)];
		if (rays[rayIndex(tile, SOUTH_WEST)] & blockers) {
			attacks &= ~rays[rayIndex(bitScanReverse((rays[rayIndex(tile, SOUTH_WEST)] & blockers)), SOUTH_WEST)];
		}

		attacks |= rays[rayIndex(tile, NORTH_WEST)];
		if (rays[rayIndex(tile, NORTH_WEST)] & blockers) {
			attacks &= ~rays[rayIndex(bitScanForward((rays[rayIndex(tile, NORTH_WEST)] & blockers)), NORTH_WEST)];
		}
		return attacks;

	}

	inline BitBoard getRookAttacks(BitBoard occ, int tile) {
		occ &= mRookTable[tile].mask;
		occ *= rookMagics[tile];
		occ >>= mRookTable[tile].shift;
		return mRookTable[tile].attacks.at(occ);
	}

	inline BitBoard getBishopAttacks(BitBoard occ, int tile) {
		occ &= mBishopTable[tile].mask;
		occ *= bishopMagics[tile];
		occ >>= mBishopTable[tile].shift;
		return mBishopTable[tile].attacks.at(occ);
	}

	BitBoard occupancyFromIndex(int index, u64 mask) {
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
	void initMagics() {

		u64 mask = 0ULL;
		int bitCount = 0;
		int occVariations = 0;
		int shift = 0;

		for (int tile = 0; tile < 64; tile++) {

			// bishops
			mask = generateBishopMask(tile);
			mBishopTable[tile].mask = mask;
			bitCount = populationCount(mask);
			occVariations = 1 << bitCount;
			shift = 64 - bitCount;
			for (int count = 0; count <= occVariations; count++) {
				u64 occ = occupancyFromIndex(count, mask);
				int key = (occ * bishopMagics[tile]) >> shift;
				mBishopTable[tile].addAttack(key, generateBishopAttack(occ, tile));
			}
			mBishopTable[tile].shift = shift;

			// rooks
			mask = generateRookMask(tile);
			mRookTable[tile].mask = mask;
			bitCount = populationCount(mask);
			occVariations = 1 << bitCount;
			shift = 64 - bitCount;
			for (int count = 0; count < occVariations; count++) {
				u64 occ = occupancyFromIndex(count, mask);
				int key = (occ * rookMagics[tile]) >> shift;
				mRookTable[tile].addAttack(key, generateRookAttack(occ, tile));
			}
			mRookTable[tile].shift = shift;

		}

		for (auto& attack : mBishopTable[0].attacks)
			std::cout << _BitBoard(attack) << std::endl;
	}

	void deleteMagics()
	{
		
	}

} // namespace KRONOS