#pragma once
#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include <map>
#include <sstream>

#include "utility.h"

#define getBit(bb, i) (bb & (1ULL << i)) 
#define setBit(bb, i) (bb |= (1ULL << i))
#define popBit(bb, i) (bb &= ~(1ULL << i))

namespace BB
{
		
	const int BitTable[64] = {
		0, 47,  1, 56, 48, 27,  2, 60,
	   57, 49, 41, 37, 28, 16,  3, 61,
	   54, 58, 35, 52, 50, 42, 21, 44,
	   38, 32, 29, 23, 17, 11,  4, 62,
	   46, 55, 26, 59, 40, 36, 15, 53,
	   34, 51, 20, 43, 31, 22, 10, 45,
	   25, 39, 14, 33, 19, 30,  9, 24,
	   13, 18,  8, 12,  7,  6,  5, 63
	};

	struct BitBoard {
		u64 bitboard;

		BitBoard() : bitboard(0) {}


		BitBoard(u64 value) : bitboard(value) {}

		BitBoard(const BitBoard& other) : bitboard(other.bitboard) {}

		uShort pop() {
			const u64 debruijn64 = u64(0x03f79d71b4cb0a89);
			assert(bitboard != 0);
			return BitTable[((bitboard ^ (bitboard - 1)) * debruijn64) >> 58];
		}

		uShort count() {
			u64 x = bitboard;
			uShort count;
			for (count = 0; x; count++)
				x &= x - 1;
			return count;	
		}

		BitBoard& operator=(const BitBoard& other) {
			bitboard = other.bitboard;
			return *this;
		}

		friend std::ostream& operator << (std::ostream& os, const BitBoard& bitboard) {

			for (uShort y = 0; y < 8; y++) {
				
				os << char('8' - y) << " ";

				for (uShort x = 0; x < 8; x++) {
					
					uShort tile = y * 8 + x;

					os << (getBit(bitboard.bitboard, tile) ? "1" : "0") << " ";

				}

				os << std::endl;

			}

			os << "  A B C D E F G H" << std::endl;

			//os << printf("  Bitboard: %llu\n", bitboard);

			return os;

		}

	};


	enum boardTiles {
		A8, B8, C8, D8, E8, F8, G8, H8,
		A7, B7, C7, D7, E7, F7, G7, H7,
		A6, B6, C6, D6, E6, F6, G6, H6,
		A5, B5, C5, D5, E5, F5, G5, H5,
		A4, B4, C4, D4, E4, F4, G4, H4,
		A3, B3, C3, D3, E3, F3, G3, H3,
		A2, B2, C2, D2, E2, F2, G2, H2,
		A1, B1, C1, D1, E1, F1, G1, H1, no_Tile
	};

	static const char* boardTilesStrings[] = {
		"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
		"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
		"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
		"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
		"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
		"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
		"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
		"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1"
	};

	enum sides {
		WHITE,
		BLACK,
		BOTH
	};

	enum pieces {
		PAWN,
		KNIGHT,
		BISHOP,
		ROOK,
		QUEEN,
		KING
	};

	enum castling {
		WK = 1,
		WQ = 2,
		BK = 4,
		BQ = 8
	};

	class Chess
	{

		// piece bit boards
		BitBoard pieceLocations[6][2];

		// white, black and both bit boards
		BitBoard collectiveLocations[3];

		// attack maps for every piece
		BitBoard attackMaps[6][2];

		uShort turn;

		uShort enPassantTile = no_Tile;

		/* castiling rights
		
			000x white king castle
			00x0 white queen castle
			0x00 black king castle
			x000 black queen castle

		*/

		uShort castleRights;
		
		uShort halfMoves;
		uShort fullMoves;

	public:

		Chess();
		~Chess();

		void processFEN(std::string FEN);
		

		friend std::ostream& operator<<(std::ostream& os, const Chess& bb) {

			os << "White Pawns:\n" << bb.pieceLocations[PAWN][WHITE] << std::endl;
			os << "White Knights:\n" << bb.pieceLocations[KNIGHT][WHITE] << std::endl;
			os << "White Bishops:\n" << bb.pieceLocations[BISHOP][WHITE] << std::endl;
			os << "White Rooks:\n" << bb.pieceLocations[ROOK][WHITE] << std::endl;
			os << "White Queens:\n" << bb.pieceLocations[QUEEN][WHITE] << std::endl;
			os << "White Kings:\n" << bb.pieceLocations[KING][WHITE] << std::endl;
			
			os << "Black Pawns:\n" << bb.pieceLocations[PAWN][BLACK] << std::endl;
			os << "Black Knights:\n" << bb.pieceLocations[KNIGHT][BLACK] << std::endl;
			os << "Black Bishops:\n" << bb.pieceLocations[BISHOP][BLACK] << std::endl;
			os << "Black Rooks:\n" << bb.pieceLocations[ROOK][BLACK] << std::endl;
			os << "Black Queens:\n" << bb.pieceLocations[QUEEN][BLACK] << std::endl;
			os << "Black Kings:\n" << bb.pieceLocations[KING][BLACK] << std::endl;
			
			os << "White Pieces:\n" << bb.collectiveLocations[WHITE] << std::endl;
			os << "Black Pieces:\n" << bb.collectiveLocations[BLACK] << std::endl;
			os << "All Pieces:\n" << bb.collectiveLocations[BOTH] << std::endl;

			return os;
		}

	};

}