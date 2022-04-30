#include "Chess.h"

using namespace BB;

Chess::Chess()
{
	pieceLocations[PAWN][WHITE] = 0;
	pieceLocations[KNIGHT][WHITE] = 0;
	pieceLocations[BISHOP][WHITE] = 0;
	pieceLocations[ROOK][WHITE] = 0;
	pieceLocations[QUEEN][WHITE] = 0;
	pieceLocations[KING][WHITE] = 0;

	pieceLocations[PAWN][BLACK] = 0;
	pieceLocations[KNIGHT][BLACK] = 0;
	pieceLocations[BISHOP][BLACK] = 0;
	pieceLocations[ROOK][BLACK] = 0;
	pieceLocations[QUEEN][BLACK] = 0;
	pieceLocations[KING][BLACK] = 0;

	collectiveLocations[WHITE] = 0;
	collectiveLocations[BLACK] = 0;
	collectiveLocations[BOTH] = 0;

	attackMaps[PAWN][WHITE] = 0;
	attackMaps[KNIGHT][WHITE] = 0;
	attackMaps[BISHOP][WHITE] = 0;
	attackMaps[ROOK][WHITE] = 0;
	attackMaps[QUEEN][WHITE] = 0;
	attackMaps[KING][WHITE] = 0;

	attackMaps[PAWN][BLACK] = 0;
	attackMaps[KNIGHT][BLACK] = 0;
	attackMaps[BISHOP][BLACK] = 0;
	attackMaps[ROOK][BLACK] = 0;
	attackMaps[QUEEN][BLACK] = 0;
	attackMaps[KING][BLACK] = 0;

	//setBit(pieceLocations[PAWN][WHITE].bitboard, E4);
	//setBit(pieceLocations[PAWN][WHITE].bitboard, D4);
	//
	//std::cout << pieceLocations[PAWN][WHITE].count() << std::endl;
	//
	//while (pieceLocations[PAWN][WHITE].bitboard) {
	//	uShort index = pieceLocations[PAWN][WHITE].pop();
	//	std::cout << boardTilesStrings[index] << std::endl;
	//	popBit(pieceLocations[PAWN][WHITE].bitboard, index);
	//}
	
	processFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	std::cout << "White Pawns:\n" << pieceLocations[PAWN][WHITE] << std::endl;
	std::cout << "White Knights:\n" << pieceLocations[KNIGHT][WHITE] << std::endl;
	std::cout << "White Bishops:\n" << pieceLocations[BISHOP][WHITE] << std::endl;
	std::cout << "White Rooks:\n" << pieceLocations[ROOK][WHITE] << std::endl;
	std::cout << "White Queens:\n" << pieceLocations[QUEEN][WHITE] << std::endl;
	std::cout << "White Kings:\n" << pieceLocations[KING][WHITE] << std::endl;

	std::cout << "Black Pawns:\n" << pieceLocations[PAWN][BLACK] << std::endl;
	std::cout << "Black Knights:\n" << pieceLocations[KNIGHT][BLACK] << std::endl;
	std::cout << "Black Bishops:\n" << pieceLocations[BISHOP][BLACK] << std::endl;
	std::cout << "Black Rooks:\n" << pieceLocations[ROOK][BLACK] << std::endl;
	std::cout << "Black Queens:\n" << pieceLocations[QUEEN][BLACK] << std::endl;
	std::cout << "Black Kings:\n" << pieceLocations[KING][BLACK] << std::endl;

	std::cout << "White Pieces:\n" << collectiveLocations[WHITE] << std::endl;
	std::cout << "Black Pieces:\n" << collectiveLocations[BLACK] << std::endl;
	std::cout << "All Pieces:\n" << collectiveLocations[BOTH] << std::endl;


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

	int bIndex = 0;

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
				bIndex++;
			}
		}
		else {
			switch (fenBoard[index])
			{
			default:
				break;

			case 'p':
				setBit(pieceLocations[PAWN][BLACK].bitboard, bIndex);
				break;

			case 'P':
				setBit(pieceLocations[PAWN][WHITE].bitboard, bIndex);
				break;

			case 'n':
				setBit(pieceLocations[KNIGHT][BLACK].bitboard, bIndex);
				break;

			case 'N':
				setBit(pieceLocations[KNIGHT][WHITE].bitboard, bIndex);
				break;

			case 'b':
				setBit(pieceLocations[BISHOP][BLACK].bitboard, bIndex);
				break;

			case 'B':
				setBit(pieceLocations[BISHOP][WHITE].bitboard, bIndex);
				break;

			case 'r':
				setBit(pieceLocations[ROOK][BLACK].bitboard, bIndex);
				break;

			case 'R':
				setBit(pieceLocations[ROOK][WHITE].bitboard, bIndex);
				break;

			case 'q':
				setBit(pieceLocations[QUEEN][BLACK].bitboard, bIndex);
				break;

			case 'Q':
				setBit(pieceLocations[QUEEN][WHITE].bitboard, bIndex);
				break;

			case 'k':
				setBit(pieceLocations[KING][BLACK].bitboard, bIndex);
				break;

			case 'K':
				setBit(pieceLocations[KING][WHITE].bitboard, bIndex);
				break;
			}
			bIndex++;
		}
	}
	
	collectiveLocations[WHITE] = pieceLocations[PAWN][WHITE].bitboard | pieceLocations[KNIGHT][WHITE].bitboard | pieceLocations[BISHOP][WHITE].bitboard | pieceLocations[ROOK][WHITE].bitboard | pieceLocations[QUEEN][WHITE].bitboard | pieceLocations[KING][WHITE].bitboard;
	collectiveLocations[BLACK] = pieceLocations[PAWN][BLACK].bitboard | pieceLocations[KNIGHT][BLACK].bitboard | pieceLocations[BISHOP][BLACK].bitboard | pieceLocations[ROOK][BLACK].bitboard | pieceLocations[QUEEN][BLACK].bitboard | pieceLocations[KING][BLACK].bitboard;
	collectiveLocations[BOTH] = collectiveLocations[WHITE].bitboard | collectiveLocations[BLACK].bitboard;

	if (fenTurn != "w" && fenTurn != "b") {
		std::cout << "Invalid colour\n";
		return;
	}

	turn = fenTurn == "w" ? WHITE : BLACK;

	castleRights = 0;

	if (instr(fenCastling, 'K')) {
		castleRights |= WK;
	}
	if (instr(fenCastling, 'Q')) {
		castleRights |= WQ;
	}
	if (instr(fenCastling, 'k')) {
		castleRights |= BK;
	}
	if (instr(fenCastling, 'q')) {
		castleRights |= BQ;
	}

	if (fenEP != "-") {

		if (!is_number(fenEP) && (std::stoi(fenEP) > 64 || std::stoi(fenEP) < 1)) {
			std::cout << "Invalid en passant square\n";
			return;
		}

		enPassantTile = std::stoi(fenEP);
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