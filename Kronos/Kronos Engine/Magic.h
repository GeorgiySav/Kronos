#pragma once
#include <memory>
#include <array>
#include <memory>
#include <iostream>

#include "utility.h"

namespace KRONOS {	
	// generates diagonal sliding piece attacks from a tile
	u64 generateBishopMask(int tile);	
	// generates orthogonal sliding piece attacks from a tile
	u64 generateRookMask(int tile);
	// generates diagonal sliding piece attacks with blockers from a tile
	u64 generateRookAttack(BitBoard blockers, int tile);
	// generates orthogonal sliding piece attacks with blockers from a tile
	u64 generateBishopAttack(BitBoard blockers, int tile);
	// gets diagonal attacks from the database
	extern constexpr BitBoard getBishopAttacks(BitBoard occ, int tile);
	// gets orthogonal attacks from the database
	extern constexpr BitBoard getRookAttacks(BitBoard occ, int tile);

	// initialises the magics and bitboards
	void initMagics();
}