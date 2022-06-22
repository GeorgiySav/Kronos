#include "Move_Generation.h"

#include "Rays.h"

namespace KRONOS
{

	inline void updatePosition(Position& position, Move move) {

		position.status.EP = no_Tile;

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
		}
		if (!(move.flag & PROMOTION)) {
			position.board.pieceLocations[position.status.isWhite][move.moved_Piece] ^= moveBB;
		}

		position.status.isWhite = !position.status.isWhite;
		position.halfMoves++;
		if (position.status.isWhite)
			position.fullMoves++;

		position.board.mergeBoth();

	}

	inline Position newPosition(Position curPos, Move move) {
		Position newPos = curPos;
		updatePosition(newPos, move);
		return newPos;
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
	constexpr void addMoves(BitBoard movesBB, BitBoard capturesBB, int from, std::vector<Move>* moves) {
		int to = 0;
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

	constexpr void addPawnMoves(BitBoard movesBB, BitBoard captureBB, BitBoard epBB, int from, bool isWhite, std::vector<Move>* moves) {
		if (epBB) {
			int to = bitScanForward(epBB);
			moves->push_back(Move(from, to, ENPASSANT, PAWN));
		}
		while (movesBB) {
			int to = bitScanForward(movesBB);
			if ((1ULL << to) & pawnPromRank(isWhite)) {
				moves->push_back(Move(from, to, KNIGHT_PROMOTION, PAWN));
				moves->push_back(Move(from, to, BISHOP_PROMOTION, PAWN));
				moves->push_back(Move(from, to, ROOK_PROMOTION, PAWN));
				moves->push_back(Move(from, to, QUEEN_PROMOTION, PAWN));
			}
			else moves->push_back(Move(from, to, QUIET, PAWN));
			popBit(movesBB, to);
		}
		while (captureBB) {
			int to = bitScanForward(captureBB);
			if ((1ULL << to) & pawnPromRank(isWhite)) {
				moves->push_back(Move(from, to, (CAPTURE | KNIGHT_PROMOTION), PAWN));
				moves->push_back(Move(from, to, (CAPTURE | BISHOP_PROMOTION), PAWN));
				moves->push_back(Move(from, to, (CAPTURE | ROOK_PROMOTION), PAWN));
				moves->push_back(Move(from, to, (CAPTURE | QUEEN_PROMOTION), PAWN));
			}
			else moves->push_back(Move(from, to, CAPTURE, PAWN));
			popBit(captureBB, to);
		}
	}

	constexpr bool inCheck(Position& position) {
		return (getPawnAttacks(position.board.pieceLocations[position.status.isWhite][KING], !position.status.isWhite) & position.board.pieceLocations[!position.status.isWhite][PAWN])
			|| (getKnightAttacks(position.board.pieceLocations[position.status.isWhite][KING]) & position.board.pieceLocations[!position.status.isWhite][KNIGHT])
			|| (getRookAttacks(position.board.occupied[BOTH], bitScanForward(position.board.pieceLocations[position.status.isWhite][KING])) & (position.board.pieceLocations[!position.status.isWhite][ROOK] | position.board.pieceLocations[!position.status.isWhite][QUEEN]))
			|| (getBishopAttacks(position.board.occupied[BOTH], bitScanForward(position.board.pieceLocations[position.status.isWhite][KING])) & (position.board.pieceLocations[!position.status.isWhite][BISHOP] | position.board.pieceLocations[!position.status.isWhite][QUEEN]));
	}

	inline void generateMoves(bool isWhite, Board& brd, BoardStatus& st, std::vector<Move>* moves) {

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
			moves->push_back(Move(kingPos, kingPos + 2, KING_CASTLE, KING));
		}

		if (st.queenCastleRights(brd.occupied[BOTH], kingBan, brd.pieceLocations[isWhite][ROOK], isWhite)) {
			moves->push_back(Move(kingPos, kingPos - 2, QUEEN_CASTLE, KING));
		}

	}

} // namespace KRONOS