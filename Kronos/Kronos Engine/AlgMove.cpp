#include "AlgMove.h"
#include <sstream>
#include <string>
#include <iostream>

namespace KRONOS {
	// gets the file in character format
	std::string getFileString(int tile) {
		return std::string(1, char(97 + (tile % 8)));
	}
	// gets the rank in character format
	std::string getRankString(int tile) {
		return std::to_string(int(tile / 8) + 1);
	}
	
	std::string KronosMoveToAlgebraic(const Move& move, const Position& position) {
		const u8& from = move.from;
		const u8& to = move.to;
		const u8& movedPiece = move.moved_Piece;
		const u8& flag = move.flag;

		std::string str = "";

		if (move == NULL_MOVE)
			return str;

		if (flag == KING_CASTLE)
			str = "0-0";
		else if (flag == QUEEN_CASTLE)
			str = "0-0-0";
		else {
			// pawn moves are characterised by the file, while other pieces by their character
			if (movedPiece == PAWN && flag & CAPTURE)
				str = getFileString(from);
			else if (movedPiece == KNIGHT)
				str = "N";
			else if (movedPiece == BISHOP)
				str = "B";
			else if (movedPiece == ROOK)
				str = "R";
			else if (movedPiece == QUEEN)
				str = "Q";
			else if (movedPiece == KING) 
				str = "K";

			// pieces of the same type may be able to move to the same tile so they must be differentiated
			if (movedPiece != PAWN && movedPiece != KING) {
				// check if other of the same pieces can move to the same tile
				BitBoard others = EMPTY;
				if (movedPiece == KNIGHT)
					others = getKnightAttacks(to);
				else if (movedPiece == BISHOP)
					others = getBishopAttacks(position.board.occupied[BOTH], to);
				else if (movedPiece == ROOK)
					others = getRookAttacks(position.board.occupied[BOTH], to);
				else if (movedPiece == QUEEN)
					others = getBishopAttacks(position.board.occupied[BOTH], to) | getRookAttacks(position.board.occupied[BOTH], to);

				others = others & position.board.pieceLocations[position.status.isWhite][movedPiece] & ~getTileBB(from);

				if (others) {
					// if you only need to differentiate by file, only by file
					// can't by file, then try rank, if you still can't, then both
					bool canByFile = (getFileMask(from) & others) == EMPTY;
					if (canByFile)
						str.append(getFileString(from));
					else {
						bool canByRank = (getRankMask(from) & others) == EMPTY;
						if (canByRank)
							str.append(getRankString(from));
						else
							str.append(getFileString(from) + getRankString(from));
					}
				}
			}

			if (flag & CAPTURE)
				str.append("x");

			str.append(boardTilesStrings[to]);

			if (flag & PROMOTION) {
				str.append("=");
				if ((flag & 0b1011) == KNIGHT_PROMOTION)
					str.append("N");
				else if ((flag & 0b1011) == BISHOP_PROMOTION)
					str.append("B");
				else if ((flag & 0b1011) == ROOK_PROMOTION)
					str.append("R");
				else
					str.append("Q");
			}
		}

		Position afterMove = position;
		updatePosition(afterMove, move);
		if (inCheck(afterMove)) {
			Move_List moves;
			generateMoves(afterMove.status.isWhite, afterMove.board, afterMove.status, moves);
			if (moves.size() == 0)
				str.append("#");
			else
				str.append("+");
		}

		return str;
	}
}