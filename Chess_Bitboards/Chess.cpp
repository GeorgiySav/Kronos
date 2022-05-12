#include "Chess.h"

using namespace CHENG;

Chess::Chess()
{
	initRays();
	initMagics();

	processFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - - -");

	//printBoard();

	std::cout << _BitBoard(positions[ply].board.occupied[BOTH]) << std::endl;

	std::cout << "Ply 1: " << perft(1) << std::endl;
	std::cout << "Ply 2: " << perft(2) << std::endl;
	std::cout << "Ply 3: " << perft(3) << std::endl;
	std::cout << "Ply 4: " << perft(4) << std::endl;
	std::cout << "Ply 5: " << perft(5) << std::endl;
	std::cout << "Ply 6: " << perft(6) << std::endl;
}

Chess::~Chess()
{
}

void Chess::processFEN(std::string FEN)
{
	moves.clear();
	ply = 0;

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
		return;
	}

	if ((count(fenBoard, '/') != 7) || (count(fenBoard, 'k') != 1) || (count(fenBoard, 'K') != 1)) {
		std::cout << "Invalid number of kings or ranks\n";
		return;
	}

	int bIndex = 56;

	for (int index = 0; index < fenBoard.length(); index++) {
		if (fenBoard[index] == '/') {
			index++;
		}
		if (std::isdigit(fenBoard[index])) {
			if (bIndex % 8 + fenBoard[index] - '0' > 8) {
				std::cout << "Too many pieces or empty spaces in file\n";
				return;
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
				setBit(positions[ply].board.pieceLocations[BLACK][PAWN], bIndex);
				break;

			case 'P':
				setBit(positions[ply].board.pieceLocations[WHITE][PAWN], bIndex);
				break;

			case 'n':
				setBit(positions[ply].board.pieceLocations[BLACK][KNIGHT], bIndex);
				break;

			case 'N':
				setBit(positions[ply].board.pieceLocations[WHITE][KNIGHT], bIndex);
				break;

			case 'b':
				setBit(positions[ply].board.pieceLocations[BLACK][BISHOP], bIndex);
				break;

			case 'B':
				setBit(positions[ply].board.pieceLocations[WHITE][BISHOP], bIndex);
				break;

			case 'r':
				setBit(positions[ply].board.pieceLocations[BLACK][ROOK], bIndex);
				break;

			case 'R':
				setBit(positions[ply].board.pieceLocations[WHITE][ROOK], bIndex);
				break;

			case 'q':
				setBit(positions[ply].board.pieceLocations[BLACK][QUEEN], bIndex);
				break;

			case 'Q':
				setBit(positions[ply].board.pieceLocations[WHITE][QUEEN], bIndex);
				break;

			case 'k':
				setBit(positions[ply].board.pieceLocations[BLACK][KING], bIndex);
				break;

			case 'K':
				setBit(positions[ply].board.pieceLocations[WHITE][KING], bIndex);
				break;
			}
			bIndex = !((bIndex + 1) % 8) ? bIndex - 15 : bIndex + 1;
		}
	}

	positions[ply].board.mergeBoth();

	if (fenTurn != "w" && fenTurn != "b") {
		std::cout << "Invalid colour\n";
		return;
	}

	positions[ply].status.isWhite = fenTurn == "w" ? true : false;

	positions[ply].status.BKcastle = false;
	positions[ply].status.BQcastle = false;
	positions[ply].status.WKcastle = false;
	positions[ply].status.WQcastle = false;

	if (instr(fenCastling, 'K')) {
		positions[ply].status.WKcastle = true;
	}
	if (instr(fenCastling, 'Q')) {
		positions[ply].status.WQcastle = true;
	}
	if (instr(fenCastling, 'k')) {
		positions[ply].status.BKcastle = true;
	}
	if (instr(fenCastling, 'q')) {
		positions[ply].status.BQcastle = true;
	}

	if (fenEP != "-") {
		if (!is_number(fenEP) && (std::stoi(fenEP) > 64 || std::stoi(fenEP) < 1)) {
			std::cout << "Invalid en passant square\n";
			return;
		}

		positions[ply].status.EP = std::stoi(fenEP);
	}

	if (!is_number(fenHalfmove)) {
		positions[ply].halfMoves = 0;
	}

	positions[ply].halfMoves = atoi(fenHalfmove.c_str());

	if (!is_number(fenFullmove)) {
		positions[ply].fullMoves = 0;
	}

	positions[ply].fullMoves = atoi(fenFullmove.c_str());
}