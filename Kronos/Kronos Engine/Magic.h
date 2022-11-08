#pragma once
#include <memory>
#include <array>
#include <memory>
#include <iostream>

#include "utility.h"

namespace KRONOS {

	template<int tblSize>
	struct Magic {
		BitBoard mask;
		int shift;
		u64 attacks[tblSize] = {};
		
		Magic() : mask(0), shift(0) {
			
		}

		~Magic() {}

		void addAttack(int index, u64 attack) {
			attacks[index] = attack;
		}

	};

	u64 generateBishopMask(int tile);	
	u64 generateRookMask(int tile);
	u64 generateRookAttack(BitBoard blockers, int tile);
	u64 generateBishopAttack(BitBoard blockers, int tile);
	extern constexpr BitBoard getRookAttacks(BitBoard occ, int tile);
	extern constexpr BitBoard getBishopAttacks(BitBoard occ, int tile);
	BitBoard occupancyFromIndex(int index, u64 mask);

	// initialises the magics and bitboards
	void initMagics();

	void deleteMagics();

}