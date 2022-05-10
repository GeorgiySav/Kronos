#include "Chess.h"

using namespace CHENG;

Chess::Chess() : board()
{

	initRays();
	initMagics();
	
	processFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	//std::cout << "White Pawns:\n" << _BitBoard(board.pieceLocations[WHITE][PAWN]) << std::endl;
	//std::cout << "White Knights:\n" << _BitBoard(board.pieceLocations[WHITE][KNIGHT]) << std::endl;
	//std::cout << "White Bishops:\n" << _BitBoard(board.pieceLocations[WHITE][BISHOP]) << std::endl;
	//std::cout << "White Rooks:\n" << _BitBoard(board.pieceLocations[WHITE][ROOK]) << std::endl;
	//std::cout << "White Queens:\n" << _BitBoard(board.pieceLocations[WHITE][QUEEN]) << std::endl;
	//std::cout << "White Kings:\n" << _BitBoard(board.pieceLocations[WHITE][KING]) << std::endl;
	//
	//std::cout << "Black Pawns:\n" << _BitBoard(board.pieceLocations[BLACK][PAWN]) << std::endl;
	//std::cout << "Black Knights:\n" << _BitBoard(board.pieceLocations[BLACK][KNIGHT]) << std::endl;
	//std::cout << "Black Bishops:\n" << _BitBoard(board.pieceLocations[BLACK][BISHOP]) << std::endl;
	//std::cout << "Black Rooks:\n" << _BitBoard(board.pieceLocations[BLACK][ROOK]) << std::endl;
	//std::cout << "Black Queens:\n" << _BitBoard(board.pieceLocations[BLACK][QUEEN]) << std::endl;
	//std::cout << "Black Kings:\n" << _BitBoard(board.pieceLocations[BLACK][KING]) << std::endl;
	//
	//std::cout << "\n\n";


	board.mergeBoth();

	

	isWhite = true;

	generateMoves();

	for (auto move : moves)
	{
		std::cout << _BitBoard((1ULL << move.to) | (1ULL << move.from)) << std::endl;
	}

}

Chess::~Chess()
{
	
}

void Chess::processFEN(std::string FEN)
{

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
				setBit(board.pieceLocations[BLACK][PAWN], bIndex);
				break;

			case 'P':
				setBit(board.pieceLocations[WHITE][PAWN], bIndex);
				break;

			case 'n':
				setBit(board.pieceLocations[BLACK][KNIGHT], bIndex);
				break;

			case 'N':
				setBit(board.pieceLocations[WHITE][KNIGHT], bIndex);
				break;

			case 'b':
				setBit(board.pieceLocations[BLACK][BISHOP], bIndex);
				break;

			case 'B':
				setBit(board.pieceLocations[WHITE][BISHOP], bIndex);
				break;

			case 'r':
				setBit(board.pieceLocations[BLACK][ROOK], bIndex);
				break;

			case 'R':
				setBit(board.pieceLocations[WHITE][ROOK], bIndex);
				break;

			case 'q':
				setBit(board.pieceLocations[BLACK][QUEEN], bIndex);
				break;

			case 'Q':
				setBit(board.pieceLocations[WHITE][QUEEN], bIndex);
				break;

			case 'k':
				setBit(board.pieceLocations[BLACK][KING], bIndex);
				break;

			case 'K':
				setBit(board.pieceLocations[WHITE][KING], bIndex);
				break;
			}
			bIndex = !((bIndex + 1) % 8) ? bIndex - 15 : bIndex + 1;
		}
	}
	
	board.mergeBoth();

	if (fenTurn != "w" && fenTurn != "b") {
		std::cout << "Invalid colour\n";
		return;
	}

	isWhite = fenTurn == "w" ? true : false;

	status.BKcastle = false;
	status.BQcastle = false;
	status.WKcastle = false;
	status.WQcastle = false;

	if (instr(fenCastling, 'K')) {
		status.WKcastle = true;
	}
	if (instr(fenCastling, 'Q')) {
		status.WQcastle = true;
	}
	if (instr(fenCastling, 'k')) {
		status.BKcastle = true;
	}
	if (instr(fenCastling, 'q')) {
		status.BQcastle = true;
	}

	if (fenEP != "-") {

		if (!is_number(fenEP) && (std::stoi(fenEP) > 64 || std::stoi(fenEP) < 1)) {
			std::cout << "Invalid en passant square\n";
			return;
		}

		status.EP = std::stoi(fenEP);
	}

	if (!is_number(fenHalfmove)) {
		halfMoves = 0;
	}

	halfMoves = atoi(fenHalfmove.c_str());

	if (!is_number(fenFullmove)) {
		fullMoves = 0;
	}

	fullMoves = atoi(fenFullmove.c_str());
}


