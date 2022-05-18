#pragma once
#include <random>

#include "utility.h"
#include "Move_Generation.h"
#include "BitBoard.h"

namespace KRONOS {
	
	namespace ZOBRIST {
		
		static u64 zobristHashVals[2][6][64];

		static void initZobrists() {

			std::random_device rd;
			std::mt19937_64 e2(rd());
			std::uniform_int_distribution<u64> dist(0ULL, 0xFFFFFFFFFFFFFFFFULL);

			for (int c = 0; c < 2; c++) {
				for (int p = 0; p < 6; p++) {
					for (int s = 0; s < 64; s++) {
						zobristHashVals[c][p][s] = dist(e2);
					}
				}
			}

		}

		sinline u64 getHash(Position position) {
			u64 hash = 0ULL;
			int pos;
			for (int c = 0; c < 2; c++) {
				for (int p = 0; p < 6; p++) {		
					while (position.board.pieceLocations[c][p]) {
						pos = bitScanForward(position.board.pieceLocations[c][p]);
						popBit(position.board.pieceLocations[c][p], pos);
						hash ^= zobristHashVals[c][p][pos];
					}
				}
			}
			return hash;
		}

		CompileTime void updateHash(u64& hash, int from, int to, bool isWhite, int piece) {
			hash ^= zobristHashVals[isWhite][piece][from];
			hash ^= zobristHashVals[isWhite][piece][to];
		}

	}

}