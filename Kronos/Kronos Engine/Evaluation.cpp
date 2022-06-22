#include "Evaluation.h"

#include <algorithm>

#include "Eval_Params.h"

namespace KRONOS {
	
	namespace EVALUATION {
		
		using namespace PARAMS;

		Evaluation::Evaluation()
			: position(Position()), ally(false), enem(false) 
		{

			totalPhase = PAWN_PHASE * 16 + KNIGHT_PHASE * 4 + BISHOP_PHASE * 4 + ROOK_PHASE * 4 + QUEEN_PHASE * 2;

			memset(PST, 0, sizeof(PST));

			for (int tile = 0; tile < 64; tile++) {

				int bTile = ((7 - std::floor(tile / 8)) * 8) + (tile % 8);

				PST[WHITE][PAWN][tile] = PARAMS::pawnPST[tile];
				PST[WHITE][KNIGHT][tile] = knightPST[tile];
				PST[WHITE][BISHOP][tile] = bishopPST[tile];
				PST[WHITE][ROOK][tile] = rookPST[tile];
				PST[WHITE][QUEEN][tile] = queenPST[tile];
				PST[WHITE][KING][tile] = kingPST[tile];

				PST[BLACK][PAWN][bTile] = pawnPST[tile];
				PST[BLACK][KNIGHT][bTile] = knightPST[tile];
				PST[BLACK][BISHOP][bTile] = bishopPST[tile];
				PST[BLACK][ROOK][bTile] = rookPST[tile];
				PST[BLACK][QUEEN][bTile] = queenPST[tile];
				PST[BLACK][KING][bTile] = kingPST[tile];

			}

		}

		Evaluation::~Evaluation() {

		}

		constexpr void Evaluation::initialise(bool side) {

			phase = totalPhase;

			BitBoard kingBB = position.board.pieceLocations[side][KING];

			BitBoard doublePawnAttacks = (pawnAttackLeft(position.board.pieceLocations[side][PAWN], side) & pawnAttackRight(position.board.pieceLocations[side][PAWN], side))
				| (attacks[side][KING] & attacks[side][PAWN]);

			attacks[side][KING] = getKingAttacks(kingBB);
			attacks[side][PAWN] = getPawnAttacks(position.board.pieceLocations[side][PAWN], side);
			attacks[side][6] = attacks[side][PAWN] | attacks[side][KING];
			attackedBy2[side] = doublePawnAttacks;

			// by default the king's position, however, if on the outer edges, it is pushed out
			int kingPos = bitScanForward(kingBB);;
			BitBoard kingRingOrigin = (std::clamp(getFileMask(kingPos), fileMask[B], fileMask[G])
				& std::clamp(getRankMask(kingPos), rankMask[RANK_2], rankMask[RANK_7]));
			kingRing[side] = getKingAttacks(kingRingOrigin) | kingRingOrigin;

			kingAttackersCount[!side] = populationCount(kingRing[side] & getPawnAttacks(position.board.pieceLocations[!side][PAWN], !side));
			kingAttackersWeight[!side] = kingAttacksCount[!side] = 0;

			kingRing[side] &= ~doublePawnAttacks;
		}

		constexpr Score Evaluation::countMaterial(bool side) {

			Score score;
			score += PAWN_VALUE * populationCount(position.board.pieceLocations[side][PAWN]);

			int pieceCount = populationCount(position.board.pieceLocations[side][KNIGHT]);
			score += KNIGHT_VALUE * pieceCount;
			phase -= pieceCount * KNIGHT_PHASE;

			pieceCount = populationCount(position.board.pieceLocations[side][BISHOP]);
			score += BISHOP_VALUE * pieceCount;
			phase -= pieceCount * BISHOP_PHASE;

			pieceCount = populationCount(position.board.pieceLocations[side][ROOK]);
			score += ROOK_VALUE * pieceCount;
			phase -= pieceCount * ROOK_PHASE;

			pieceCount = populationCount(position.board.pieceLocations[side][QUEEN]);
			score += QUEEN_VALUE * pieceCount;
			phase -= pieceCount * QUEEN_PHASE;


			return score;
		}

		template <Pieces pieceType>
		constexpr Score Evaluation::evaluatePiece(bool side) {

			BitBoard pBB = position.board.pieceLocations[side][pieceType];
			BitBoard atks;
			Score score = SCORE_ZERO;

			attacks[side][pieceType] = 0ULL;

			while (pBB) {

				int tile = bitScanForward(pBB);
				popBit(pBB, tile);

				score += (PST[side][pieceType][tile]);

				if constexpr (pieceType == KNIGHT) {
					atks = getKnightAttacks(1ULL << tile);
				}
				else if constexpr (pieceType == BISHOP) {
					atks = getBishopAttacks(position.board.occupied[BOTH], tile);
				}
				else if constexpr (pieceType == ROOK) {
					atks = getRookAttacks(position.board.occupied[BOTH], tile);
				}
				else if constexpr (pieceType == QUEEN) {
					atks = getBishopAttacks(position.board.occupied[BOTH], tile) | getRookAttacks(position.board.occupied[BOTH], tile);
				}

				attackedBy2[side] |= attacks[side][6] & atks;
				attacks[side][pieceType] |= atks;
				attacks[side][6] |= atks;

				if (atks & kingRing[!side]) {
					kingAttackersCount[side]++;
					kingAttackersWeight[side] += kingAttackWeights[pieceType];
					kingAttackersCount[side] += populationCount(atks & attacks[!side][KING]);
				}
				else if (pieceType == ROOK && (getFileMask(tile) & kingRing[!side])) {
					score += ROOK_ON_KING_RING;
				}
				else if (pieceType == BISHOP && (getBishopAttacks(position.board.occupied[BOTH] ^ position.board.pieceLocations[!side][PAWN], tile) & kingRing[!side])) {
					score += BISHOP_ON_KING_RING;
				}

				int mobility = populationCount(atks & mobilityMask);
				score += getMobilityScore<pieceType>(mobility);

			}

			return score;

		}

		constexpr Score Evaluation::pawnStructure(bool side) {

			Score score;

			BitBoard allyPawns = position.board.pieceLocations[side][PAWN];

			while (allyPawns) {
				int tile = bitScanForward(allyPawns);
				popBit(allyPawns, tile);

				score += (PST[side][PAWN][tile]);
			}

			allyPawns = position.board.pieceLocations[side][PAWN];
			BitBoard enemyPawns = position.board.pieceLocations[!side][PAWN];

			BitBoard aAtk = getPawnAttacks(allyPawns, side);

			BitBoard passedPawns = allyPawns & ~(allyPawns & pawnFill(enemyPawns, !side));
			BitBoard connectedPawns = allyPawns & aAtk;
			BitBoard halfConnectedPawns = allyPawns & pawnPush(aAtk, !side);
			BitBoard doubledPawns = allyPawns & (pawnFillEx(allyPawns, !side));

			BitBoard noWestNeighbour = allyPawns & ~(westOne(fileFill(allyPawns)));
			BitBoard noEastNeighbour = allyPawns & ~(eastOne(fileFill(allyPawns)));
			BitBoard isolatedPawns = noWestNeighbour & noEastNeighbour;
			BitBoard halfIsolatedPawns = noWestNeighbour ^ noEastNeighbour;

			BitBoard backwardPawns = (allyPawns & ~isolatedPawns) & (pawnPush(allyPawns, side) & attacks[!side][6])
				& ~pawnFillEx(getPawnAttacks(allyPawns, !side), side);


			score += CONNECTED_PAWN_VALUE * populationCount(connectedPawns);
			score += HALF_CONNECTED_PAWN_VALUE * populationCount(halfConnectedPawns);
			score += DOUBLED_PAWN_VALUE * populationCount(doubledPawns);
			score += ISOLATED_PAWN_VALUE * populationCount(isolatedPawns & ~passedPawns);
			score += HALF_ISOLATED_PAWN_VALUE * populationCount(halfIsolatedPawns & ~passedPawns);
			score += BACKWARD_PAWN_VALUE * populationCount(backwardPawns & ~passedPawns);
			score += PASSED_ISOLATED_PAWN_VALUE * populationCount(passedPawns & isolatedPawns);
			score += PASSED_BACKWARD_PAWN_VALUE * populationCount(passedPawns & backwardPawns);

			return score;
		}

		inline Score Evaluation::kingSafety(bool side) {

			// temp
			BitBoard b0, b1, b2;

			const BitBoard Camp = (side == WHITE ? (UNIVERSE ^ rankMask[RANK_6] ^ rankMask[RANK_7] ^ rankMask[RANK_8])
				: (UNIVERSE ^ rankMask[RANK_1] ^ rankMask[RANK_2] ^ rankMask[RANK_3]));

			BitBoard weakTiles, safeTiles, unsafeChecks = 0ULL;
			BitBoard rookChecks, bishopChecks, queenChecks, knightChecks;
			basic_score kingDanger = 0;
			int kingPos = bitScanForward(position.board.pieceLocations[side][KING]);

			Score score = SCORE_ZERO;

			score += PST[side][KING][kingPos];

			// attacked squares defended by at most one queen or king
			weakTiles = attacks[!side][6]
				& ~attackedBy2[side]
				& (~attacks[side][6] | attacks[side][KING] | attacks[side][QUEEN]);

			// find safe enemy checks
			safeTiles = ~position.board.occupied[side];
			safeTiles &= ~attacks[side][6] | (weakTiles & attackedBy2[!side]);

			b0 = getRookAttacks(position.board.occupied[BOTH] ^ position.board.pieceLocations[side][QUEEN], kingPos);
			b1 = getBishopAttacks(position.board.occupied[BOTH] ^ position.board.pieceLocations[side][QUEEN], kingPos);

			// find enemy rook checks
			rookChecks = b0 & attacks[!side][ROOK] & safeTiles;
			if (rookChecks) {
				kingDanger += SAFE_CHECK_SCORE[ROOK][populationCount(rookChecks) > 1 ? 1 : 0];
			}
			else {
				unsafeChecks |= b0 & attacks[!side][ROOK];
			}

			queenChecks = (b0 | b1) & attacks[!side][QUEEN] & safeTiles & ~(attacks[side][QUEEN] | rookChecks);
			if (queenChecks) {
				kingDanger += SAFE_CHECK_SCORE[QUEEN][populationCount(queenChecks) > 1 ? 1 : 0];;
			}

			bishopChecks = b1 & attacks[!side][BISHOP] & safeTiles & ~queenChecks;
			if (bishopChecks) {
				kingDanger += SAFE_CHECK_SCORE[BISHOP][populationCount(bishopChecks) > 1 ? 1 : 0];;
			}
			else {
				unsafeChecks |= b1 & attacks[!side][BISHOP];
			}

			knightChecks = getKnightAttacks(position.board.pieceLocations[side][KING]) & attacks[!side][KNIGHT];
			if (knightChecks & safeTiles) {
				kingDanger += SAFE_CHECK_SCORE[KNIGHT][populationCount(knightChecks) > 1 ? 1 : 0];;
			}
			else {
				unsafeChecks |= knightChecks;
			}

			// find tiles that the opponent can attack our king from the flank, tiles that they attack twice in that flank, and tiles we defend
			BitBoard kingFlank = (eastOne(westOne(pawnPush(position.board.pieceLocations[side][KING], !side))));
			b0 = attacks[!side][6] & kingFlank & Camp;
			b1 = b0 & attackedBy2[!side];
			b2 = attacks[side][6] & kingFlank;

			int kingFlankAttack = populationCount(b0) + populationCount(b1);
			int kingFlankDefence = populationCount(b2);

			kingDanger += kingAttackersCount[!side] * kingAttackersWeight[!side]
				+ 183 * populationCount(kingRing[side] & weakTiles)
				+ 148 * populationCount(unsafeChecks)
				//+  98 * populationCount()
				+ 69 * kingAttacksCount[!side]
				+ 3 * kingFlankAttack * kingFlankAttack / 8
				- 100 * (attacks[side][KNIGHT] & attacks[side][KING] ? 1 : 0)
				- 4 * kingFlankDefence
				+ 37;

			if (kingDanger > 75)
				score -= Score(kingDanger * kingDanger / 4096, kingDanger / 16);

			if (!(position.board.pieceLocations[side][PAWN] & kingFlank))
				score -= PAWNLESS_FLANK;

			score -= FLANK_ATTACKS * kingFlankAttack;

			return score;


		}

		constexpr Score Evaluation::threats(bool side) {
			Score score = SCORE_ZERO;
			BitBoard minors = position.board.pieceLocations[!side][KNIGHT] | position.board.pieceLocations[!side][BISHOP];
			BitBoard weakTiles = (attacks[side][6] & ~attacks[!side][6]) | (attackedBy2[side] & ~attackedBy2[!side] & ~attacks[!side][PAWN]);
			BitBoard safePush = ~attacks[!side][PAWN] & ~position.board.occupied[BOTH];
			BitBoard pushTarget = getPawnAttacks(position.board.occupied[!side] & ~position.board.pieceLocations[!side][PAWN], !side) & (attacks[side][6] | ~attacks[!side][6]);
			BitBoard push = pawnPush(position.board.pieceLocations[side][PAWN], side) & safePush;
			push |= pawnPush(push, side) & epRank(side) & safePush;
			score += THREAT_PAWN_PUSH_VALUE * populationCount(push & pushTarget);
			score += THREAT_WEAK_PAWNS_VALUE * populationCount(position.board.pieceLocations[!side][PAWN] & weakTiles);
			score += THREAT_PAWNSxMINORS_VALUE * populationCount(attacks[side][PAWN] & minors);
			score += THREAT_MINORSxMINORS_VALUE * populationCount((attacks[side][KNIGHT] | attacks[side][BISHOP]) & minors);
			score += THREAT_MAJORSxWEAK_MINORS_VALUE * populationCount((attacks[side][ROOK] | attacks[side][QUEEN]) & minors & weakTiles);
			score += THREAT_PAWN_MINORSxMAJORS_VALUE * populationCount((attacks[side][PAWN] | attacks[side][KNIGHT] | attacks[side][BISHOP]) & (position.board.pieceLocations[!side][ROOK] | position.board.pieceLocations[!side][QUEEN]));
			score += THREAT_ALLxQUEENS_VALUE * populationCount(attacks[side][6] & position.board.pieceLocations[!side][QUEEN]);
			score += THREAT_KINGxMINORS_VALUE * populationCount(getKingAttacks(position.board.pieceLocations[side][KING]) & minors & weakTiles);
			score += THREAT_KINGxROOKS_VALUE * populationCount(getKingAttacks(position.board.pieceLocations[side][KING]) & position.board.pieceLocations[!side][ROOK] & weakTiles);
			return score;
		}

		constexpr Score Evaluation::control(bool side) {
			Score score = SCORE_ZERO;
			BitBoard controlledTiles = attackedBy2[side] & attacks[!side][6] & ~attackedBy2[!side] & ~attacks[!side][PAWN];
			score += PIECE_SPACE * populationCount(controlledTiles & position.board.occupied[BOTH]);
			score += EMPTY_SPACE * populationCount(controlledTiles & ~position.board.occupied[BOTH]);
			return score;
		}

		int Evaluation::evaluate(const Position& position) {
			this->position = position;
			ally = position.status.isWhite;
			enem = !ally;


			Score eval;

			initialise(ally); initialise(enem);

			eval += countMaterial(ally) - countMaterial(enem);

			mobilityMask = ~attacks[enem][PAWN] & EnemyAndEmpty(position.board, ally);
			eval += evaluatePiece<KNIGHT>(ally);
			eval += evaluatePiece<BISHOP>(ally);

			mobilityMask = ~attacks[ally][PAWN] & EnemyAndEmpty(position.board, enem);
			eval -= evaluatePiece<KNIGHT>(enem);
			eval -= evaluatePiece<BISHOP>(enem);

			mobilityMask = ~attacks[enem][PAWN] | ~attacks[enem][BISHOP] | ~attacks[enem][KNIGHT] & EnemyAndEmpty(position.board, ally);
			eval += evaluatePiece<ROOK>(ally);

			mobilityMask = ~attacks[ally][PAWN] | ~attacks[ally][BISHOP] | ~attacks[ally][KNIGHT] & EnemyAndEmpty(position.board, enem);
			eval -= evaluatePiece<ROOK>(enem);

			mobilityMask = ~attacks[enem][PAWN] | ~attacks[enem][ROOK] | ~attacks[enem][KNIGHT] | ~attacks[enem][BISHOP] & EnemyAndEmpty(position.board, ally);
			eval += evaluatePiece<QUEEN>(ally);

			mobilityMask = ~attacks[ally][PAWN] | ~attacks[ally][ROOK] | ~attacks[ally][KNIGHT] | ~attacks[ally][BISHOP] & EnemyAndEmpty(position.board, enem);
			eval -= evaluatePiece<QUEEN>(enem);

			eval += pawnStructure(ally) - pawnStructure(enem);

			eval += kingSafety(ally) - kingSafety(enem);

			eval += threats(ally) - threats(enem);

			eval += control(ally) - control(enem);

			phase = (phase * 256 + (totalPhase / 2)) / totalPhase;
			return ((eval.middleGame * (256 - phase)) + (eval.endGame * phase)) / 256;
		}

		int Evaluation::tracedEval(const Position& position) {
			this->position = position;
			ally = position.status.isWhite;
			enem = !ally;


			Score eval = SCORE_ZERO;

			initialise(ally); initialise(enem);

			Score aMaterial = countMaterial(ally);
			Score eMaterial = countMaterial(enem);

			mobilityMask = ~attacks[enem][PAWN] & EnemyAndEmpty(position.board, ally);
			Score aKnight = evaluatePiece<KNIGHT>(ally);
			Score aBishop = evaluatePiece<BISHOP>(ally);

			mobilityMask = ~attacks[ally][PAWN] & EnemyAndEmpty(position.board, enem);
			Score eKnight = evaluatePiece<KNIGHT>(enem);
			Score eBishop = evaluatePiece<BISHOP>(enem);

			mobilityMask = ~attacks[enem][PAWN] | ~attacks[enem][BISHOP] | ~attacks[enem][KNIGHT] & EnemyAndEmpty(position.board, ally);
			Score aRook = evaluatePiece<ROOK>(ally);

			mobilityMask = ~attacks[ally][PAWN] | ~attacks[ally][BISHOP] | ~attacks[ally][KNIGHT] & EnemyAndEmpty(position.board, enem);
			Score eRook = evaluatePiece<ROOK>(enem);

			mobilityMask = ~attacks[enem][PAWN] | ~attacks[enem][ROOK] | ~attacks[enem][KNIGHT] | ~attacks[enem][BISHOP] & EnemyAndEmpty(position.board, ally);
			Score aQueen = evaluatePiece<QUEEN>(ally);

			mobilityMask = ~attacks[ally][PAWN] | ~attacks[ally][ROOK] | ~attacks[ally][KNIGHT] | ~attacks[ally][BISHOP] & EnemyAndEmpty(position.board, enem);
			Score eQueen = evaluatePiece<QUEEN>(enem);

			Score aPawn = pawnStructure(ally);
			Score ePawn = pawnStructure(enem);

			Score aKing = kingSafety(ally);
			Score eKing = kingSafety(enem);

			Score aThreats = threats(ally);
			Score eThreats = threats(enem);

			Score aControl = control(ally);
			Score eControl = control(enem);

			eval += (aMaterial - eMaterial);
			eval += (aKnight - eKnight);
			eval += (aBishop - eBishop);
			eval += (aRook - eRook);
			eval += (aQueen - eQueen);
			eval += (aPawn - ePawn);
			eval += (aKing - eKing);
			eval += (aThreats - eThreats);
			eval += (aControl - eControl);

			std::cout << "Ally Material: " << aMaterial.middleGame << " " << aMaterial.endGame << std::endl;
			std::cout << "Enemy Material: " << eMaterial.middleGame << " " << eMaterial.endGame << std::endl << std::endl;

			std::cout << "Ally Knight: " << aKnight.middleGame << " " << aKnight.endGame << std::endl;
			std::cout << "Enemy Knight: " << eKnight.middleGame << " " << eKnight.endGame << std::endl << std::endl;

			std::cout << "Ally Bishop: " << aBishop.middleGame << " " << aBishop.endGame << std::endl;
			std::cout << "Enemy Bishop: " << eBishop.middleGame << " " << eBishop.endGame << std::endl << std::endl;

			std::cout << "Ally Rook: " << aRook.middleGame << " " << aRook.endGame << std::endl;
			std::cout << "Enemy Rook: " << eRook.middleGame << " " << eRook.endGame << std::endl << std::endl;

			std::cout << "Ally Queen: " << aQueen.middleGame << " " << aQueen.endGame << std::endl;
			std::cout << "Enemy Queen: " << eQueen.middleGame << " " << eQueen.endGame << std::endl << std::endl;

			std::cout << "Ally Pawn: " << aPawn.middleGame << " " << aPawn.endGame << std::endl;
			std::cout << "Enemy Pawn: " << ePawn.middleGame << " " << ePawn.endGame << std::endl << std::endl;

			std::cout << "Ally King: " << aKing.middleGame << " " << aKing.endGame << std::endl;
			std::cout << "Enemy King: " << eKing.middleGame << " " << eKing.endGame << std::endl << std::endl;

			std::cout << "Ally Threats: " << aThreats.middleGame << " " << aThreats.endGame << std::endl;
			std::cout << "Enemy Threats: " << eThreats.middleGame << " " << eThreats.endGame << std::endl << std::endl;

			std::cout << "Ally Control: " << aControl.middleGame << " " << aControl.endGame << std::endl;
			std::cout << "Enemy Control: " << eControl.middleGame << " " << eControl.endGame << std::endl << std::endl;

			phase = (phase * 256 + (totalPhase / 2)) / totalPhase;
			int score = ((eval.middleGame * (256 - phase)) + (eval.endGame * phase)) / 256;
			std::cout << "Evaluation: " << eval.middleGame << " " << eval.middleGame << " " << score << std::endl;

			return score;
		}
		
	} // namespace EVALUATION
	
} // namespace KRONOS