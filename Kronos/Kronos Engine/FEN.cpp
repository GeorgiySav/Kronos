#include "FEN.h"

#include <iostream>
#include <string>
#include <sstream>

namespace KRONOS
{
	Position FENtoBoard(std::string FEN)
	{
		Position position;
		
		std::stringstream fenStream;
		fenStream.str(FEN);
		std::string fenBoard, fenTurn, fenCastling, fenEP, fenHalfmove, fenFullmove;

		std::getline(fenStream, fenBoard, ' ');
		std::getline(fenStream, fenTurn, ' ');
		std::getline(fenStream, fenCastling, ' ');
		std::getline(fenStream, fenEP, ' ');
		std::getline(fenStream, fenHalfmove, ' ');
		std::getline(fenStream, fenFullmove, ' ');

		if (fenStream.fail()) {
			std::cout << "Invalid FEN syntax\n";
			return position;
		}

		if ((count(fenBoard, '/') != 7) || (count(fenBoard, 'k') != 1) || (count(fenBoard, 'K') != 1)) {
			std::cout << "Invalid number of kings or ranks\n";
			return position;
		}

		int bIndex = 56;

		for (int index = 0; index < fenBoard.length(); index++) {
			if (fenBoard[index] == '/') {
				index++;
			}
			if (std::isdigit(fenBoard[index])) {
				if (bIndex % 8 + fenBoard[index] - '0' > 8) {
					std::cout << "Too many pieces or empty spaces in file\n";
					return Position();
				}
				for (int i = 0; i < fenBoard[index] - '0'; i++) {
					bIndex = !((bIndex + 1) % 8) ? bIndex - 15 : bIndex + 1;
				}
			}
			else {
				switch (fenBoard[index])
				{
				default:
					break;

				case 'p':
					setBit(position.board.pieceLocations[BLACK][PAWN], bIndex);
					break;

				case 'P':
					setBit(position.board.pieceLocations[WHITE][PAWN], bIndex);
					break;

				case 'n':
					setBit(position.board.pieceLocations[BLACK][KNIGHT], bIndex);
					break;

				case 'N':
					setBit(position.board.pieceLocations[WHITE][KNIGHT], bIndex);
					break;

				case 'b':
					setBit(position.board.pieceLocations[BLACK][BISHOP], bIndex);
					break;

				case 'B':
					setBit(position.board.pieceLocations[WHITE][BISHOP], bIndex);
					break;

				case 'r':
					setBit(position.board.pieceLocations[BLACK][ROOK], bIndex);
					break;

				case 'R':
					setBit(position.board.pieceLocations[WHITE][ROOK], bIndex);
					break;

				case 'q':
					setBit(position.board.pieceLocations[BLACK][QUEEN], bIndex);
					break;

				case 'Q':
					setBit(position.board.pieceLocations[WHITE][QUEEN], bIndex);
					break;

				case 'k':
					setBit(position.board.pieceLocations[BLACK][KING], bIndex);
					break;

				case 'K':
					setBit(position.board.pieceLocations[WHITE][KING], bIndex);
					break;
				}
				bIndex = !((bIndex + 1) % 8) ? bIndex - 15 : bIndex + 1;
			}
		}

		position.board.mergeBoth();

		if (fenTurn != "w" && fenTurn != "b") {
			std::cout << "Invalid colour\n";
			return Position();
		}

		position.status.isWhite = fenTurn == "w" ? true : false;

		position.status.BKcastle = false;
		position.status.BQcastle = false;
		position.status.WKcastle = false;
		position.status.WQcastle = false;

		if (instr(fenCastling, 'K')) {
			position.status.WKcastle = true;
		}
		if (instr(fenCastling, 'Q')) {
			position.status.WQcastle = true;
		}
		if (instr(fenCastling, 'k')) {
			position.status.BKcastle = true;
		}
		if (instr(fenCastling, 'q')) {
			position.status.BQcastle = true;
		}

		if (fenEP != "-") {
			position.status.EP = ((fenEP[1] - '1') * 8) + (fenEP[0] - 'a');
		}
		else
			position.status.EP = no_Tile;

		if (!is_number(fenHalfmove)) {
			position.halfMoves = 0;
		}

		position.halfMoves = atoi(fenHalfmove.c_str());

		if (!is_number(fenFullmove)) {
			position.fullMoves = 0;
		}

		position.fullMoves = atoi(fenFullmove.c_str());

		return position;

	}

	std::string BoardToFEN(const Position* position)
	{
		std::string FEN = "";
		int emptyCount = 0;

		for (int tile = 0; tile < 64; tile++)
		{
			if (tile % 8 == 0)
			{
				if (emptyCount)
				{
					FEN += std::to_string(emptyCount);
					emptyCount = 0;
				}
				FEN += "/";
			}

			if (position->board.pieceLocations[WHITE][PAWN] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "P";
			}
			else if (position->board.pieceLocations[BLACK][PAWN] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "p";
			}
			else if (position->board.pieceLocations[WHITE][KNIGHT] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "N";
			}
			else if (position->board.pieceLocations[BLACK][KNIGHT] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "n";
			}
			else if (position->board.pieceLocations[WHITE][BISHOP] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "B";
			}
			else if (position->board.pieceLocations[BLACK][BISHOP] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "b";
			}
			else if (position->board.pieceLocations[WHITE][ROOK] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "R";
			}
			else if (position->board.pieceLocations[BLACK][ROOK] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "r";
			}
			else if (position->board.pieceLocations[WHITE][QUEEN] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "Q";
			}
			else if (position->board.pieceLocations[BLACK][QUEEN] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "q";
			}
			else if (position->board.pieceLocations[WHITE][KING] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "K";
			}
			else if (position->board.pieceLocations[BLACK][KING] & (1ULL << tile)) {
				if (emptyCount)
					FEN += std::to_string(emptyCount);
				FEN += "k";
			}
			else {
				emptyCount++;
			}
		}

		if (emptyCount)
			FEN += std::to_string(emptyCount);

		FEN += " ";

		if (position->status.isWhite)
			FEN += "w ";
		else
			FEN += "b ";

		if (position->status.WKcastle)
			FEN += "K";
		if (position->status.WQcastle)
			FEN += "Q";
		if (position->status.BKcastle)
			FEN += "k";
		if (position->status.BQcastle)
			FEN += "q";

		FEN += " ";

		if (position->status.EP == no_Tile)
			FEN += "-";
		else
			FEN += (std::string(1, 'a' + (position->status.EP % 8)) + std::string(1, '1' + (position->status.EP / 8)));

		FEN += " ";
		FEN += std::to_string(position->halfMoves);
		FEN += " ";
		FEN += std::to_string(position->fullMoves);

		return FEN;
	}

}