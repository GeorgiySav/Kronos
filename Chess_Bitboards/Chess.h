#pragma once
#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include <map>
#include <sstream>
#include <array>

#include "utility.h"
#include "BitBoard.h"
#include "Magic.h"
#include "Rays.h"
#include "Board.h"
#include "Move_Generation.h"

#define MAX_POSITIONS 1024

namespace CHENG
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

	struct Position {
		Board board;
		BoardStatus status;
		uShort halfMoves;
		uShort fullMoves;
	};

	class Chess
	{

		int ply = 0;
		std::array<Position, MAX_POSITIONS> positions;
		std::array<Move, MAX_POSITIONS> moveHistory;

		std::vector<Move> moves;

	public:

		Chess();
		~Chess();

		void processFEN(std::string FEN);
		
		void generateMoves() {
			moves.clear();
			CHENG::generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, &moves);
		}

		void makeMove(Move move) {
			ply++;
			positions[ply] = positions[ply - 1];
			moveHistory[ply] = move;

			positions[ply].status.EP = no_Tile;

			if (move.moved_Piece == PAWN && abs(move.to - move.from) == 16) {
				positions[ply].status.EP = move.to;
			}
			else if (move.moved_Piece == ROOK) {
				if (positions[ply].status.isWhite) {
					if (move.from == A1) {
						positions[ply].status.WQcastle = false;
					}
					else if (move.from == H1) {
						positions[ply].status.WKcastle = false;

					}
				}
				else {
					if (move.from == A8) {
						positions[ply].status.BQcastle = false;
					}
					else if (move.from == H8) {
						positions[ply].status.BKcastle = false;
					}
				}
			}
			else if (move.flag == KING_CASTLE) {
				setBit(positions[ply].board.pieceLocations[positions[ply].status.isWhite][ROOK], (positions[ply].status.isWhite) ? F1 : F8);
				popBit(positions[ply].board.pieceLocations[positions[ply].status.isWhite][ROOK], (positions[ply].status.isWhite) ? H1 : H8);
				if (positions[ply].status.isWhite) positions[ply].status.WKcastle = false;
				else positions[ply].status.BKcastle = false;
			}
			else if (move.flag == QUEEN_CASTLE) {
				setBit(positions[ply].board.pieceLocations[positions[ply].status.isWhite][ROOK], (positions[ply].status.isWhite) ? D1 : D8);
				popBit(positions[ply].board.pieceLocations[positions[ply].status.isWhite][ROOK], (positions[ply].status.isWhite) ? A1 : A8);
				if (positions[ply].status.isWhite) positions[ply].status.WQcastle = false;
				else positions[ply].status.BQcastle = false;
			}
			else if (move.flag == ENPASSANT) {
				popBit(positions[ply].board.pieceLocations[!positions[ply].status.isWhite][PAWN], (positions[ply].status.isWhite) ? move.to - 8 : move.to + 8);
			}
			else if (move.flag & PROMOTION) {
				popBit(positions[ply].board.pieceLocations[positions[ply].status.isWhite][PAWN], move.from);
				if ((move.flag & 0b1011) == KNIGHT_PROMOTION) {
					setBit(positions[ply].board.pieceLocations[positions[ply].status.isWhite][KNIGHT], move.to);
				}
				else if ((move.flag & 0b1011) == BISHOP_PROMOTION) {
					setBit(positions[ply].board.pieceLocations[positions[ply].status.isWhite][BISHOP], move.to);
				}
				else if ((move.flag & 0b1011) == ROOK_PROMOTION) {
					setBit(positions[ply].board.pieceLocations[positions[ply].status.isWhite][ROOK], move.to);
				}
				else {
					setBit(positions[ply].board.pieceLocations[positions[ply].status.isWhite][QUEEN], move.to);
				}
			}
			else if (move.moved_Piece == KING) {
				if (positions[ply].status.isWhite) {
					positions[ply].status.WKcastle = false;
					positions[ply].status.WQcastle = false;
				}
				else {
					positions[ply].status.BKcastle = false;
					positions[ply].status.BQcastle = false;
				}
			}
			
			BitBoard moveBB = u64(1ULL << move.from) | u64(1ULL << move.to);
			
			if (move.flag & CAPTURE && move.flag != ENPASSANT) {
				BitBoard notTaken = ~(1ULL << move.to);
				for (BitBoard& enemyPieceBB : positions[ply].board.pieceLocations[!positions[ply].status.isWhite]) {
					enemyPieceBB &= notTaken;
				}
			}
			if (!(move.flag & PROMOTION)) {
				positions[ply].board.pieceLocations[positions[ply].status.isWhite][move.moved_Piece] ^= moveBB;
			}
			

			positions[ply].status.isWhite = !positions[ply].status.isWhite;
			positions[ply].halfMoves++;
			if (positions[ply].status.isWhite)
				positions[ply].fullMoves++;

			positions[ply].board.mergeBoth();

		}			
		
		void unmakeMove() {
			ply = ply - 1;
		}

		perftResults perft(int depth) {
			if (depth == 0) {
				perftResults result = perftResults();
				if (moveHistory[ply].flag == ENPASSANT) {
					result.eps++;
				}
				else if (moveHistory[ply].flag & PROMOTION) {
					result.promotions++;
				}
				else if (moveHistory[ply].flag == KING_CASTLE || moveHistory[ply].flag == QUEEN_CASTLE) {
					result.castles++;
				}
				else {
					//std::cout << _BitBoard(positions[ply - 1].board.occupied[BOTH]) << std::endl;
					//std::cout << _BitBoard(positions[ply].board.occupied[BOTH]) << std::endl;
					//std::cout << moveHistory[ply].from << " " << moveHistory[ply].to << std::endl;
				}

				if (moveHistory[ply].flag & CAPTURE) {
					result.captures++;
				}

				result.nodes++;
				return result;
				
			}

			perftResults count = perftResults();

			generateMoves();
			std::vector<Move> moves = this->moves;
			
			for (Move move : moves) {
				makeMove(move);
				//std::cout << _BitBoard(positions[ply].board.occupied[BOTH]) << std::endl;
				count = count + perft(depth - 1);
				unmakeMove();
			}

			return count;

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

		friend std::ostream& operator<<(std::ostream& os, const Chess& bb) {

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