#pragma once

#include "utility.h"
#include "BitBoard.h"

namespace KRONOS {

	// numerical values for tiles
	enum boardTiles : int {
		A1, B1, C1, D1, E1, F1, G1, H1,
		A2, B2, C2, D2, E2, F2, G2, H2,
		A3, B3, C3, D3, E3, F3, G3, H3,
		A4, B4, C4, D4, E4, F4, G4, H4,
		A5, B5, C5, D5, E5, F5, G5, H5,
		A6, B6, C6, D6, E6, F6, G6, H6,
		A7, B7, C7, D7, E7, F7, G7, H7,
		A8, B8, C8, D8, E8, F8, G8, H8, no_Tile
	};

	// string values for tiles
	static const char* boardTilesStrings[] = {
		"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
		"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
		"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
		"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
		"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
		"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
		"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
		"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "no_Tile"
	};

	// enum for sides
	enum sides : int {
		BLACK,
		WHITE,
		BOTH
	};

	// enum for pieces
	enum Pieces : int {
		PAWN,
		KNIGHT,
		BISHOP,
		ROOK,
		QUEEN,
		KING
	};	

	// stores the bitboard information about a position
	struct Board {

		// piece bit boards
		BitBoard pieceLocations[2][6];

		// white, black and both bit boards
		BitBoard occupied[3];

		constexpr Board() :
			pieceLocations{ {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0} },
			occupied{ 0, 0, 0 } {}

		constexpr Board(
			BitBoard wp, BitBoard wn, BitBoard wb, BitBoard wr, BitBoard wq, BitBoard wk,
			BitBoard bp, BitBoard bn, BitBoard bb, BitBoard br, BitBoard bq, BitBoard bk) :
			pieceLocations{ { wp, wn, wb, wr, wq, wk }, { bp, bn, bb, br, bq, bk } },
			occupied{}
		{
			occupied[WHITE] = wp | wn | wb | wr | wq | wk;
			occupied[BLACK] = bp | bn | bb | br | bq | bk;
			occupied[BOTH] = occupied[WHITE] | occupied[BLACK];
		}

		constexpr Board(const Board& other) :
			pieceLocations{ { other.pieceLocations[WHITE][PAWN], other.pieceLocations[WHITE][KNIGHT], other.pieceLocations[WHITE][BISHOP], 
							  other.pieceLocations[WHITE][ROOK], other.pieceLocations[WHITE][QUEEN],  other.pieceLocations[WHITE][KING] }, 
							{ other.pieceLocations[BLACK][PAWN], other.pieceLocations[BLACK][KNIGHT], other.pieceLocations[BLACK][BISHOP], 
			                  other.pieceLocations[BLACK][ROOK], other.pieceLocations[BLACK][QUEEN],  other.pieceLocations[BLACK][KING] } },
			occupied{ other.occupied[WHITE], other.occupied[BLACK], other.occupied[BOTH] } {}

		template<bool isWhite>
		constexpr void mergeColour() {
			occupied[isWhite] = pieceLocations[isWhite][PAWN] | pieceLocations[isWhite][KNIGHT] | pieceLocations[isWhite][BISHOP] |
								pieceLocations[isWhite][ROOK] | pieceLocations[isWhite][QUEEN]  | pieceLocations[isWhite][KING];
		}

		// merges bitboards together
		constexpr void mergeBoth() {
			mergeColour<WHITE>(); mergeColour<BLACK>();
			occupied[BOTH] = occupied[WHITE] | occupied[BLACK];
		}

	};

}