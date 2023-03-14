#pragma once
#include <iostream>
#include <assert.h>
#include <string_view>

#include "utility.h"
#include "Magic.h"
#include "BitBoard.h"
#include "Board.h"
#include "Move.h"

namespace KRONOS {

	// bitboard stating which tiles should be empty and not being attacked for castling
	static const u64 WnotOccQ = 0b0000'1110ULL;
	static const u64 WnotAtkQ = 0b0001'1100ULL;

	static const u64 WnotOccK = 0b0110'0000ULL;
	static const u64 WnotAtkK = 0b0111'0000ULL;

	static const u64 BnotOccQ = WnotOccQ << 56;
	static const u64 BnotAtkQ = WnotAtkQ << 56;

	static const u64 BnotOccK = WnotOccK << 56;
	static const u64 BnotAtkK = WnotAtkK << 56;

	struct BoardStatus
	{
		bool isWhite;
		int EP;

		bool WKcastle;
		bool WQcastle;
		bool BKcastle;
		bool BQcastle;

	    constexpr BoardStatus() : isWhite(true), EP(no_Tile), WKcastle(true), WQcastle(true), BKcastle(true), BQcastle(true) {}
		constexpr BoardStatus(bool isWhite, int EP, bool WKcastle, bool WQcastle, bool BKcastle, bool BQcastle) : isWhite(isWhite), EP(EP), WKcastle(WKcastle), WQcastle(WQcastle), BKcastle(BKcastle), BQcastle(BQcastle) {}
		constexpr BoardStatus(const BoardStatus& other) : isWhite(other.isWhite), EP(other.EP), WKcastle(other.WKcastle), WQcastle(other.WQcastle), BKcastle(other.BKcastle), BQcastle(other.BQcastle) {}

		// returns whether king side castling is possible
		constexpr bool kingCastleRights(const BitBoard occ, const BitBoard atk, const BitBoard rooks, bool isWhite) const {
			if (isWhite) {
				if (!WKcastle)
					return false;
				if (WnotOccK & occ)
					return false;
				if (WnotAtkK & atk)
					return false;
				if (!getBit(rooks, H1))
					return false;
			}
			else {
				if (!BKcastle)
					return false;
				if (BnotOccK & occ)
					return false;
				if (BnotAtkK & atk)
					return false;
				if (!getBit(rooks, H8))
					return false;
			}
			return true;
		}

		// returns whether queen side castling is possible
		constexpr bool queenCastleRights(const BitBoard occ, const BitBoard atk, const BitBoard rooks, bool isWhite) const {
			if (isWhite) {
				if (!WQcastle)
					return false;
				if (WnotOccQ & occ)
					return false;
				if (WnotAtkQ & atk)
					return false;
				if (!getBit(rooks, A1))
					return false;
			}
			else {
				if (!BQcastle)
					return false;
				if (BnotOccQ & occ)
					return false;
				if (BnotAtkQ & atk)
					return false;
				if (!getBit(rooks, A8))
					return false;
			}
			return true;
		}

	};

	struct Position {
		Board board;
		BoardStatus status;
		uShort halfMoves = 0;
		uShort fullMoves = 0;

		u64 hash = 0;

		constexpr Position() {}
		constexpr Position(Board b, BoardStatus s) : board(b), status(s), halfMoves(0), fullMoves(0), hash(0) {}
		constexpr Position(const Position& other) {
			this->board = other.board;
			this->status = other.status;
			this->halfMoves = other.halfMoves;
			this->fullMoves = other.fullMoves;
		}
		constexpr void setHash(u64 newHash) { this->hash = newHash; }
		// returns the piece type of a piece at a specific tile
		constexpr u8 getPieceType(int tile) const {
			u64 toBB = 1ULL << tile;
			for (u8 p = 0; p < 6; p++) {
				if ((board.pieceLocations[BLACK][p] | board.pieceLocations[WHITE][p]) & toBB) {
					return p;
				}
			}
			return 6;
		}
		// returns the SEE GE score of a move
		bool SEE_GE(Move& move, int threshold) const;
	private:
		static int const PieceValues[6];
	};

	inline int const Position::PieceValues[6] = { 100, 300, 300, 500, 900 };

	enum MoveFlagsAndMasks {
		QUIET            = 0b0000,
		KING_CASTLE      = 0b0001,
		QUEEN_CASTLE     = 0b0010,
		ENPASSANT        = 0b0101,
		
		KNIGHT_PROMOTION = 0b1000,
		BISHOP_PROMOTION = 0b1001,
		ROOK_PROMOTION   = 0b1010,
		QUEEN_PROMOTION  = 0b1011,

		CAPTURE          = 0b0100,
		PROMOTION        = 0b1000
	};

	// initialises tablebases
	extern void initMGVars();

	// converts an integer move to a Kronos Move
	extern inline Move MoveIntToMove(uint16_t move, const Position* position);
	// updates a position with a new move
	extern constexpr void updatePosition(Position& position, Move move);
	// returns a bitboard of tiles that are empty or have an enemy
	extern constexpr BitBoard EnemyAndEmpty(const Board& brd, bool isWhite);
	// return a bitboard of tiles that are empty
	extern constexpr BitBoard Empty(const Board& brd);
	// generates a bitboard of brd attacking diagonally to the left
	extern constexpr BitBoard pawnAttackLeft(BitBoard brd, bool isWhite);
	// generates a bitabord of brd attacking diagonally to the right
	extern constexpr BitBoard pawnAttackRight(BitBoard brd, bool isWhite);
	// generates a bitboard of brd attacking diagonally in both directions
	extern constexpr BitBoard generatePawnAttacks(BitBoard brd, bool isWhite);
	// returns the bitboard mask of the rank where ep can happen
	extern constexpr BitBoard epRank(bool isWhite);
	// returns whether a pawn can take with en passant
	extern constexpr BitBoard getPawnEP(int pawnPos, int EPtile, bool isWhite);
	// generates a bitabord of brd pushing forwards
	extern constexpr BitBoard pawnPush(BitBoard brd, bool isWhite);
	// returns the bitboard mask of the starting rank
	extern constexpr BitBoard pawnStartRank(bool isWhite);
	// returns the bitboard mask of the promotion rank
	extern constexpr BitBoard pawnPromRank(bool isWhite);
	// generates pawn moves
	extern constexpr BitBoard generatePawnMoves(int pawnPos, const Board& brd, bool isWhite);
	// generates knight attacks
	extern constexpr BitBoard generateKnightAttacks(BitBoard brd);
	// generates king attacks
	extern constexpr BitBoard generateKingAttacks(BitBoard brd);
	// gets the bitboard of a pawn push
	extern constexpr BitBoard getPawnPush(int tile, bool isWhite);
	// gets the bitboard of a pawns attacks
	extern constexpr BitBoard getPawnAttacks(int tile, bool isWhite);
	// gets the bitboard of a knights attacks
	extern constexpr BitBoard getKnightAttacks(int tile);
	// gets the bitboard of a kings attacks
	extern constexpr BitBoard getKingAttacks(int tile);
	// gets the bitboard of a single bit
	extern constexpr BitBoard getTileBB(int tile);
	// generate the checkmaask for diagonal sliding pieces
	extern constexpr void bishopCheckMask(const int bishopPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan);
	// generate the checkmask for othorgonal sliding pieces
	extern constexpr void rookCheckMask(const int rookPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan);
	// generate the checkmask for queens
	extern constexpr void queenCheckMask(const int queenPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan);
	// highlight tiles between two tiles
	extern constexpr BitBoard highlightTilesBetween(int start, int end);
	// adds moves to the list
	template <Pieces pieceType>
	extern constexpr void addMoves(BitBoard movesBB, BitBoard capturesBB, int from, Move_List& moves);
	// adds pawn moves to the list
	extern constexpr void addPawnMoves(BitBoard movesBB, BitBoard captureBB, BitBoard epBB, int from, bool isWhite, Move_List& moves);
	// check if a position is in check
	extern bool inCheck(const Position& position);
	// generate moves
	extern constexpr void generateMoves(bool isWhite, const Board& brd, const BoardStatus& st, Move_List& moves);
	// perft test
	extern void perftTest(Position startPos, int depth);
}