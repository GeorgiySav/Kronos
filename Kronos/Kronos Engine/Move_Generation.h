#pragma once
#include <iostream>
#include <assert.h>
#include <string_view>

#include "utility.h"
#include "Magic.h"
#include "BitBoard.h"
#include "Board.h"

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

	    BoardStatus() : isWhite(true), EP(no_Tile), WKcastle(true), WQcastle(true), BKcastle(true), BQcastle(true) {}
		BoardStatus(bool isWhite, int EP, bool WKcastle, bool WQcastle, bool BKcastle, bool BQcastle) : isWhite(isWhite), EP(EP), WKcastle(WKcastle), WQcastle(WQcastle), BKcastle(BKcastle), BQcastle(BQcastle) {}

		bool kingCastleRights(const BitBoard occ, const BitBoard atk, const BitBoard rooks, bool isWhite) const {
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

		bool queenCastleRights(const BitBoard occ, const BitBoard atk, const BitBoard rooks, bool isWhite) const {
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

	struct Move {

		int from;
		int to;
		int flag;
		
		Pieces moved_Piece;

		Move() : from(0), to(0), flag(0), moved_Piece(PAWN) {}
		Move(int from, int to, int flag, Pieces moved_Piece) : from(from), to(to), flag(flag), moved_Piece(moved_Piece) {}
	
	};

	sinline BitBoard EnemyAndEmpty(const Board& brd, bool isWhite) {
		if (isWhite) return ~brd.occupied[WHITE];
		return ~brd.occupied[BLACK];
	}

	sinline BitBoard Empty(const Board& brd) {
		return ~brd.occupied[BOTH];
	}
	
	sinline BitBoard pawnAttackLeft(BitBoard brd, bool isWhite) {
		if (isWhite) return NWOne(brd);
		return SWOne(brd);
	}

	sinline BitBoard pawnAttackRight(BitBoard brd, bool isWhite) {
		if (isWhite) return NEOne(brd);
		return SEOne(brd);
	}

	sinline BitBoard getPawnAttacks(BitBoard brd, bool isWhite) {
		if (isWhite) return (pawnAttackLeft(brd, isWhite) | pawnAttackRight(brd, isWhite));
		return (pawnAttackLeft(brd, isWhite) | pawnAttackRight(brd, isWhite));
	}

	sinline BitBoard getPawnEP(BitBoard brd, int EPtile, bool isWhite) {
		if (getPawnAttacks(brd, isWhite) & (1ULL << EPtile)) {
			return 1ULL << EPtile;
		}
		return 0ULL;
	}

	sinline BitBoard pawnPush(BitBoard brd, bool isWhite) {
		if (isWhite) return northOne(brd);
		return southOne(brd);
	}

	sinline BitBoard pawnStartRank(bool isWhite) {
		if (isWhite) return rankMask[RANK_2];
		return rankMask[RANK_7];
	}

	sinline BitBoard pawnPromRank(bool isWhite) {
		if (isWhite) return rankMask[RANK_8];
		return rankMask[RANK_1];
	}

	sinline BitBoard epRank(bool isWhite) {
		if (isWhite) return rankMask[RANK_4];
		return rankMask[RANK_5];
	}

	sinline BitBoard getPawnMoves(int pawnPos, const Board& brd, bool isWhite) {
		BitBoard moves = EMPTY;

		moves |= pawnPush(1ULL << pawnPos, isWhite) & Empty(brd);
		moves |= ((pawnPush(moves, isWhite) & epRank(isWhite) & Empty(brd)));

		return moves;
	}

	sinline BitBoard knightAttacks(BitBoard brd) {

		static u64 notABfile = ~(fileMask[A] | fileMask[B]);
		static u64 notGHfile = ~(fileMask[G] | fileMask[H]);

		BitBoard attacks = 0ULL;

		attacks |= ((brd <<  6) | (brd >> 10)) & notGHfile;
		attacks |= ((brd << 15) | (brd >> 17)) & notHFile;
		attacks |= ((brd << 17) | (brd >> 15)) & notAFile;
		attacks |= ((brd << 10) | (brd >>  6)) & notABfile;

		return attacks;

	}

	sinline BitBoard kingAttacks(BitBoard brd) {
		
		BitBoard attacks = 0ULL;

		attacks |= (westOne(brd) | NWOne(brd) | SWOne(brd)) & notHFile;
		attacks |= (eastOne(brd) | NEOne(brd) | SEOne(brd)) & notAFile;
		attacks |= (northOne(brd) | southOne(brd));

		return attacks;

	}

	sinline void bishopCheckMask(const int bishopPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan) {
		setBit(checkMask, bishopPos);

		/*

			. . . . . . . .
			. . . . . . . .
			.14 . . .18 . .
			. . 7 . 9 . . .
			. . . x . . . .
			. .-9 .-7 . . .
			.-18. . .-14. .
			. . . . . . . .

			. . . . . . . .      . . . . . . . .
			. . . . . . . .      . . . . . . . .
			. . . . . . . .      . . . . . . . .
			. . K . . . . .    	 . . K . . . . .
			. . . . . . . .  --> . . . x . . . .
			. . . . . . . .      . . . . x . . .
			. . . . . B . .      . . . . . x . .
			. . . . . . . .      . . . . . . . .

			   bishop ray     &      king ray     =   blocker mask
			. . . . . . . .		 . . . . . . . .	 . . . . . . . .
			x . . . . . . .      . . . . . . . .	 . . . . . . . .
			. x . . . . . .		 . . . . . . . .	 . . . . . . . .
			. . x . . . . .		 . . K . . . . .	 . . K . . . . .
			. . . x . . . .      . . . x . . . .	 . . . x . . . .
			. . . . x . . .      . . . . x . . .	 . . . . x . . .
			. . . . . B . .      . . . . . x . .	 . . . . . B . .
			. . . . . . . .      . . . . . . x .	 . . . . . . . .

		*/

		int offset = bishopPos - kingPos;
		if (!(offset % 7)) {
			if (bishopPos < kingPos) {
				// bishop is south east from the king
				kingBan |= rays[bishopPos][NORTH_WEST];

				checkMask |= (rays[bishopPos][NORTH_WEST] & rays[kingPos][SOUTH_EAST]);

			}
			else {
				// bishop is north west from the king
				kingBan |= rays[bishopPos][SOUTH_EAST];

				checkMask |= (rays[bishopPos][SOUTH_EAST] & rays[kingPos][NORTH_WEST]);
			}
		}
		else if (!(offset % 9)) {
			if (bishopPos < kingPos) {
				// bishop is south west from the king
				kingBan |= rays[bishopPos][NORTH_EAST];

				checkMask |= (rays[bishopPos][NORTH_EAST] & rays[kingPos][SOUTH_WEST]);
			}
			else {
				// bishop is north east from the king
				kingBan |= rays[bishopPos][SOUTH_WEST];

				checkMask |= (rays[bishopPos][SOUTH_WEST] & rays[kingPos][NORTH_EAST]);
			}
		}
	}

	sinline void rookCheckMask(const int rookPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan) {

		setBit(checkMask, rookPos);

		/*

			. . . . . . . .
			. . . . . . . .
			. . . 16. . . .
			. . . 8 . . . .
		   -3-2-1 K 1 2 3 4
			. . .-8 . . . .
			. . .-16. . . .
			. . . . . . . .

		*/

		int offset = rookPos - kingPos;

		if (!(offset % 8)) {
			if (rookPos < kingPos) {
				// rook is south from the king
				kingBan |= rays[rookPos][NORTH];
				checkMask |= (rays[rookPos][NORTH] & rays[kingPos][SOUTH]);
			}
			else {
				// rook is north from the king
				kingBan |= rays[rookPos][SOUTH];
				checkMask |= (rays[rookPos][SOUTH] & rays[kingPos][NORTH]);
			}
		}
		else {
			if (rookPos < kingPos) {
				// rook is west from the king
				kingBan |= rays[rookPos][EAST];
				checkMask |= (rays[rookPos][EAST] & rays[kingPos][WEST]);
			}
			else {
				// rook is east from the king
				kingBan |= rays[rookPos][WEST];
				checkMask |= (rays[rookPos][WEST] & rays[kingPos][EAST]);
			}
		}

	}

	sinline void queenCheckMask(const int queenPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan) {
		
		int offset = queenPos - kingPos;

		if (!(offset % 7)) {
			if (queenPos < kingPos) {
				// bishop is south east from the king
				kingBan |= rays[queenPos][NORTH_WEST];
				checkMask |= (rays[queenPos][NORTH_WEST] & rays[kingPos][SOUTH_EAST]);
			}
			else {
				// bishop is north west from the king
				kingBan |= rays[queenPos][SOUTH_EAST];
				checkMask |= (rays[queenPos][SOUTH_EAST] & rays[kingPos][NORTH_WEST]);
			}
		}
		else if (!(offset % 9)) {
			if (queenPos < kingPos) {
				// bishop is south west from the king
				kingBan |= rays[queenPos][NORTH_EAST];
				checkMask |= (rays[queenPos][NORTH_EAST] & rays[kingPos][SOUTH_WEST]);
			}
			else {
				// bishop is north east from the king
				kingBan |= rays[queenPos][SOUTH_WEST];
				checkMask |= (rays[queenPos][SOUTH_WEST] & rays[kingPos][NORTH_EAST]);
			}
		}
		else if (!(offset % 8)) {
			if (queenPos < kingPos) {
				// rook is south from the king
				kingBan |= rays[queenPos][NORTH];
				checkMask |= (rays[queenPos][NORTH] & rays[kingPos][SOUTH]);
			}
			else {
				// rook is north from the king
				kingBan |= rays[queenPos][SOUTH];
				checkMask |= (rays[queenPos][SOUTH] & rays[kingPos][NORTH]);
			}
		}
		else {
			if (queenPos < kingPos) {
				// rook is west from the king
				kingBan |= rays[queenPos][EAST];
				checkMask |= (rays[queenPos][EAST] & rays[kingPos][WEST]);
			}
			else {
				// rook is east from the king
				kingBan |= rays[queenPos][WEST];
				checkMask |= (rays[queenPos][WEST] & rays[kingPos][EAST]);
			}
		}		

	}

	sinline BitBoard highlightTilesBetween(int start, int end) {
		BitBoard b = EMPTY;
		return b |= (rays[start][NORTH] & rays[end][SOUTH])
			      | (rays[start][EAST] & rays[end][WEST])
			      | (rays[start][SOUTH] & rays[end][NORTH])
			      | (rays[start][WEST] & rays[end][EAST])
			      | (rays[start][NORTH_EAST] & rays[end][SOUTH_WEST])
				  | (rays[start][NORTH_WEST] & rays[end][SOUTH_EAST])
			      | (rays[start][SOUTH_WEST] & rays[end][NORTH_EAST])
			      | (rays[start][SOUTH_EAST] & rays[end][NORTH_WEST]);
	}

	template <Pieces pieceType>
	sinline void addMoves(BitBoard movesBB, BitBoard capturesBB, int from, std::vector<Move>* moves) {
		int to;
		while (movesBB) {
			to = bitScanForward(movesBB);
			moves->push_back(Move(from, to, QUIET, pieceType));
			popBit(movesBB, to);
		}
		while (capturesBB) {
			to = bitScanForward(capturesBB);
			moves->push_back(Move(from, to, CAPTURE, pieceType));
			popBit(capturesBB, to);
		}
	}

	sinline void addPawnMoves(BitBoard movesBB, BitBoard captureBB, BitBoard epBB, int from, bool isWhite, std::vector<Move>* moves) {
		int to;
		if (epBB) {
			to = bitScanForward(epBB);
			moves->push_back(Move(from, to, ENPASSANT, PAWN));
		}
		while (movesBB) {
			to = bitScanForward(movesBB);
			if (1ULL << to & pawnPromRank(isWhite)) {
				moves->push_back(Move(from, to, KNIGHT_PROMOTION, PAWN));
				moves->push_back(Move(from, to, BISHOP_PROMOTION, PAWN));
				moves->push_back(Move(from, to, ROOK_PROMOTION, PAWN));
				moves->push_back(Move(from, to, QUEEN_PROMOTION, PAWN));
			}
			else moves->push_back(Move(from, to, QUIET, PAWN));
			popBit(movesBB, to);
		}
		while (captureBB) {
			to = bitScanForward(captureBB);
			if (1ULL << to & pawnPromRank(isWhite)) {
				moves->push_back(Move(from, to, CAPTURE | KNIGHT_PROMOTION, PAWN));
				moves->push_back(Move(from, to, CAPTURE | BISHOP_PROMOTION, PAWN));
				moves->push_back(Move(from, to, CAPTURE | ROOK_PROMOTION, PAWN));
				moves->push_back(Move(from, to, CAPTURE | QUEEN_PROMOTION, PAWN));
			}
			else moves->push_back(Move(from, to, CAPTURE, PAWN));
			popBit(captureBB, to);
		}
	}

	sinline void generateMoves(bool isWhite, const Board& brd, const BoardStatus& st, std::vector<Move>* moves) {

		bool onlyKingMoves = false;

		// temporary bitboards
		BitBoard b1;

		BitBoard pinners = 0ULL;
		BitBoard pinned = 0ULL;

		BitBoard pawns = brd.pieceLocations[isWhite][PAWN];
		BitBoard knights = brd.pieceLocations[isWhite][KNIGHT];
		BitBoard bishops = brd.pieceLocations[isWhite][BISHOP];
		BitBoard rooks = brd.pieceLocations[isWhite][ROOK];
		BitBoard queens = brd.pieceLocations[isWhite][QUEEN];
		BitBoard king = brd.pieceLocations[isWhite][KING];

		BitBoard kingBan = 0ULL;
		int kingPos = bitScanForward(brd.pieceLocations[isWhite][KING]);
		
		BitBoard ePwnAtkL = pawnAttackLeft(brd.pieceLocations[!isWhite][PAWN], !isWhite);
		BitBoard ePwnAtkR = pawnAttackRight(brd.pieceLocations[!isWhite][PAWN], !isWhite);
		BitBoard eKnightAtk = knightAttacks(brd.pieceLocations[!isWhite][KNIGHT]);

		BitBoard eB = brd.pieceLocations[!isWhite][BISHOP];
		BitBoard eBishopAtk = 0ULL;
		while (eB) {
			int tile = bitScanForward(eB);
			popBit(eB, tile);
			eBishopAtk |= getBishopAttacks(brd.occupied[BOTH], tile);
		}

		BitBoard eR = brd.pieceLocations[!isWhite][ROOK];
		BitBoard eRookAtk = 0ULL;
		while (eR) {
			int tile = bitScanForward(eR);
			popBit(eR, tile);
			eRookAtk |= getRookAttacks(brd.occupied[BOTH], tile);
		}

		BitBoard eQ = brd.pieceLocations[!isWhite][QUEEN];
		BitBoard eQueenAtk = 0ULL;
		while (eQ) {
			int tile = bitScanForward(eQ);
			popBit(eQ, tile);
			eQueenAtk |= getBishopAttacks(brd.occupied[BOTH], tile) | getRookAttacks(brd.occupied[BOTH], tile);
		}

		BitBoard eKingAtk = kingAttacks(brd.pieceLocations[!isWhite][KING]);

		kingBan = ePwnAtkL | ePwnAtkR | eKnightAtk | eBishopAtk | eRookAtk | eQueenAtk | eKingAtk;

		// check for checks
		BitBoard checkMask = UNIVERSE;

		// get position of checks made by a knight or pawn
		BitBoard checkers = (knightAttacks(1ULL << kingPos) & brd.pieceLocations[!isWhite][KNIGHT])
						   |(getPawnAttacks(king, isWhite) & brd.pieceLocations[!isWhite][PAWN]);

		// sliding checks candidates
		BitBoard canditates = (getRookAttacks(brd.occupied[!isWhite], kingPos) & (brd.pieceLocations[!isWhite][ROOK] | brd.pieceLocations[!isWhite][QUEEN]))
						    | (getBishopAttacks(brd.occupied[!isWhite], kingPos) & (brd.pieceLocations[!isWhite][BISHOP] | brd.pieceLocations[!isWhite][QUEEN]));

		while (canditates) {
			int sliderPos = bitScanForward(canditates);
			popBit(canditates, sliderPos);

			// get the squares between the slider piece and the king
			b1 = highlightTilesBetween(sliderPos, kingPos) & brd.occupied[isWhite];

			// means the king is in check
			if (b1 == EMPTY) {
				setBit(checkers, sliderPos);
			}
			else if (populationCount(b1) == 1) {
				 pinned |= b1;
				 setBit(pinners, sliderPos);
			}

		}

		switch (populationCount(checkers)) {

		case 2: {
			// double check
			onlyKingMoves = true;
		}
		case 1: {
			// single check
			checkMask = EMPTY;

			int checkerSquare = bitScanForward(checkers);
			setBit(checkMask, checkerSquare);

			if (checkers & brd.pieceLocations[!isWhite][BISHOP]) {
				bishopCheckMask(checkerSquare, kingPos, checkMask, kingBan);
			}
			else if (checkers & brd.pieceLocations[!isWhite][ROOK]) {
				rookCheckMask(checkerSquare, kingPos, checkMask, kingBan);
			}
			else if (checkers & brd.pieceLocations[!isWhite][QUEEN]) {
				queenCheckMask(checkerSquare, kingPos, checkMask, kingBan);
			}
		}

		}

		// no need to check for pins if the king is in a double check
		if (onlyKingMoves) {

			BitBoard kingMoves = kingAttacks(king) & EnemyAndEmpty(brd, isWhite) & ~(kingBan);
			BitBoard kingAttacks = kingMoves & brd.occupied[!isWhite];
			kingMoves ^= kingAttacks;

			// add moves to the list and return it
			addMoves<KING>(kingMoves, kingAttacks, kingPos, moves);
			return;
		}

		while (pinned) {
			int pinnedPos = bitScanForward(pinned);
			popBit(pinned, pinnedPos);
			
			// get the direction of the pin
			dir Dir = getDirectionFromOffset(pinnedPos - kingPos);

			// get position of the attacking piece
			int attackingSlider = bitScanForward(rays[kingPos][Dir] & pinners);

			BitBoard pinnedMask = rays[kingPos][Dir] ^ rays[attackingSlider][Dir];

			if (getBit(pawns, pinnedPos)) {
				BitBoard pawnAttacks = getPawnAttacks(1ULL << pinnedPos, isWhite) & brd.occupied[!isWhite] & pinnedMask & checkMask;
				BitBoard pawnMoves = getPawnMoves(pinnedPos, brd, isWhite) & pinnedMask & checkMask;		
				BitBoard pawnEp = getPawnEP(1ULL << pinnedPos, st.EP, isWhite) & pinnedMask & checkMask;

				addPawnMoves(pawnMoves, pawnAttacks, pawnEp, pinnedPos, isWhite, moves);

				popBit(pawns, pinnedPos);
			}
			else if (getBit(knights, pinnedPos)) {
				BitBoard knightMoves = knightAttacks(1ULL << pinnedPos) & EnemyAndEmpty(brd, isWhite) & pinnedMask & checkMask;
				BitBoard knightAttacks = knightMoves & brd.occupied[!isWhite];
				knightMoves ^= knightAttacks;

				addMoves<KNIGHT>(knightMoves, knightAttacks, pinnedPos, moves);

				popBit(knights, pinnedPos);
			}
			else if (getBit(bishops, pinnedPos)) {
				BitBoard bishopMoves = getBishopAttacks(brd.occupied[BOTH], pinnedPos) & EnemyAndEmpty(brd, isWhite) & pinnedMask & checkMask;
				BitBoard bishopAttacks = bishopMoves & brd.occupied[!isWhite];
				bishopMoves ^= bishopAttacks;

				addMoves<BISHOP>(bishopMoves, bishopAttacks, pinnedPos, moves);

				popBit(bishops, pinnedPos);
			}
			else if (getBit(rooks, pinnedPos)) {
				BitBoard rookMoves = getRookAttacks(brd.occupied[BOTH], pinnedPos) & EnemyAndEmpty(brd, isWhite) & pinnedMask & checkMask;
				BitBoard rookAttacks = rookMoves & brd.occupied[!isWhite];
				rookMoves ^= rookAttacks;

				addMoves<ROOK>(rookMoves, rookAttacks, pinnedPos, moves);

				popBit(rooks, pinnedPos);
			}
			else if (getBit(queens, pinnedPos)) {
				BitBoard queenMoves = (getRookAttacks(brd.occupied[BOTH], pinnedPos) | getBishopAttacks(brd.occupied[BOTH], pinnedPos)) & EnemyAndEmpty(brd, isWhite) & pinnedMask & checkMask;
				BitBoard queenAttacks = queenMoves & brd.occupied[!isWhite];
				queenMoves ^= queenAttacks;

				addMoves<QUEEN>(queenMoves, queenAttacks, pinnedPos, moves);
				
				popBit(queens, pinnedPos);
			}
		}

		
		// generate all other moves
		int from;
		while (pawns) {
			from = bitScanForward(pawns);
			popBit(pawns, from);

			BitBoard pawnAttacks = getPawnAttacks(1ULL << from, isWhite) & brd.occupied[!isWhite] & checkMask;
			BitBoard pawnMoves = getPawnMoves(from, brd, isWhite) & checkMask;
			BitBoard pawnEp = getPawnEP(1ULL << from, st.EP, isWhite) & checkMask;

			/* 
			
				have to check for the special case of an enpassant pin 
			
				. . . . . . . .  can't use enpassant as that will put the king in check
				. . . . . . . .
				. . . . . . . .
				. . . . . . . .
				k . . p p . . q
				. . . . . . . .
				. . . . . . . .
				. . . . . . . .

			*/

			if (pawnEp) {
				// first we check whether the king is on the same rank as the pawn
				if (king & epRank(!isWhite)) {
					// we have to check whether there is either a rook or a queen on the same rank
					// also we have to check that the attacking piece isn't blocked either
					if (getRookAttacks(brd.occupied[BOTH] & ~((1ULL << from) | (1ULL << (isWhite ? (st.EP - 8) : (st.EP + 8)))), kingPos) 
						& epRank(!isWhite) 
						& (brd.pieceLocations[!isWhite][ROOK] | brd.pieceLocations[!isWhite][QUEEN])) {
						pawnEp = EMPTY;
					}
				}
			}

			addPawnMoves(pawnMoves, pawnAttacks, pawnEp, from, isWhite, moves);
		}

		while (knights) {
			from = bitScanForward(knights);
			popBit(knights, from);

			BitBoard knightMoves = knightAttacks(1ULL << from) & EnemyAndEmpty(brd, isWhite) & checkMask;
			BitBoard knightAttacks = knightMoves & brd.occupied[!isWhite];
			knightMoves ^= knightAttacks;

			addMoves<KNIGHT>(knightMoves, knightAttacks, from, moves);
		}

		while (bishops) {
			from = bitScanForward(bishops);
			popBit(bishops, from);

			BitBoard bishopMoves = getBishopAttacks(brd.occupied[BOTH], from) & EnemyAndEmpty(brd, isWhite) & checkMask;
			BitBoard bishopAttacks = bishopMoves & brd.occupied[!isWhite];
			bishopMoves ^= bishopAttacks;

			addMoves<BISHOP>(bishopMoves, bishopAttacks, from, moves);
		}

		while (rooks) {
			from = bitScanForward(rooks);
			popBit(rooks, from);

			BitBoard rookMoves = getRookAttacks(brd.occupied[BOTH], from) & EnemyAndEmpty(brd, isWhite) & checkMask;
			BitBoard rookAttacks = rookMoves & brd.occupied[!isWhite];
			rookMoves ^= rookAttacks;
			
			addMoves<ROOK>(rookMoves, rookAttacks, from, moves);
		}

		while (queens) {
			from = bitScanForward(queens);
			popBit(queens, from);

			BitBoard queenMoves = (getBishopAttacks(brd.occupied[BOTH], from) | getRookAttacks(brd.occupied[BOTH], from)) & EnemyAndEmpty(brd, isWhite) & checkMask;
			BitBoard queenAttacks = queenMoves & brd.occupied[!isWhite];
			queenMoves ^= queenAttacks;

			addMoves<QUEEN>(queenMoves, queenAttacks, from, moves);
		}

		from = bitScanForward(king);
		
		BitBoard kingMoves = kingAttacks(king) & EnemyAndEmpty(brd, isWhite) & ~kingBan;
		BitBoard kingAttacks = kingMoves & brd.occupied[!isWhite];
		kingMoves ^= kingAttacks;
		
		addMoves<KING>(kingMoves, kingAttacks, from, moves);

		if (st.kingCastleRights(brd.occupied[BOTH], kingBan, brd.pieceLocations[isWhite][ROOK], isWhite)) {
			moves->push_back(Move(kingPos, kingPos + 2, KING_CASTLE, KING));
		}

		if (st.queenCastleRights(brd.occupied[BOTH], kingBan, brd.pieceLocations[isWhite][ROOK], isWhite)) {
			moves->push_back(Move(kingPos, kingPos - 2, QUEEN_CASTLE, KING));
		}

	}

}