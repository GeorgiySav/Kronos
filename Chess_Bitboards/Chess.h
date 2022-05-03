#pragma once
#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include <map>
#include <sstream>

#include "utility.h"
#include "BitBoard.h"
#include "Magic.h"

namespace CHENG
{

	enum boardTiles : uShort {
		A1, B1, C1, D1, E1, F1, G1, H1,
		A2, B2, C2, D2, E2, F2, G2, H2,
		A3, B3, C3, D3, E3, F3, G3, H3,
		A4, B4, C4, D4, E4, F4, G4, H4,
		A5, B5, C5, D5, E5, F5, G5, H5,
		A6, B6, C6, D6, E6, F6, G6, H6,
		A7, B7, C7, D7, E7, F7, G7, H7,
		A8, B8, C8, D8, E8, F8, G8, H8, no_Tile
	};

	static const char* boardTilesStrings[] = {
		"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
		"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
		"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
		"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
		"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
		"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
		"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
		"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8", "no_Tile"
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

	struct Board {

		// piece bit boards
		BitBoard pieceLocations[2][6];

		// white, black and both bit boards
		BitBoard occupied[3];

		constexpr Board() :
			pieceLocations{ {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0} },
			occupied{0, 0, 0}
		{
			
		}

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

		void mergeWhite() {
			occupied[WHITE] = pieceLocations[WHITE][PAWN] | pieceLocations[WHITE][KNIGHT] | pieceLocations[WHITE][BISHOP] | pieceLocations[WHITE][ROOK] | pieceLocations[WHITE][QUEEN] | pieceLocations[WHITE][KING];
		}

		void mergeBlack() {
			occupied[BLACK] = pieceLocations[BLACK][PAWN] | pieceLocations[BLACK][KNIGHT] | pieceLocations[BLACK][BISHOP] | pieceLocations[BLACK][ROOK] | pieceLocations[BLACK][QUEEN] | pieceLocations[BLACK][KING];
		}

		void mergeBoth() {
			mergeBlack(); mergeWhite();
			occupied[BOTH] = occupied[WHITE] | occupied[BLACK];
		}

	};

	class Chess
	{

		Board board;

		uShort turn;

		uShort enPassantTile = no_Tile;

		/* castling rights	
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

			//os << "White Pawns:\n" << bb.pieceLocations[WHITE][PAWN] << std::endl;
			//os << "White Knights:\n" << bb.pieceLocations[WHITE][KNIGHT] << std::endl;
			//os << "White Bishops:\n" << bb.pieceLocations[WHITE][BISHOP] << std::endl;
			//os << "White Rooks:\n" << bb.pieceLocations[WHITE][ROOK] << std::endl;
			//os << "White Queen:\n" << bb.pieceLocations[WHITE][QUEEN] << std::endl;
			//os << "White King:\n" << bb.pieceLocations[WHITE][KING] << std::endl;
			//
			//os << "Black Pawns:\n" << bb.pieceLocations[BLACK][PAWN] << std::endl;
			//os << "Black Knights:\n" << bb.pieceLocations[BLACK][KNIGHT] << std::endl;
			//os << "Black Bishops:\n" << bb.pieceLocations[BLACK][BISHOP] << std::endl;
			//os << "Black Rooks:\n" << bb.pieceLocations[BLACK][ROOK] << std::endl;
			//os << "Black Queen:\n" << bb.pieceLocations[BLACK][QUEEN] << std::endl;
			//os << "Black King:\n" << bb.pieceLocations[BLACK][KING] << std::endl;
			//
			//os << "White Pieces:\n" << bb.collectiveLocations[WHITE] << std::endl;
			//os << "Black Pieces:\n" << bb.collectiveLocations[BLACK] << std::endl;
			//os << "Both Pieces:\n" << bb.collectiveLocations[BOTH] << std::endl;

			return os;
		}

	};

}