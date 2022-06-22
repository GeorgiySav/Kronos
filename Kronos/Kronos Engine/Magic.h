#pragma once
#include <memory>
#include <array>
#include <memory>
#include <iostream>

#include "utility.h"

namespace KRONOS {

	template <int T>
	struct Magic {
		BitBoard mask;
		int shift;
		u64 attacks[T];
		
		Magic() : mask(0), shift(0) {
			memset(attacks, 0, sizeof(attacks));
		}

		~Magic() {}

	};

	inline std::array<Magic< 512>, 64> mBishopTable;
	inline std::array<Magic<4096>, 64> mRookTable;

	u64 generateBishopMask(int tile);	
	u64 generateRookMask(int tile);
	u64 generateRookAttack(BitBoard blockers, int tile);
	u64 generateBishopAttack(BitBoard blockers, int tile);
	extern inline BitBoard getRookAttacks(BitBoard occ, int tile);
	extern inline BitBoard getBishopAttacks(BitBoard occ, int tile);
	BitBoard occupancyFromIndex(int index, u64 mask);

	// initialises the magics and bitboards
	void initMagics();

	void deleteMagics();

}