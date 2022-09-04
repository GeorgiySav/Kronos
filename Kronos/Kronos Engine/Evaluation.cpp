#include "Evaluation.h"

#include <algorithm>

#include "Eval_Params.h"

namespace KRONOS {
	
	namespace EVALUATION {

		const u64 COLOURED_SQUARE_BB[2] = {
			12273903644374837845ULL,
		   ~12273903644374837845ULL
		}; 

		Evaluation::Evaluation()
			: position(Position()), ally(false), enem(false) 
		{

			totalPhase = PARAM.QUEEN_PHASE * 2 + PARAM.ROOK_PHASE * 4 + PARAM.BISHOP_PHASE * 4 + PARAM.KNIGHT_PHASE * 4;

		}

		Evaluation::~Evaluation() {

		}

		constexpr void Evaluation::initialise(bool side) {

			phase = 0;

			BitBoard kingBB = position.board.pieceLocations[side][KING];

			BitBoard doublePawnAttacks = (pawnAttackLeft(position.board.pieceLocations[side][PAWN], side) & pawnAttackRight(position.board.pieceLocations[side][PAWN], side))
				| (attacks[side][KING] & attacks[side][PAWN]);

			attacks[side][KING] = getKingAttacks(kingBB);
			attacks[side][PAWN] = getPawnAttacks(position.board.pieceLocations[side][PAWN], side);
			attacks[side][6] = attacks[side][PAWN] | attacks[side][KING];
			attackedBy2[side] = doublePawnAttacks;

			atksOnKingScore[side] = 0;
			kingRing[side] = KING_RING[side][bitScanForward(kingBB)];
		}

		constexpr Score Evaluation::countMaterial(bool side) {

			Score score = SCORE_ZERO;
			score += PARAM.PAWN_VALUE * populationCount(position.board.pieceLocations[side][PAWN]);

			int pieceCount = populationCount(position.board.pieceLocations[side][KNIGHT]);
			score += PARAM.KNIGHT_VALUE * pieceCount;
			pieceCountImb[side][KNIGHT] = pieceCount;
			phase += pieceCount * PARAM.KNIGHT_PHASE;

			pieceCount = populationCount(position.board.pieceLocations[side][BISHOP]);
			score += PARAM.BISHOP_VALUE * pieceCount;
			pieceCountImb[side][BISHOP] = pieceCount;
			pieceCountImb[side][0] = pieceCount > 1;
			phase += pieceCount * PARAM.BISHOP_PHASE;

			pieceCount = populationCount(position.board.pieceLocations[side][ROOK]);
			score += PARAM.ROOK_VALUE * pieceCount;
			pieceCountImb[side][ROOK] = pieceCount;
			phase += pieceCount * PARAM.ROOK_PHASE;

			pieceCount = populationCount(position.board.pieceLocations[side][QUEEN]);
			score += PARAM.QUEEN_VALUE * pieceCount;
			pieceCountImb[side][QUEEN] = pieceCount;
			phase += pieceCount * PARAM.QUEEN_PHASE;


			return score;
		}

		template <Pieces pieceType>
		constexpr Score Evaluation::evaluatePiece(bool side) {
			constexpr BitBoard centerTiles = (1ULL << D4) | (1ULL << D5) | (1ULL << E4) | (1ULL << E5);

			BitBoard pBB = position.board.pieceLocations[side][pieceType];
			BitBoard atks = EMPTY;
			Score score = SCORE_ZERO;

			attacks[side][pieceType] = 0ULL;

			while (pBB) {

				int tile = bitScanForward(pBB);
				popBit(pBB, tile);

				score += (PARAM.PST[side][pieceType][tile]);

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
					atksOnKingScore[side] += PARAM.ATK_ON_KING_WEIGHT[pieceType] * populationCount(atks & kingRing[!side]);
				}

				int mobility = populationCount(atks & mobilityMask);
				score += PARAM.MOBILITY_BONUS[pieceType][mobility];

				if constexpr (pieceType == KNIGHT)
				{
					if ((1ULL << tile) & outpostTiles)
						score += PARAM.KNIGHT_OUTPOST_BONUS;
					else if (atks & outpostTiles & ~position.board.occupied[side])
						score += PARAM.KNIGHT_X_OUTPOST_BONUS;				
				}
				if constexpr (pieceType == BISHOP)
				{
					if ((1ULL << tile) & outpostTiles)
						score += PARAM.BISHOP_OUTPOST_BONUS;

					if (populationCount(atks & centerTiles) >= 2) score += PARAM.BISHOP_CENTER_CONTROL;
					// punish the bishop if too many pawns are on its coloured square
					int colouredTile = ((int)std::floor(tile / 8) % 2) == ((tile % 8) % 2);
					score -= PARAM.BISHOP_PAWN_PENALTY * populationCount(COLOURED_SQUARE_BB[colouredTile] & position.board.pieceLocations[side][PAWN]);		
				}
				else if constexpr (pieceType == ROOK)
				{
					if ((1ULL << tile) & ((side == WHITE) ? (rankMask[RANK_7]) : rankMask[RANK_2]))
						if (position.board.pieceLocations[!side][KING] & ((side == WHITE) ? (rankMask[RANK_7] | rankMask[RANK_8]) : (rankMask[RANK_1] | rankMask[RANK_2])))
							score += PARAM.ROOK_ON_7TH;

					if (!(fileMask[tile % 8] & position.board.pieceLocations[side][PAWN])) {
						if (!(fileMask[tile % 8] & position.board.pieceLocations[!side][PAWN])) 
							score += PARAM.ROOK_OPEN_FILE_BONUS;
						else 
							score += PARAM.ROOK_SEMI_OPEN_FILE_BONUS;	
					}
				}

			}

			return score;
		}

		constexpr Score Evaluation::pawnStructure(bool side) {

			Score score;

			BitBoard allyPawns = position.board.pieceLocations[side][PAWN];

			while (allyPawns) {
				int tile = bitScanForward(allyPawns);
				popBit(allyPawns, tile);

				score += (PARAM.PST[side][PAWN][tile]);
			}

			allyPawns = position.board.pieceLocations[side][PAWN];
			BitBoard enemyPawns = position.board.pieceLocations[!side][PAWN];

			BitBoard aAtk = getPawnAttacks(allyPawns, side);

			BitBoard passedPawns = allyPawns & ~(allyPawns & pawnFill(enemyPawns, !side));
			BitBoard connectedPawns = allyPawns & (aAtk | pawnPush(aAtk, !side));
			BitBoard doubledPawns = allyPawns & (pawnFillEx(allyPawns, !side));
			BitBoard isolatedPawns = allyPawns & ~fileFill(aAtk);
			BitBoard backwardPawns = allyPawns & ~isolatedPawns & pawnPush(getPawnAttacks(enemyPawns, !side) | enemyPawns, !side) & ~pawnFill(aAtk, side);

			score += PARAM.CONNECTED_PAWN_VALUE * populationCount(connectedPawns);
			score += PARAM.DOUBLED_PAWN_VALUE * populationCount(doubledPawns);
			score += PARAM.ISOLATED_PAWN_VALUE * populationCount(isolatedPawns & ~passedPawns);
			score += PARAM.BACKWARD_PAWN_VALUE * populationCount(backwardPawns & ~passedPawns);
			score += PARAM.PASSED_ISOLATED_PAWN_VALUE * populationCount(passedPawns & isolatedPawns);
			score += PARAM.PASSED_BACKWARD_PAWN_VALUE * populationCount(passedPawns & backwardPawns);

			return score;
		}

		//Evaluates the pawn storm in the enemy king and their pawn shelter
		constexpr basic_score Evaluation::pawnStorm(bool side, int tile) 
		{		
			basic_score shelter = 0;
			int file = FILE_TO_SHELTER[tile % 8];

			BitBoard kingFileMask = fileMask[tile % 8];

			BitBoard kingShelter = position.board.pieceLocations[!side][PAWN];
			BitBoard kingStorm = position.board.pieceLocations[side][PAWN];

			BitBoard shelterMask1 = KING_SHELTER_MASK_1[!side][file];
			BitBoard shelterMask2 = KING_SHELTER_MASK_2[!side][file];
			BitBoard shelterMask3 = KING_SHELTER_MASK_3[!side][file];

			shelter += PARAM.KING_SHELTER_1 * populationCount(kingShelter & shelterMask1 & ~kingFileMask);
			shelter += PARAM.KING_SHELTER_F1 * populationCount(kingShelter & shelterMask1 & kingFileMask);
			shelter += PARAM.KING_SHELTER_2 * populationCount(kingShelter & shelterMask2 & ~kingFileMask);
			shelter += PARAM.KING_SHELTER_F2 * populationCount(kingShelter & shelterMask2 & kingFileMask);
			shelter += PARAM.KingStorm1 * populationCount(kingStorm & shelterMask2);
			shelter += PARAM.KingStorm2 * populationCount(kingStorm & shelterMask3);

			return shelter;
		}
		
		// evaluates the ally's pressure on the enemy king 
		inline Score Evaluation::kingPressure(bool side) {

			int kingPos = bitScanForward(position.board.pieceLocations[!side][KING]);
			basic_score pStorm = pawnStorm(side, kingPos);
			if (side == WHITE) {
				if (position.status.BKcastle)
					pStorm = std::max(pStorm, pawnStorm(side, G8));
				else if (position.status.BQcastle)
					pStorm = std::max(pStorm, pawnStorm(side, B8));
			}
			else {
				if (position.status.WKcastle)
					pStorm = std::max(pStorm, pawnStorm(side, G1));
				else if (position.status.WQcastle)
					pStorm = std::max(pStorm, pawnStorm(side, B8));
			}

			Score bonus = Score(pStorm, 0);

			if (atksOnKingScore[side] > 0) {

				BitBoard kingAtks = getKingAttacks(position.board.pieceLocations[!side][KING]);
				// weak tiles are tiles that are attacked by the ally, and that isn't defended by enemy pieces
				BitBoard weakTiles = attacks[side][6] & ~attackedBy2[!side] & (~attacks[!side][6] | kingAtks);
				// safe tiles are tiles that aren't occupied by our pieces, aren't attacked by their pieces and are attacked twice by us
				// essentially that are tiles that are safe for us to move into
				BitBoard safeTiles = ~position.board.occupied[side] & (~attacks[!side][6] | (weakTiles & attackedBy2[side]));

				BitBoard knightThreats = getKnightAttacks(position.board.pieceLocations[!side][KING]);
				BitBoard bishopThreats = getBishopAttacks(position.board.occupied[BOTH], kingPos);
				BitBoard rookThreats = getRookAttacks(position.board.occupied[BOTH], kingPos);
				BitBoard queenThreats = bishopThreats | rookThreats;

				basic_score score = 0;
				score += atksOnKingScore[side];
				score += PARAM.WEAK_TILE * populationCount(weakTiles & kingAtks);
				score += PARAM.ENEMY_PAWN * populationCount(position.board.pieceLocations[!side][PAWN] & kingAtks & ~weakTiles);
				score += PARAM.QUEEN_SAFE_CHECK * populationCount(queenThreats & attacks[side][QUEEN] & safeTiles);
				score += PARAM.ROOK_SAFE_CHECK * populationCount(rookThreats & attacks[side][ROOK] & safeTiles);
				score += PARAM.BISHOP_SAFE_CHECK * populationCount(bishopThreats & attacks[side][BISHOP] & safeTiles);
				score += PARAM.KNIGHT_SAFE_CHECK * populationCount(knightThreats & attacks[side][KNIGHT] & safeTiles);

				if (score > 0) return bonus + Score(score * score / 1024, score / 20);

			}
			return bonus;
		}

		constexpr Score Evaluation::passedPawns(bool side) {
			BitBoard passers = position.board.pieceLocations[side][PAWN] & ~pawnFillEx(position.board.pieceLocations[side][PAWN], !side) & ~pawnFill(attacks[!side][PAWN], !side);
			if (passers == EMPTY)
				return SCORE_ZERO;

			Score score = SCORE_ZERO;
			const BitBoard notBlocked = ~pawnPush(position.board.occupied[BOTH], !side);
			const BitBoard safePush = ~pawnPush(attacks[!side][6], !side);
			const BitBoard safeProm = ~pawnFillEx(attacks[!side][6] | position.board.occupied[!side], !side);
			
			int kingPos = bitScanForward(position.board.pieceLocations[side][KING]);
			int enemKingPos = bitScanForward(position.board.pieceLocations[!side][KING]);
			while (passers) {
				int tile = bitScanForward(passers);
				popBit(passers, tile);

				int rank = side == WHITE ? tile / 8 : 7 - tile / 8;
				score += PARAM.PASSER_DIST_ALLY[rank] * std::max(std::abs(int(kingPos / 8) - int(tile / 8)), std::abs((kingPos % 8) - (tile % 8)));
				score += PARAM.PASSER_DIST_ENEMY[rank] * std::max(std::abs(int(enemKingPos / 8) - int(tile / 8)), std::abs((enemKingPos % 8) - (tile % 8)));
				score += PARAM.PASSER_BONUS[rank];
				
				if ((1ull << tile) & notBlocked)
					score += PARAM.UNBLOCKED_PASSER[rank];
				if ((1ull << tile) & safePush)
					score += PARAM.PASSER_SAFE_PUSH[rank];
				if ((1ull << tile) & safeProm)
					score += PARAM.PASSER_SAFE_PROM[rank];
			}

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
			score += PARAM.THREAT_PAWN_PUSH_VALUE * populationCount(push & pushTarget);
			score += PARAM.THREAT_WEAK_PAWNS_VALUE * populationCount(position.board.pieceLocations[!side][PAWN] & weakTiles);
			score += PARAM.THREAT_PAWNSxMINORS_VALUE * populationCount(attacks[side][PAWN] & minors);
			score += PARAM.THREAT_MINORSxMINORS_VALUE * populationCount((attacks[side][KNIGHT] | attacks[side][BISHOP]) & minors);
			score += PARAM.THREAT_MAJORSxWEAK_MINORS_VALUE * populationCount((attacks[side][ROOK] | attacks[side][QUEEN]) & minors & weakTiles);
			score += PARAM.THREAT_PAWN_MINORSxMAJORS_VALUE * populationCount((attacks[side][PAWN] | attacks[side][KNIGHT] | attacks[side][BISHOP]) & (position.board.pieceLocations[!side][ROOK] | position.board.pieceLocations[!side][QUEEN]));
			score += PARAM.THREAT_ALLxQUEENS_VALUE * populationCount(attacks[side][6] & position.board.pieceLocations[!side][QUEEN]);
			score += PARAM.THREAT_KINGxMINORS_VALUE * populationCount(getKingAttacks(position.board.pieceLocations[side][KING]) & minors & weakTiles);
			score += PARAM.THREAT_KINGxROOKS_VALUE * populationCount(getKingAttacks(position.board.pieceLocations[side][KING]) & position.board.pieceLocations[!side][ROOK] & weakTiles);
			return score;
		}

		constexpr Score Evaluation::control(bool side) {
			Score score = SCORE_ZERO;
			BitBoard controlledTiles = attackedBy2[side] & attacks[!side][6] & ~attackedBy2[!side] & ~attacks[!side][PAWN];
			score += PARAM.PIECE_SPACE * populationCount(controlledTiles & position.board.occupied[BOTH]);
			score += PARAM.EMPTY_SPACE * populationCount(controlledTiles & ~position.board.occupied[BOTH]);
			return score;
		}

		int Evaluation::evaluate(const Position& position) {
			static const BitBoard OutpostMask[2] = { rankMask[RANK_5] | rankMask[RANK_4] | rankMask[RANK_3], rankMask[RANK_4] | rankMask[RANK_5] | rankMask[RANK_6] };

			this->position = position;
			ally = position.status.isWhite;
			enem = !ally;

			int scale = 32;

			Score eval;

			initialise(ally); initialise(enem);

			Material mat;
			if (PARAM.getMaterial(populationCount(position.board.pieceLocations[WHITE][PAWN]),
								  populationCount(position.board.pieceLocations[BLACK][PAWN]),
								  populationCount(position.board.pieceLocations[WHITE][KNIGHT]),
								  populationCount(position.board.pieceLocations[BLACK][KNIGHT]),
								  populationCount(position.board.pieceLocations[WHITE][BISHOP]),
								  populationCount(position.board.pieceLocations[BLACK][BISHOP]),
								  populationCount(position.board.pieceLocations[WHITE][ROOK]),
								  populationCount(position.board.pieceLocations[BLACK][ROOK]),
								  populationCount(position.board.pieceLocations[WHITE][QUEEN]),
								  populationCount(position.board.pieceLocations[BLACK][QUEEN]),
								  mat)) {
				phase = mat.phase;
				eval += mat.value * (ally == BLACK ? -1 : 1);
				if (mat.flags & 1) return 0;
				if (mat.flags & 2) scale = 1;
				if (mat.flags & 4 && populationCount((position.board.pieceLocations[WHITE][BISHOP] | position.board.pieceLocations[BLACK][BISHOP]) & COLOURED_SQUARE_BB[WHITE]) == 1) scale = 16;
			}
			else {
				eval += countMaterial(ally) - countMaterial(enem);
				if (phase > totalPhase) phase = totalPhase;
				eval += PARAM.imbalance(pieceCountImb, ally) - PARAM.imbalance(pieceCountImb, enem);
			}

			eval += pawnStructure(ally) - pawnStructure(enem);
			
			mobilityMask = ~attacks[enem][PAWN] & EnemyAndEmpty(position.board, ally);
			outpostTiles = OutpostMask[ally] & attacks[ally][PAWN] & ~pawnFill(attacks[enem][PAWN], enem);
			eval += evaluatePiece<KNIGHT>(ally);
			eval += evaluatePiece<BISHOP>(ally);
			eval += evaluatePiece<ROOK>(ally);
			eval += evaluatePiece<QUEEN>(ally);

			mobilityMask = ~attacks[ally][PAWN] & EnemyAndEmpty(position.board, enem);
			outpostTiles = OutpostMask[enem] & attacks[enem][PAWN] & ~pawnFill(attacks[ally][PAWN], ally);
			eval -= evaluatePiece<KNIGHT>(enem);
			eval -= evaluatePiece<BISHOP>(enem);
			eval -= evaluatePiece<ROOK>(enem);
			eval -= evaluatePiece<QUEEN>(enem);

			eval += kingPressure(ally) - kingPressure(enem);
			
			eval += passedPawns(ally) - passedPawns(enem);
			
			eval += threats(ally) - threats(enem);
			
			eval += control(ally) - control(enem);

			int tapered = (eval.middleGame * phase + eval.endGame * (totalPhase - phase)) / totalPhase;
			return (tapered * scale / 32) + PARAM.TEMPO;
		}
	} // namespace EVALUATION
	
} // namespace KRONOS