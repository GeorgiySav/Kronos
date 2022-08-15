#include "Zobrist_Hashing.h"

namespace KRONOS {
	
	namespace HASH {

		ZobristGenerator::ZobristGenerator()
		{
			std::mt19937_64 gen(std::mt19937_64::default_seed);
			std::uniform_int_distribution<u64> dist;

			for (int c = 0; c < 2; c++) {
				for (int p = 0; p < 6; p++) {
					for (int s = 0; s < 64; s++) {
						pieceVals[c][p][s] = dist(gen);
					}
				}
			}

			sideToMove[0] = dist(gen);
			sideToMove[1] = dist(gen);

			castlingRights[0] = dist(gen);
			castlingRights[1] = dist(gen);
			castlingRights[2] = dist(gen);
			castlingRights[3] = dist(gen);

			enPassantFiles[0] = dist(gen);
			enPassantFiles[1] = dist(gen);
			enPassantFiles[2] = dist(gen);
			enPassantFiles[3] = dist(gen);
			enPassantFiles[4] = dist(gen);
			enPassantFiles[5] = dist(gen);
			enPassantFiles[6] = dist(gen);
			enPassantFiles[7] = dist(gen);
		}

		u64 ZobristGenerator::generateHash(const Position position)
		{
			u64 hash = 0ULL;
			int pos;
			BitBoard b0;
			for (int c = 0; c < 2; c++) {
				for (int p = 0; p < 6; p++) {
					b0 = position.board.pieceLocations[c][p];
					while (b0) {
						pos = bitScanForward(b0);
						popBit(b0, pos);
						hash ^= pieceVals[c][p][pos];
					}
				}
			}

			hash ^= (position.status.WKcastle ? castlingRights[0] : 0ULL);
			hash ^= (position.status.WQcastle ? castlingRights[1] : 0ULL);
			hash ^= (position.status.BKcastle ? castlingRights[2] : 0ULL);
			hash ^= (position.status.BQcastle ? castlingRights[3] : 0ULL);

			hash ^= position.status.EP != no_Tile ? enPassantFiles[position.status.EP % 8] : 0ULL;

			hash ^= sideToMove[position.status.isWhite];

			return hash;
		}

	} // namsapce HASH

} // namespace KRONOS