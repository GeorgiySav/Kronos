#include "Zobrist_Hashing.h"

namespace KRONOS {
	
	namespace HASH {

		// returns a random unsigned 64 bit integer
		uint64_t rand64() {
			static uint64_t s = 0xDEADBEEFDEADBEEF;
			s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
			return s * 2685821657736338717LL;
		}
		
		ZobristGenerator::ZobristGenerator()
		{
			// initialises all variables
			for (int c = 0; c < 2; c++) {
				for (int p = 0; p < 6; p++) {
					for (int s = 0; s < 64; s++) {
						pieceVals[c][p][s] = rand64();
					}
				}
			}

			sideToMove[0] = rand64();
			sideToMove[1] = rand64();

			castlingRights[0][0] = rand64();
			castlingRights[0][1] = rand64();
			castlingRights[1][0] = rand64();
			castlingRights[1][1] = rand64();
			castlingRights[2][0] = rand64();
			castlingRights[2][1] = rand64();
			castlingRights[3][0] = rand64();
			castlingRights[3][1] = rand64();
								
			enPassantFiles[0] = rand64();
			enPassantFiles[1] = rand64();
			enPassantFiles[2] = rand64();
			enPassantFiles[3] = rand64();
			enPassantFiles[4] = rand64();
			enPassantFiles[5] = rand64();
			enPassantFiles[6] = rand64();
			enPassantFiles[7] = rand64();
		}

		u64 ZobristGenerator::generateHash(const Position& position)
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

			hash ^= (position.status.WKcastle ? castlingRights[0][0] : castlingRights[0][1]);
			hash ^= (position.status.WQcastle ? castlingRights[1][0] : castlingRights[1][1]);
			hash ^= (position.status.BKcastle ? castlingRights[2][0] : castlingRights[2][1]);
			hash ^= (position.status.BQcastle ? castlingRights[3][0] : castlingRights[3][1]);

			hash ^= position.status.EP != no_Tile ? enPassantFiles[position.status.EP % 8] : 0ULL;

			hash ^= sideToMove[position.status.isWhite];

			return hash;
		}

		void ZobristGenerator::updateHash(const Position& prevPos, Position& position, Move& move)
		{
			u64 prevHash = prevPos.hash;
			
			int from = move.from;
			int to = move.to;
			u8 piece = move.moved_Piece;
			u8 flag = move.flag;
		
			// move the piece in the hash
			prevHash ^= pieceVals[prevPos.status.isWhite][piece][from];
			prevHash ^= pieceVals[prevPos.status.isWhite][piece][to];

			// remove or add en passant information
			prevHash ^= (prevPos.status.EP != no_Tile ? enPassantFiles[prevPos.status.EP % 8] : 0ULL);
			prevHash ^= (position.status.EP != no_Tile ? enPassantFiles[position.status.EP % 8] : 0ULL);
		
			// remove piece that was attacked by en passant
			if (flag == ENPASSANT)
				prevHash ^= pieceVals[!prevPos.status.isWhite][PAWN][((prevPos.status.isWhite) ? (move.to - 8) : (move.to + 8))];
			else if (flag & CAPTURE)
				prevHash ^= pieceVals[!prevPos.status.isWhite][prevPos.getPieceType(to)][to];
			else if (flag == KING_CASTLE || flag == QUEEN_CASTLE) { // move rooks in castling
				if (flag == KING_CASTLE) {
					prevHash ^= pieceVals[prevPos.status.isWhite][ROOK][(prevPos.status.isWhite) ? H1 : H8];
					prevHash ^= pieceVals[prevPos.status.isWhite][ROOK][(prevPos.status.isWhite) ? F1 : F8];
				}
				else {
					prevHash ^= pieceVals[prevPos.status.isWhite][ROOK][(prevPos.status.isWhite) ? A1 : A8];
					prevHash ^= pieceVals[prevPos.status.isWhite][ROOK][(prevPos.status.isWhite) ? D1 : D8];
				}
			}

			// check for promotions
			if (flag & PROMOTION) {
				prevHash ^= pieceVals[prevPos.status.isWhite][PAWN][to];
				if (flag & CAPTURE) 
					flag ^= CAPTURE;

				if (flag == KNIGHT_PROMOTION)
					prevHash ^= pieceVals[prevPos.status.isWhite][KNIGHT][to];
				else if (flag == BISHOP_PROMOTION)
					prevHash ^= pieceVals[prevPos.status.isWhite][BISHOP][to];
				else if (flag == ROOK_PROMOTION) 
					prevHash ^= pieceVals[prevPos.status.isWhite][ROOK][to];
				else if (flag == QUEEN_PROMOTION)
					prevHash ^= pieceVals[prevPos.status.isWhite][QUEEN][to];
			}
			
			// update castling rights
			if (prevPos.status.WKcastle != position.status.WKcastle) {
				prevHash ^= castlingRights[0][0];
				prevHash ^= castlingRights[0][1];
			}
			if (prevPos.status.WQcastle != position.status.WQcastle) {
				prevHash ^= castlingRights[1][0];
				prevHash ^= castlingRights[1][1];
			}
			if (prevPos.status.BKcastle != position.status.BKcastle) {
				prevHash ^= castlingRights[2][0];
				prevHash ^= castlingRights[2][1];
			}
			if (prevPos.status.BQcastle != position.status.BQcastle) {
				prevHash ^= castlingRights[3][0];
				prevHash ^= castlingRights[3][1];
			}

			prevHash ^= sideToMove[prevPos.status.isWhite];
			prevHash ^= sideToMove[position.status.isWhite];

			position.hash = prevHash;
		}
	} // namsapce HASH
} // namespace KRONOS