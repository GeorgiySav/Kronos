#include "Move_Generation.h"

#include "Rays.h"
#include "Zobrist_Hashing.h"

namespace KRONOS
{

	bool Position::SEE_GE(Move& move, int threshold) const {
		int from = move.from;
		int to = move.to;
		int movedPiece = move.moved_Piece;
		bool promo = move.flag & PROMOTION;
		u64 toBB = 1ULL << to;

		if (promo)
			return true;

		int swap = PieceValues[getPieceType(to)] - threshold;
		if (swap < 0)
			return false;

		swap = PieceValues[movedPiece] - swap;
		if (swap <= 0)
			return true;

		BitBoard occ = board.occupied[BOTH] ^ (1ULL << from) ^ (1ULL << to);
		BitBoard attackers = (getPawnAttacks(toBB, WHITE) & board.pieceLocations[BLACK][PAWN])
			               | (getPawnAttacks(toBB, BLACK) & board.pieceLocations[WHITE][PAWN])
			               | (getKnightAttacks(toBB) & (board.pieceLocations[WHITE][KNIGHT] | board.pieceLocations[BLACK][KNIGHT]))
			               | (getBishopAttacks(occ, to) & (board.pieceLocations[WHITE][BISHOP] | board.pieceLocations[BLACK][BISHOP] | board.pieceLocations[WHITE][QUEEN] | board.pieceLocations[BLACK][QUEEN]))
			               | (getRookAttacks(occ, to) & (board.pieceLocations[WHITE][ROOK] | board.pieceLocations[BLACK][ROOK] | board.pieceLocations[WHITE][QUEEN] | board.pieceLocations[BLACK][QUEEN]))
			               | (getKingAttacks(toBB) & (board.pieceLocations[WHITE][KING] | board.pieceLocations[BLACK][KING]));
		popBit(attackers, from);

		BitBoard sideToMoveAtks, bb;
		bool sideToMove = status.isWhite, res = true;

		while (true) {
			sideToMove = !sideToMove;
			attackers &= occ;
			sideToMoveAtks = attackers & board.occupied[sideToMove];

			// if the side to move has no more attackers then we quiet as the side to move has lost
			if (sideToMoveAtks == EMPTY)
				break;

			res = !res;

			// find the next least valuable attacker and remove it
			// then check if there are any more attackers hidden by it
			if (bb = sideToMoveAtks & board.pieceLocations[sideToMove][PAWN]) {
				if ((swap = PieceValues[PAWN] - swap) < res)
					break;
				popBit(occ, bitScanForward(bb));
				attackers |= getBishopAttacks(occ, to) & (board.pieceLocations[WHITE][BISHOP] | board.pieceLocations[BLACK][BISHOP] | board.pieceLocations[WHITE][QUEEN] | board.pieceLocations[BLACK][QUEEN]);
			}
			else if (bb = sideToMoveAtks & board.pieceLocations[sideToMove][KNIGHT]) {
				if ((swap = PieceValues[KNIGHT] - swap) < res)
					break;
				popBit(occ, bitScanForward(bb));
			}
			else if (bb = sideToMoveAtks & board.pieceLocations[sideToMove][BISHOP]) {
				if ((swap = PieceValues[BISHOP] - swap) < res)
					break;
				popBit(occ, bitScanForward(bb));
				attackers |= getBishopAttacks(occ, to) & (board.pieceLocations[WHITE][BISHOP] | board.pieceLocations[BLACK][BISHOP] | board.pieceLocations[WHITE][QUEEN] | board.pieceLocations[BLACK][QUEEN]);
			}
			else if (bb = sideToMoveAtks & board.pieceLocations[sideToMove][ROOK]) {
				if ((swap = PieceValues[ROOK] - swap) < res)
					break;
				popBit(occ, bitScanForward(bb));
				attackers |= getRookAttacks(occ, to) & (board.pieceLocations[WHITE][ROOK] | board.pieceLocations[BLACK][ROOK] | board.pieceLocations[WHITE][QUEEN] | board.pieceLocations[BLACK][QUEEN]);
			}
			else if (bb = sideToMoveAtks & board.pieceLocations[sideToMove][QUEEN]) {
				if ((swap = PieceValues[QUEEN] - swap) < res)
					break;
				popBit(occ, bitScanForward(bb));
				attackers |= (getBishopAttacks(occ, to) & (board.pieceLocations[WHITE][BISHOP] | board.pieceLocations[BLACK][BISHOP] | board.pieceLocations[WHITE][QUEEN] | board.pieceLocations[BLACK][QUEEN]))
					| (getRookAttacks(occ, to) & (board.pieceLocations[WHITE][ROOK] | board.pieceLocations[BLACK][ROOK] | board.pieceLocations[WHITE][QUEEN] | board.pieceLocations[BLACK][QUEEN]));
			}
			else if (bb = sideToMoveAtks & board.pieceLocations[sideToMove][KING]) {
				// if we "take" with the king and there are still enemy attackers, when we lost
				return (attackers & board.occupied[!sideToMove]) ? !res : res;
			}
		}
		return res;
	}

	bool Position::givesCheck(Move& move) {
		BitBoard toBB = 1ULL << move.to;
		if (move.moved_Piece == PAWN) {
			if (toBB & getPawnAttacks(board.pieceLocations[!status.isWhite][KING], !status.isWhite))
				return true;
		}
		else if (move.moved_Piece == KNIGHT) {
			if (toBB & getKnightAttacks(board.pieceLocations[!status.isWhite][KING]))
				return true;
		}
		else if (move.moved_Piece == BISHOP) {
			if (toBB & getBishopAttacks(board.occupied[BOTH], bitScanForward(board.pieceLocations[!status.isWhite][KING])))
				return true;
		}
		else if (move.moved_Piece == ROOK) {
			if (toBB & getRookAttacks(board.occupied[BOTH], bitScanForward(board.pieceLocations[!status.isWhite][KING])))
				return true;
		}
		else if (move.moved_Piece == QUEEN) {
			if (toBB & (getBishopAttacks(board.occupied[BOTH], bitScanForward(board.pieceLocations[!status.isWhite][KING])) | getRookAttacks(board.occupied[BOTH], bitScanForward(board.pieceLocations[!status.isWhite][KING]))))
				return true;
		}
		return false;
	}

	inline Move MoveIntToMove(uint16_t move, const Position* position)
	{
		if (move == 0)
			return NULL_MOVE;

		Move newMove;

		newMove.from = (move & 0b1111'1100'0000) >> 6;
		newMove.to = (move & 0b11'1111);

		switch ((move & 0b1111'0000'0000'0000) >> 12)
		{
		case QUEEN_PROMOTION:
			newMove.flag = QUEEN_PROMOTION;
			break;
		case ROOK_PROMOTION:
			newMove.flag = ROOK_PROMOTION;
			break;
		case BISHOP_PROMOTION:
			newMove.flag = BISHOP_PROMOTION;
			break;
		case KNIGHT_PROMOTION:
			newMove.flag = KNIGHT_PROMOTION;
			break;
		}

		for (int p = 0; p < 6; p++) {
			if (position->board.pieceLocations[position->status.isWhite][p] & (1ULL << newMove.from)) {
				newMove.moved_Piece = p;
				break;
			}
		}

		if (newMove.moved_Piece == PAWN) {
			// check for enpassant
			if ((abs(newMove.to - newMove.from) == 7 || abs(newMove.to - newMove.from) == 9) && !((1ULL << newMove.to) & position->board.occupied[BOTH])) {
				newMove.flag = ENPASSANT;
			}
		}
		else if (newMove.moved_Piece == KING) {
			if (newMove.from == E1) {
				if (newMove.to == G1)
					newMove.flag = KING_CASTLE;
				else if (newMove.to == C1)
					newMove.flag = QUEEN_CASTLE;
			}
			else if (newMove.from == E8) {
				if (newMove.to == G8)
					newMove.flag = KING_CASTLE;
				else if (newMove.to == C8)
					newMove.flag = QUEEN_CASTLE;
			}
		}

		if ((1ULL << newMove.to) & position->board.occupied[BOTH])
			newMove.flag |= CAPTURE;

		return newMove;

	}

	inline void updatePosition(Position& position, Move move) {

		position.status.EP = no_Tile;
		
		position.halfMoves++;
		if (position.status.isWhite)
			position.fullMoves++;

		if (move.moved_Piece == PAWN) {
			if (abs(move.to - move.from) == 16) position.status.EP = move.to + (position.status.isWhite ? -8 : 8);
			else if (move.flag & PROMOTION) {
				popBit(position.board.pieceLocations[position.status.isWhite][PAWN], move.from);
				if ((move.flag & 0b1011) == KNIGHT_PROMOTION)      setBit(position.board.pieceLocations[position.status.isWhite][KNIGHT], move.to);
				else if ((move.flag & 0b1011) == BISHOP_PROMOTION) setBit(position.board.pieceLocations[position.status.isWhite][BISHOP], move.to);
				else if ((move.flag & 0b1011) == ROOK_PROMOTION)   setBit(position.board.pieceLocations[position.status.isWhite][ROOK], move.to);
				else                                               setBit(position.board.pieceLocations[position.status.isWhite][QUEEN], move.to);
			}
			else if (move.flag == ENPASSANT)
				popBit(position.board.pieceLocations[!position.status.isWhite][PAWN], ((position.status.isWhite) ? (move.to - 8) : (move.to + 8)));
			position.halfMoves = 0;
		}
		else if (move.moved_Piece == ROOK) {
			if (position.status.isWhite) {
				if (move.from == A1)      position.status.WQcastle = false;
				else if (move.from == H1) position.status.WKcastle = false;
			}
			else {
				if (move.from == A8)      position.status.BQcastle = false;
				else if (move.from == H8) position.status.BKcastle = false;
			}
		}
		else if (move.moved_Piece == KING) {
			if (position.status.isWhite) { position.status.WKcastle = false; position.status.WQcastle = false; }
			else { position.status.BKcastle = false; position.status.BQcastle = false; }

			if (move.flag == KING_CASTLE) {
				setBit(position.board.pieceLocations[position.status.isWhite][ROOK], ((position.status.isWhite) ? F1 : F8));
				popBit(position.board.pieceLocations[position.status.isWhite][ROOK], ((position.status.isWhite) ? H1 : H8));
			}
			else if (move.flag == QUEEN_CASTLE) {
				setBit(position.board.pieceLocations[position.status.isWhite][ROOK], ((position.status.isWhite) ? D1 : D8));
				popBit(position.board.pieceLocations[position.status.isWhite][ROOK], ((position.status.isWhite) ? A1 : A8));
			}
		}

		BitBoard moveBB = (1ULL << move.from) | (1ULL << move.to);
		if (move.flag & CAPTURE && move.flag != ENPASSANT) {
			BitBoard notTaken = ~(1ULL << move.to);
			for (BitBoard& enemyPieceBB : position.board.pieceLocations[!position.status.isWhite])
				enemyPieceBB &= notTaken;
			assert(position.board.pieceLocations[!position.status.isWhite][KING] != EMPTY);
			position.halfMoves = 0;
		}
		if (!(move.flag & PROMOTION)) {
			position.board.pieceLocations[position.status.isWhite][move.moved_Piece] ^= moveBB;
		}

		position.status.isWhite = !position.status.isWhite;

		position.board.mergeBoth();

	}

	inline Position newPosition(Position curPos, Move move) {
		Position newPos = curPos;
		updatePosition(newPos, move);
		return newPos;
	}

	inline void makeNullMove(Position& position) {
		u64 newHash = position.hash;
		HASH::zobrist.nullMove(newHash, position.status.EP);
		position.status.isWhite = !position.status.isWhite;
		position.status.EP = no_Tile;
		position.hash = newHash;
	}

	constexpr BitBoard EnemyAndEmpty(const Board& brd, bool isWhite) {
		if (isWhite) return ~brd.occupied[WHITE];
		return ~brd.occupied[BLACK];
	}

	constexpr BitBoard Empty(const Board& brd) {
		return ~brd.occupied[BOTH];
	}

	constexpr BitBoard pawnAttackLeft(BitBoard brd, bool isWhite) {
		if (isWhite) return NWOne(brd);
		return SWOne(brd);
	}

	constexpr BitBoard pawnAttackRight(BitBoard brd, bool isWhite) {
		if (isWhite) return NEOne(brd);
		return SEOne(brd);
	}

	constexpr BitBoard getPawnAttacks(BitBoard brd, bool isWhite) {
		return (pawnAttackLeft(brd, isWhite) | pawnAttackRight(brd, isWhite));
	}

	constexpr BitBoard epRank(bool isWhite) {
		if (isWhite) return rankMask[RANK_4];
		return rankMask[RANK_5];
	}

	constexpr BitBoard getPawnEP(BitBoard brd, int EPtile, bool isWhite) {
		return getPawnAttacks(brd, isWhite) & (1ULL << EPtile) & (isWhite ? rankMask[RANK_6] : rankMask[RANK_3]);
	}

	constexpr BitBoard pawnPush(BitBoard brd, bool isWhite) {
		if (isWhite) return northOne(brd);
		return southOne(brd);
	}

	constexpr BitBoard pawnStartRank(bool isWhite) {
		if (isWhite) return rankMask[RANK_2];
		return rankMask[RANK_7];
	}

	constexpr BitBoard pawnPromRank(bool isWhite) {
		if (isWhite) return rankMask[RANK_8];
		return rankMask[RANK_1];
	}

	constexpr BitBoard getPawnMoves(int pawnPos, const Board& brd, bool isWhite) {
		BitBoard moves = EMPTY;

		moves |= pawnPush(1ULL << pawnPos, isWhite) & Empty(brd);
		moves |= ((pawnPush(moves, isWhite) & epRank(isWhite) & Empty(brd)));

		return moves;
	}

	constexpr BitBoard getKnightAttacks(BitBoard brd) {
		constexpr u64 notABfile = 18229723555195321596ULL;
		constexpr u64 notGHfile = 4557430888798830399ULL;

		BitBoard attacks = 0ULL;

		attacks |= ((brd << 6) | (brd >> 10)) & notGHfile;
		attacks |= ((brd << 15) | (brd >> 17)) & notHFile;
		attacks |= ((brd << 17) | (brd >> 15)) & notAFile;
		attacks |= ((brd << 10) | (brd >> 6)) & notABfile;

		return attacks;

	}

	constexpr BitBoard getKingAttacks(BitBoard brd) {
		BitBoard attacks = 0ULL;

		attacks |= (westOne(brd) | NWOne(brd) | SWOne(brd)) & notHFile;
		attacks |= (eastOne(brd) | NEOne(brd) | SEOne(brd)) & notAFile;
		attacks |= (northOne(brd) | southOne(brd));

		return attacks;
	}

	constexpr void bishopCheckMask(const int bishopPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan) {
		int offset = bishopPos - kingPos;
		if (!(offset % 7)) {
			if (bishopPos < kingPos) {
				// bishop is south east from the king
				kingBan |= rays[rayIndex(bishopPos, NORTH_WEST)];
				checkMask |= (rays[rayIndex(bishopPos, SOUTH_EAST)] ^ rays[rayIndex(kingPos, SOUTH_EAST)]);
			}
			else {
				// bishop is north west from the king
				kingBan |= rays[rayIndex(bishopPos, SOUTH_EAST)];
				checkMask |= (rays[rayIndex(bishopPos, NORTH_WEST)] ^ rays[rayIndex(kingPos, NORTH_WEST)]);
			}
		}
		else {
			if (bishopPos < kingPos) {
				// bishop is south west from the king
				kingBan |= rays[rayIndex(bishopPos, NORTH_EAST)];
				checkMask |= (rays[rayIndex(bishopPos, SOUTH_WEST)] ^ rays[rayIndex(kingPos, SOUTH_WEST)]);
			}
			else {
				// bishop is north east from the king
				kingBan |= rays[rayIndex(bishopPos, SOUTH_WEST)];
				checkMask |= (rays[rayIndex(bishopPos, NORTH_EAST)] ^ rays[rayIndex(kingPos, NORTH_EAST)]);
			}
		}
	}

	constexpr void rookCheckMask(const int rookPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan) {
		int offset = rookPos - kingPos;
		if (!(offset % 8)) {
			if (rookPos < kingPos) {
				// rook is south from the king
				kingBan |= rays[rayIndex(rookPos, NORTH)];
				checkMask |= (rays[rayIndex(rookPos, SOUTH)] ^ rays[rayIndex(kingPos, SOUTH)]);
			}
			else {
				// rook is north from the king
				kingBan |= rays[rayIndex(rookPos, SOUTH)];
				checkMask |= (rays[rayIndex(rookPos, NORTH)] ^ rays[rayIndex(kingPos, NORTH)]);
			}
		}
		else {
			if (rookPos < kingPos) {
				// rook is west from the king
				kingBan |= rays[rayIndex(rookPos, EAST)];
				checkMask |= (rays[rayIndex(rookPos, WEST)] ^ rays[rayIndex(kingPos, WEST)]);
			}
			else {
				// rook is east from the king
				kingBan |= rays[rayIndex(rookPos, WEST)];
				checkMask |= (rays[rayIndex(rookPos, EAST)] ^ rays[rayIndex(kingPos, EAST)]);
			}
		}
	}

	constexpr void queenCheckMask(const int queenPos, const int kingPos, BitBoard& checkMask, BitBoard& kingBan) {
		int offset = queenPos - kingPos;
		if (!(offset % 8)) {
			if (queenPos < kingPos) {
				// rook is south from the king
				kingBan |= rays[rayIndex(queenPos, NORTH)];
				checkMask |= (rays[rayIndex(queenPos, SOUTH)] ^ rays[rayIndex(kingPos, SOUTH)]);
			}
			else {
				// rook is north from the king
				kingBan |= rays[rayIndex(queenPos, SOUTH)];
				checkMask |= (rays[rayIndex(queenPos, NORTH)] ^ rays[rayIndex(kingPos, NORTH)]);
			}
		}
		else if (!(offset % 7)) {
			if (queenPos < kingPos) {
				// bishop is south east from the king
				kingBan |= rays[rayIndex(queenPos, NORTH_WEST)];
				checkMask |= (rays[rayIndex(queenPos, SOUTH_EAST)] ^ rays[rayIndex(kingPos, SOUTH_EAST)]);
			}
			else {
				// bishop is north west from the king
				kingBan |= rays[rayIndex(queenPos, SOUTH_EAST)];
				checkMask |= (rays[rayIndex(queenPos, NORTH_WEST)] ^ rays[rayIndex(kingPos, NORTH_WEST)]);
			}
		}
		else if (!(offset % 9)) {
			if (queenPos < kingPos) {
				// bishop is south west from the king
				kingBan |= rays[rayIndex(queenPos, NORTH_EAST)];
				checkMask |= (rays[rayIndex(queenPos, SOUTH_WEST)] ^ rays[rayIndex(kingPos, SOUTH_WEST)]);
			}
			else {
				// bishop is north east from the king
				kingBan |= rays[rayIndex(queenPos, SOUTH_WEST)];
				checkMask |= (rays[rayIndex(queenPos, NORTH_EAST)] ^ rays[rayIndex(kingPos, NORTH_EAST)]);
			}
		}
		else {
			if (queenPos < kingPos) {
				// rook is west from the king
				kingBan |= rays[rayIndex(queenPos, EAST)];
				checkMask |= (rays[rayIndex(queenPos, WEST)] ^ rays[rayIndex(kingPos, WEST)]);
			}
			else {
				// rook is east from the king
				kingBan |= rays[rayIndex(queenPos, WEST)];
				checkMask |= (rays[rayIndex(queenPos, EAST)] ^ rays[rayIndex(kingPos, EAST)]);
			}
		}
	}

	constexpr BitBoard highlightTilesBetween(int start, int end) {
		BitBoard b = EMPTY;
		return b |= (rays[rayIndex(start, NORTH)]      & rays[rayIndex(end, SOUTH)])
			      | (rays[rayIndex(start, EAST)]       & rays[rayIndex(end, WEST)])
			      | (rays[rayIndex(start, SOUTH)]      & rays[rayIndex(end, NORTH)])
			      | (rays[rayIndex(start, WEST)]       & rays[rayIndex(end, EAST)])
			      | (rays[rayIndex(start, NORTH_EAST)] & rays[rayIndex(end, SOUTH_WEST)])
			      | (rays[rayIndex(start, NORTH_WEST)] & rays[rayIndex(end, SOUTH_EAST)])
			      | (rays[rayIndex(start, SOUTH_WEST)] & rays[rayIndex(end, NORTH_EAST)])
			      | (rays[rayIndex(start, SOUTH_EAST)] & rays[rayIndex(end, NORTH_WEST)]);
	}

	template <Pieces pieceType>
	constexpr void addMoves(BitBoard movesBB, BitBoard capturesBB, int from, Move_List<256>& moves) {
		int to = 0;
		while (movesBB) {
			to = bitScanForward(movesBB);;
			moves.add(Move(from, to, QUIET, pieceType));
			popBit(movesBB, to);
		}
		while (capturesBB) {
			to = bitScanForward(capturesBB);
			moves.add(Move(from, to, CAPTURE, pieceType));
			popBit(capturesBB, to);
		}
	}

	constexpr void addPawnMoves(BitBoard movesBB, BitBoard captureBB, BitBoard epBB, int from, bool isWhite, Move_List<256>& moves) {
		if (epBB) {
			int to = bitScanForward(epBB);
			moves.add(Move(from, to, ENPASSANT, PAWN));
		}
		while (movesBB) {
			int to = bitScanForward(movesBB);
			if ((1ULL << to) & pawnPromRank(isWhite)) {
				moves.add(Move(from, to, KNIGHT_PROMOTION, PAWN));
				moves.add(Move(from, to, BISHOP_PROMOTION, PAWN));
				moves.add(Move(from, to, ROOK_PROMOTION, PAWN));
				moves.add(Move(from, to, QUEEN_PROMOTION, PAWN));
			}
			else moves.add(Move(from, to, QUIET, PAWN));
			popBit(movesBB, to);
		}
		while (captureBB) {
			int to = bitScanForward(captureBB);
			if ((1ULL << to) & pawnPromRank(isWhite)) {
				moves.add(Move(from, to, (CAPTURE | KNIGHT_PROMOTION), PAWN));
				moves.add(Move(from, to, (CAPTURE | BISHOP_PROMOTION), PAWN));
				moves.add(Move(from, to, (CAPTURE | ROOK_PROMOTION), PAWN));
				moves.add(Move(from, to, (CAPTURE | QUEEN_PROMOTION), PAWN));
			}
			else moves.add(Move(from, to, CAPTURE, PAWN));
			popBit(captureBB, to);
		}
	}

	bool inCheck(const Position& position) {
		bool side = position.status.isWhite;
		int kingPos = bitScanForward(position.board.pieceLocations[side][KING]);
		return (getPawnAttacks  (position.board.pieceLocations[side][KING], side) & position.board.pieceLocations[!side][PAWN])
			|| (getKnightAttacks(position.board.pieceLocations[side][KING])       & position.board.pieceLocations[!side][KNIGHT])
			|| (getBishopAttacks(position.board.occupied[BOTH], kingPos) & (position.board.pieceLocations[!side][BISHOP] | position.board.pieceLocations[!side][QUEEN]))
			|| (getRookAttacks  (position.board.occupied[BOTH], kingPos) & (position.board.pieceLocations[!side][ROOK]   | position.board.pieceLocations[!side][QUEEN]));
	}

	inline void generateMoves(bool isWhite, const Board& brd, const BoardStatus& st, Move_List<256>& moves) {

		// temporary bitboard
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
		BitBoard eKnightAtk = getKnightAttacks(brd.pieceLocations[!isWhite][KNIGHT]);

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

		BitBoard eKingAtk = getKingAttacks(brd.pieceLocations[!isWhite][KING]);

		kingBan = ePwnAtkL | ePwnAtkR | eKnightAtk | eBishopAtk | eRookAtk | eQueenAtk | eKingAtk;

		// check for checks
		BitBoard checkMask = UNIVERSE;
		// used for en passant checks
		BitBoard pawnCheckMask = UNIVERSE;

		// get position of checks made by a knight or pawn
		BitBoard checkers = (getKnightAttacks(1ULL << kingPos) & brd.pieceLocations[!isWhite][KNIGHT])
			| (getPawnAttacks(king, isWhite) & brd.pieceLocations[!isWhite][PAWN]);

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

		int numCheckers = populationCount(checkers);
		if (numCheckers) {

			checkMask = EMPTY;
			pawnCheckMask = EMPTY;

			while (checkers) {
				int checkerSquare = bitScanForward(checkers);
				popBit(checkers, checkerSquare);
				setBit(checkMask, checkerSquare);

				if (getBit(brd.pieceLocations[!isWhite][PAWN], checkerSquare)) {
					setBit(pawnCheckMask, st.EP);
				}
				else if (getBit(brd.pieceLocations[!isWhite][BISHOP], checkerSquare)) {
					bishopCheckMask(checkerSquare, kingPos, checkMask, kingBan);
				}
				else if (getBit(brd.pieceLocations[!isWhite][ROOK], checkerSquare)) {
					rookCheckMask(checkerSquare, kingPos, checkMask, kingBan);
				}
				else if (getBit(brd.pieceLocations[!isWhite][QUEEN], checkerSquare)) {
					queenCheckMask(checkerSquare, kingPos, checkMask, kingBan);
				}
			}

			if (numCheckers > 1) {
				BitBoard kingMoves = getKingAttacks(king) & EnemyAndEmpty(brd, isWhite) & ~(kingBan);
				BitBoard kingAttacks = kingMoves & brd.occupied[!isWhite];
				kingMoves ^= kingAttacks;

				// add moves to the list and return it
				addMoves<KING>(kingMoves, kingAttacks, kingPos, moves);
				return;
			}

		}

		BitBoard quietMoves;
		BitBoard attacks;
		BitBoard ep;

		while (pinned) {
			int pinnedPos = bitScanForward(pinned);
			popBit(pinned, pinnedPos);

			// get the direction of the pin
			Dir dir = getDirectionFromOffset(pinnedPos - kingPos);

			// get position of the attacking piece
			int attackingSlider = bitScanForward(rays[rayIndex(kingPos, dir)] & pinners);

			BitBoard pinnedMask = rays[rayIndex(kingPos, dir)] ^ rays[rayIndex(attackingSlider, dir)];

			BitBoard posBB = 1ULL << pinnedPos;

			if (getBit(pawns, pinnedPos)) {
				attacks = getPawnAttacks(posBB, isWhite) & brd.occupied[!isWhite] & pinnedMask & checkMask;
				quietMoves = getPawnMoves(pinnedPos, brd, isWhite) & pinnedMask & checkMask;
				ep = getPawnEP(posBB, st.EP, isWhite) & pinnedMask & (checkMask | pawnCheckMask);

				addPawnMoves(quietMoves, attacks, ep, pinnedPos, isWhite, moves);

				popBit(pawns, pinnedPos);
			}
			else if (getBit(knights, pinnedPos)) {
				popBit(knights, pinnedPos);
			}
			else if (getBit(bishops, pinnedPos)) {
				quietMoves = getBishopAttacks(brd.occupied[BOTH], pinnedPos) & EnemyAndEmpty(brd, isWhite) & pinnedMask & checkMask;
				attacks = quietMoves & brd.occupied[!isWhite];
				quietMoves ^= attacks;

				addMoves<BISHOP>(quietMoves, attacks, pinnedPos, moves);

				popBit(bishops, pinnedPos);
			}
			else if (getBit(rooks, pinnedPos)) {
				quietMoves = getRookAttacks(brd.occupied[BOTH], pinnedPos) & EnemyAndEmpty(brd, isWhite) & pinnedMask & checkMask;
				attacks = quietMoves & brd.occupied[!isWhite];
				quietMoves ^= attacks;

				addMoves<ROOK>(quietMoves, attacks, pinnedPos, moves);

				popBit(rooks, pinnedPos);
			}
			else if (getBit(queens, pinnedPos)) {
				quietMoves = (getRookAttacks(brd.occupied[BOTH], pinnedPos) | getBishopAttacks(brd.occupied[BOTH], pinnedPos)) & EnemyAndEmpty(brd, isWhite) & pinnedMask & checkMask;
				attacks = quietMoves & brd.occupied[!isWhite];
				quietMoves ^= attacks;

				addMoves<QUEEN>(quietMoves, attacks, pinnedPos, moves);

				popBit(queens, pinnedPos);
			}
		}

		// generate all other moves
		int from;

		BitBoard posBB;
		while (pawns) {
			from = bitScanForward(pawns);
			popBit(pawns, from);
			posBB = 1ULL << from;

			attacks = getPawnAttacks(posBB, isWhite) & brd.occupied[!isWhite] & checkMask;
			quietMoves = getPawnMoves(from, brd, isWhite) & checkMask;
			ep = getPawnEP(posBB, st.EP, isWhite) & (checkMask | pawnCheckMask);

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


			if (ep) {

				int to = bitScanForward(ep);

				// first we check whether the king is on the same rank as the pawn
				if (king & epRank(!isWhite)) {
					// we have to check whether there is either a rook or a queen on the same rank
					// also we have to check that the attacking piece isn't blocked either
					if ((getRookAttacks(brd.occupied[BOTH] & ~u64((posBB) | (1ULL << (isWhite ? (st.EP - 8) : (st.EP + 8)))), kingPos)
						& epRank(!isWhite))
						& (brd.pieceLocations[!isWhite][ROOK] | brd.pieceLocations[!isWhite][QUEEN])) {
						ep = EMPTY;
					}
				}
			}

			addPawnMoves(quietMoves, attacks, ep, from, isWhite, moves);
		}

		while (knights) {
			from = bitScanForward(knights);
			popBit(knights, from);

			posBB = 1ULL << from;

			quietMoves = getKnightAttacks(posBB) & EnemyAndEmpty(brd, isWhite) & checkMask;
			attacks = quietMoves & brd.occupied[!isWhite];
			quietMoves ^= attacks;

			addMoves<KNIGHT>(quietMoves, attacks, from, moves);
		}

		while (bishops) {
			from = bitScanForward(bishops);
			popBit(bishops, from);

			quietMoves = getBishopAttacks(brd.occupied[BOTH], from) & EnemyAndEmpty(brd, isWhite) & checkMask;
			attacks = quietMoves & brd.occupied[!isWhite];
			quietMoves ^= attacks;

			addMoves<BISHOP>(quietMoves, attacks, from, moves);
		}

		while (rooks) {
			from = bitScanForward(rooks);
			popBit(rooks, from);

			quietMoves = getRookAttacks(brd.occupied[BOTH], from) & EnemyAndEmpty(brd, isWhite) & checkMask;
			attacks = quietMoves & brd.occupied[!isWhite];
			quietMoves ^= attacks;

			addMoves<ROOK>(quietMoves, attacks, from, moves);
		}

		while (queens) {
			from = bitScanForward(queens);
			popBit(queens, from);

			quietMoves = (getBishopAttacks(brd.occupied[BOTH], from) | getRookAttacks(brd.occupied[BOTH], from)) & EnemyAndEmpty(brd, isWhite) & checkMask;
			attacks = quietMoves & brd.occupied[!isWhite];
			quietMoves ^= attacks;

			addMoves<QUEEN>(quietMoves, attacks, from, moves);
		}

		from = bitScanForward(king);

		quietMoves = getKingAttacks(king) & EnemyAndEmpty(brd, isWhite) & ~kingBan;
		attacks = quietMoves & brd.occupied[!isWhite];
		quietMoves ^= attacks;

		addMoves<KING>(quietMoves, attacks, from, moves);

		if (st.kingCastleRights(brd.occupied[BOTH], kingBan, brd.pieceLocations[isWhite][ROOK], isWhite)) {
			moves.add(Move(kingPos, kingPos + 2, KING_CASTLE, KING));
		}

		if (st.queenCastleRights(brd.occupied[BOTH], kingBan, brd.pieceLocations[isWhite][ROOK], isWhite)) {
			moves.add(Move(kingPos, kingPos - 2, QUEEN_CASTLE, KING));
		}

	}

	inline BitBoard generateAttacksToSquare(Position* position, int tile)
	{
		BitBoard attackers = 0ULL;
		attackers |= (getPawnAttacks((1ULL << tile), BLACK) & position->board.pieceLocations[WHITE][PAWN]);
		attackers |= (getPawnAttacks((1ULL << tile), WHITE) & position->board.pieceLocations[BLACK][PAWN]);
		attackers |= (getKnightAttacks(1ULL << tile) & (position->board.pieceLocations[WHITE][KNIGHT] | position->board.pieceLocations[BLACK][KNIGHT]));
		attackers |= (getBishopAttacks(position->board.occupied[BOTH], tile) & (position->board.pieceLocations[WHITE][BISHOP] | position->board.pieceLocations[BLACK][BISHOP] | position->board.pieceLocations[WHITE][QUEEN] | position->board.pieceLocations[BLACK][QUEEN]));
		attackers |=   (getRookAttacks(position->board.occupied[BOTH], tile) & (position->board.pieceLocations[WHITE][ROOK] | position->board.pieceLocations[BLACK][ROOK] | position->board.pieceLocations[WHITE][QUEEN] | position->board.pieceLocations[BLACK][QUEEN]));
		attackers |= (getKingAttacks((1ULL << tile)) & (position->board.pieceLocations[WHITE][KING] | position->board.pieceLocations[BLACK][KING]));
		return attackers;
	}

} // namespace KRONOS