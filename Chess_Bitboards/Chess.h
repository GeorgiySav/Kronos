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
#include "Rays.h"
#include "Board.h"
#include "Move_Generation.h"

#define MAX_POSITIONS 1024

namespace CHENG
{

	struct Position {
		Board board;
		BoardStatus status;
		uShort halfMoves;
		uShort fullMoves;
	};

	class Chess
	{

		int ply = 0;
		Position positions[MAX_POSITIONS];

		std::vector<Move> moves;

	public:

		Chess();
		~Chess();

		void processFEN(std::string FEN);
		
		void generateMoves() {
			CHENG::generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, &moves);
		}

		void makeMove(Move move) {
			ply++;
			positions[ply] = positions[ply - 1];

			positions[ply].status.isWhite = !positions[ply].status.isWhite;
			positions[ply].halfMoves++;
			if (positions[ply].status.isWhite)
				positions[ply].fullMoves++;

			if (move.flag == QUIET) {
				
			}

		}			
		
		void unmakeMove() {
			
		}

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