#pragma once
#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include <map>
#include <sstream>
#include <array>
#include <chrono>
#include <memory>

#include "utility.h"
#include "BitBoard.h"
#include "Magic.h"
#include "Rays.h"
#include "Board.h"
#include "Move_Generation.h"
#include "Zobrist_Hashing.h"
#include "Search.h"

#define MAX_POSITIONS 100

namespace KRONOS
{
	
	struct perftResults {
		int nodes;
		int captures;
		int eps;
		int castles;
		int promotions;
		
		perftResults() : nodes(0), captures(0), eps(0), castles(0), promotions(0) {}

		perftResults operator + (const perftResults& other) const {
			perftResults result;
			result.nodes = this->nodes + other.nodes;
			result.captures = this->captures + other.captures;
			result.eps = this->eps + other.eps;
			result.castles = this->castles + other.castles;
			result.promotions = this->promotions + other.promotions;
			return result;
		}			
		
		friend std::ostream& operator << (std::ostream& os, const perftResults& results) {
			os << "Nodes: " << results.nodes << std::endl;
			os << "Captures: " << results.captures << std::endl;
			os << "Eps: " << results.eps << std::endl;
			os << "Castles: " << results.castles << std::endl;
			os << "Promotions: " << results.promotions << std::endl;
			return os;
		}

	};

	class KronosEngine
	{

		int ply = 0;
		std::array<Position, MAX_POSITIONS> positions;
		std::array<Move, MAX_POSITIONS> moveHistory;

		std::vector<Move> moves;

		SEARCH::SearchTree search;

	public:

		KronosEngine();
		~KronosEngine();

		int BoardCoordsToIndex(std::string coord) {
			return ((coord[1] - '1') * 8) + (coord[0] - 'a');
		}

		std::string BoardIndexToCoords(int index) {
			return std::string(1, 'a' + (index % 8)) + std::string(1, '1' + (index / 8));
		}

		void processFEN(std::string FEN);
		
		inline void generateMoves() {
			moves.clear();
			KRONOS::generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, &moves);
		}

		inline void makeMove(Move move) {
			ply++;
			positions[ply] = positions[(ply - 1)];
			//moveHistory[ply] = move;
			
			updatePosition(positions[ply], move);
		}			
		
		constexpr void unmakeMove() {
			ply = ply - 1;
		}



		int perft(int depth) {
			if (depth == 0) {
				//perftResults result = perftResults();
				//if (moveHistory[ply].flag == ENPASSANT) {
				//	result.eps++;
				//}
				//else if (moveHistory[ply].flag & PROMOTION) {
				//	result.promotions++;
				//}
				//else if (moveHistory[ply].flag == KING_CASTLE || moveHistory[ply].flag == QUEEN_CASTLE) {
				//	result.castles++;
				//}
				//
				//if (moveHistory[ply].flag & CAPTURE) {
				//	result.captures++;
				//}
				//
				//result.nodes++;
				//return result;
				return 1;
			}

			int count = 0;

			generateMoves();
			std::vector<Move> moves = this->moves;

			for (Move move : moves) {
				makeMove(move);
				count += perft(depth - 1);
				unmakeMove();
			}

			return count;

		}

		perftResults perftDiv(int depth) {
			
			perftResults count = perftResults();

			generateMoves();
			std::vector<Move> moves = this->moves;

			for (Move move : moves) {
				makeMove(move);
				std::cout << BoardIndexToCoords(move.from) << BoardIndexToCoords(move.to) << ":\n" << perft(depth - 1) << '\n';
				unmakeMove();
			}

			return count;

		}

		Move getBestMove() {
			return search.search(positions[ply], 5, 4000);
		}

		void printBoard() {
			std::cout << "White Pawns: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][PAWN]) << std::endl;
			std::cout << "White Knights: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][KNIGHT]) << std::endl;
			std::cout << "White Bishops: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][BISHOP]) << std::endl;
			std::cout << "White Rooks: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][ROOK]) << std::endl;
			std::cout << "White Queens: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][QUEEN]) << std::endl;
			std::cout << "White King: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][KING]) << std::endl;

			std::cout << "Black Pawns: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][PAWN]) << std::endl;
			std::cout << "Black Knights: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][KNIGHT]) << std::endl;
			std::cout << "Black Bishops: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][BISHOP]) << std::endl;
			std::cout << "Black Rooks: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][ROOK]) << std::endl;
			std::cout << "Black Queens: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][QUEEN]) << std::endl;
			std::cout << "Black King: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][KING]) << std::endl;
		}

		Board* getBitBoardsPointer() {
			return &positions[ply].board;
		}

		BoardStatus* getStatusPointer() {
			return &positions[ply].status;
		}
		
		std::vector<Move>* getMovesPointer() {
			return &moves;
		}

		int getPly() {
			return ply;
		}
		
		friend std::ostream& operator<<(std::ostream& os, const KronosEngine& bb) {

			//os << "White Pawns: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][PAWN]) << std::endl;
			//os << "White Knights: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][KNIGHT]) << std::endl;
			//os << "White Bishops: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][BISHOP]) << std::endl;
			//os << "White Rooks: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][ROOK]) << std::endl;
			//os << "White Queens: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][QUEEN]) << std::endl;
			//os << "White King: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][KING]) << std::endl;
			//
			//os << "Black Pawns: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][PAWN]) << std::endl;
			//os << "Black Knights: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][KNIGHT]) << std::endl;
			//os << "Black Bishops: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][BISHOP]) << std::endl;
			//os << "Black Rooks: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][ROOK]) << std::endl;
			//os << "Black Queens: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][QUEEN]) << std::endl;
			//os << "Black King: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][KING]) << std::endl;;

			return os;
		}

	};

}