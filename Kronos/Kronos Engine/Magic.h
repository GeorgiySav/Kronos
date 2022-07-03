#pragma once
#include <memory>
#include <array>
#include <memory>
#include <iostream>

#include "utility.h"

namespace KRONOS {

	struct Magic {
		BitBoard mask;
		int shift;
		std::vector<u64> attacks = {};
		
		Magic() : mask(0), shift(0) {
			
		}

		~Magic() {}

		void addAttack(int index, u64 attack) {
			if (index >= attacks.size())
				attacks.resize(size_t(index + 1));		
			attacks[index] = attack;
		}

	};

	inline std::array<Magic, 64> mBishopTable;
	inline std::array<Magic, 64> mRookTable;

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