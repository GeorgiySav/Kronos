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
		constexpr u8 getPieceType(int tile) {
			u64 toBB = 1ULL << tile;
			for (u8 p = 0; p < 6; p++) {
				if ((board.pieceLocations[BLACK][p] | board.pieceLocations[WHITE][p]) & toBB) {
					return p;
				}
			}
		}
	};

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
		PROMOTION        = 0b1000,

	};

	extern inline Move MoveIntToMove(uint16_t move, const Position* position);

	extern inline void updatePosition(Position& position, Move move);
	extern inline Position newPosition(Position curPos, Move move);
	extern constexpr BitBoard EnemyAndEmpty(const Board& brd, bool isWhite);
	extern constexpr BitBoard Empty(const Board& brd);
	extern constexpr BitBoard pawnAttackLeft(BitBoard brd, bool isWhite);
	extern constexpr BitBoard pawnAttackRight(BitBoard brd, bool isWhite);
	extern constexpr BitBoard getPawnAttacks(BitBoard brd, bool isWhite);
	extern constexpr BitBoard epRank(bool isWhite);
	extern constexpr BitBoard getPawnEP(BitBoard brd, int EPtile, bool isWhite);
	extern constexpr BitBoard pawnPush(BitBoard brd, bool isWhite);
	extern constexpr BitBoard pawnStartRank(bool isWhite);
	extern constexpr BitBoard pawnPromRank(bool isWhite);
	extern constexpr BitBoard getPawnMoves(int pawnPos, const Board& brd, bool isWhite);
	extern constexpr BitBoard getKnightAttacks(BitBoard brd);
	extern constexpr BitBoard getKingAttacks(BitBoard brd);
	extern constexpr void bishopCheckMask(const int bishopPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan);
	extern constexpr void rookCheckMask(const int rookPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan);
	extern constexpr void queenCheckMask(const int queenPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan);
	extern constexpr BitBoard highlightTilesBetween(int start, int end);
	template <Pieces pieceType>
	extern constexpr void addMoves(BitBoard movesBB, BitBoard capturesBB, int from, std::vector<Move>* moves);
	extern constexpr void addPawnMoves(BitBoard movesBB, BitBoard captureBB, BitBoard epBB, int from, bool isWhite, std::vector<Move>* moves);
	extern constexpr bool inCheck(Position& position);
	extern inline void generateMoves(bool isWhite, Board& brd, BoardStatus& st, std::vector<Move>* moves);
	extern inline BitBoard generateAttacksToSquare(Position* position, int tile);
}