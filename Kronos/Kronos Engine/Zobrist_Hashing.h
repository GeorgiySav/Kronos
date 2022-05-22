#pragma once
#include <random>

#include "utility.h"
#include "Move_Generation.h"
#include "BitBoard.h"

namespace KRONOS {
	
	namespace ZOBRIST {
		
		static u64 zobristHashVals[2][6][64];
		static u64 moveBlack;
		static u64 castlingRights[4];
		static u64 enPassantFiles[8];

		static void initZobrists() {

			std::random_device rd;
			std::mt19937_64 e2(rd());
			e2.seed(NULL);
			std::uniform_int_distribution<u64> dist(0ULL, 0xFFFFFFFFFFFFFFFFULL);

			for (int c = 0; c < 2; c++) {
				for (int p = 0; p < 6; p++) {
					for (int s = 0; s < 64; s++) {
						zobristHashVals[c][p][s] = dist(e2);
					}
				}
			}

			moveBlack = dist(e2);
			
			castlingRights[0] = dist(e2);
			castlingRights[1] = dist(e2);
			castlingRights[2] = dist(e2);
			castlingRights[3] = dist(e2);

			for (int i = 0; i < 8; i++) {
				enPassantFiles[i] = dist(e2);
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

			hash ^= (position.status.WKcastle ? castlingRights[0] : 0ULL);
			hash ^= (position.status.WQcastle ? castlingRights[1] : 0ULL);
			hash ^= (position.status.BKcastle ? castlingRights[2] : 0ULL);
			hash ^= (position.status.BQcastle ? castlingRights[3] : 0ULL);

			hash ^= position.status.EP != no_Tile ? enPassantFiles[position.status.EP % 8] : 0ULL;

			hash ^= (!position.status.isWhite ? moveBlack : 0ULL);
			
			return hash;
		}

		sinline u64 updateHash(u64 hash, Move move, BoardStatus newStatus) {
			
		}

	}

}