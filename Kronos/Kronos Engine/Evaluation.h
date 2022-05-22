#pragma once

#include "Move_Generation.h"

namespace KRONOS {
	
	namespace EVALUATION {
		
		namespace EVAL_PARAMS {
			
			const int PAWN_VALUE = 100;
			const int KNIGHT_VALUE = 310;
			const int BISHOP_VALUE = 320;
			const int ROOK_VALUE = 500;
			const int QUEEN_VALUE = 900;
			const int KING_VALUE = 20000;

			const int CONNECTED_PAWN_VALUE = 20;
			const int DOUBLED_PAWN_VALUE = -5;
			const int ISOLATED_PAWN_VALUE = -10;
			const int HALF_ISOLATED_PAWN_VALUE = 3;
			const int BACKWARD_PAWN_VALUE = -7;
			const int PASSED_ISOLATED_PAWN_VALUE = 7;
			const int PASSED_BACKWARD_PAWN_VALUE = 15;

		}

		using namespace EVAL_PARAMS;

		// evaluates a position from the player to move perspective
		class Evaluation {
			
		private:

			
			Position position;
			bool ally, enem;

			BitBoard attacks[2], pawnAttacks[2], knightAttacks[2], bishopAttacks[2], rookAttacks[2], queenAttacks[2], kingAttacks[2];

		public:

			Evaluation() : position(Position()), ally(false), enem(false) {
				for (int i = 0; i < 2; i++) {
					attacks[i] = 0;
					pawnAttacks[i] = 0;
					knightAttacks[i] = 0;
					bishopAttacks[i] = 0;
					rookAttacks[i] = 0;
					queenAttacks[i] = 0;
					kingAttacks[i] = 0;
				}
			}
			
			~Evaluation() {
				
			}
			
			constexpr void prepareEval() {
				// set bitboards to 0ULL
				for (int i = 0; i < 2; i++) {
					attacks[i] = 0ULL;
					pawnAttacks[i] = 0ULL;
					knightAttacks[i] = 0ULL;
					bishopAttacks[i] = 0ULL;
					rookAttacks[i] = 0ULL;
					queenAttacks[i] = 0ULL;
					kingAttacks[i] = 0ULL;
				}

				// temp vars
				BitBoard b0;

				pawnAttacks[enem] = getPawnAttacks(position.board.pieceLocations[enem][PAWN], enem);
				knightAttacks[enem] = getKnightAttacks(position.board.pieceLocations[enem][KNIGHT]);
			
				b0 = position.board.pieceLocations[enem][BISHOP];
				while (b0) {
					int tile = bitScanForward(b0);
					popBit(b0, tile);

					bishopAttacks[enem] |= getBishopAttacks(position.board.occupied[BOTH], tile);
				}

				b0 = position.board.pieceLocations[enem][ROOK];
				while (b0) {
					int tile = bitScanForward(b0);
					popBit(b0, tile);

					rookAttacks[enem] |= getRookAttacks(position.board.occupied[BOTH], tile);
				}

				b0 = position.board.pieceLocations[enem][QUEEN];
				while (b0) {
					int tile = bitScanForward(b0);
					popBit(b0, tile);

					queenAttacks[enem] |= getBishopAttacks(position.board.occupied[BOTH], tile) | getRookAttacks(position.board.occupied[BOTH], tile);
				}

				knightAttacks[enem] = getKingAttacks(position.board.pieceLocations[enem][KING]);
				

				pawnAttacks[ally] = getPawnAttacks(position.board.pieceLocations[ally][PAWN], ally);
				knightAttacks[ally] = getKnightAttacks(position.board.pieceLocations[ally][KNIGHT]);

				b0 = position.board.pieceLocations[ally][BISHOP];
				while (b0) {
					int tile = bitScanForward(b0);
					popBit(b0, tile);

					bishopAttacks[ally] |= getBishopAttacks(position.board.occupied[BOTH], tile);
				}
				
				b0 = position.board.pieceLocations[ally][ROOK];
				while (b0) {
					int tile = bitScanForward(b0);
					popBit(b0, tile);

					rookAttacks[ally] |= getRookAttacks(position.board.occupied[BOTH], tile);
				}
				
				b0 = position.board.pieceLocations[ally][QUEEN];
				while (b0) {
					int tile = bitScanForward(b0);
					popBit(b0, tile);

					queenAttacks[ally] |= getBishopAttacks(position.board.occupied[BOTH], tile) | getRookAttacks(position.board.occupied[BOTH], tile);
				}

				knightAttacks[ally] = getKingAttacks(position.board.pieceLocations[ally][KING]);
				
				attacks[enem] = pawnAttacks[enem] | knightAttacks[enem] | bishopAttacks[enem] | rookAttacks[enem] | queenAttacks[enem] | kingAttacks[enem];
				attacks[ally] = pawnAttacks[ally] | knightAttacks[ally] | bishopAttacks[ally] | rookAttacks[ally] | queenAttacks[ally] | kingAttacks[ally];

			}

			constexpr int countMaterial(bool side) {

				return KING_VALUE   * populationCount(position.board.pieceLocations[side][KING])
					 + QUEEN_VALUE  * populationCount(position.board.pieceLocations[side][QUEEN])
					 + ROOK_VALUE   * populationCount(position.board.pieceLocations[side][ROOK])
					 + BISHOP_VALUE * populationCount(position.board.pieceLocations[side][BISHOP])
					 + KNIGHT_VALUE * populationCount(position.board.pieceLocations[side][KNIGHT])
					 + PAWN_VALUE   * populationCount(position.board.pieceLocations[side][PAWN]);
			}

			constexpr int mobility(bool side) {

				BitBoard mobilityMask = ~(pawnAttacks[!side] & EnemyAndEmpty(position.board, side));

				int knightMobility = populationCount(knightAttacks[side] & mobilityMask);
				int bishopMobility = populationCount(bishopAttacks[side] & mobilityMask);
				
				mobilityMask |= ~((knightAttacks[!side] | bishopAttacks[!side]) & EnemyAndEmpty(position.board, side));
				int rookMobiltiy = populationCount(rookAttacks[side] & mobilityMask);
				
				mobilityMask |= ~(rookAttacks[!side] & EnemyAndEmpty(position.board, side));
				int queenMobility = populationCount(queenAttacks[side] & mobilityMask);

				return knightMobility + bishopMobility + rookMobiltiy + queenMobility;

			}

			constexpr int pawnStructure(bool side) {
				BitBoard allyPawns = position.board.pieceLocations[side][PAWN];
				BitBoard enemyPawns = position.board.pieceLocations[!side][PAWN];
				
				BitBoard passedPawns = allyPawns & ~(allyPawns & pawnFill(enemyPawns, !side));
				BitBoard connectedPawns = allyPawns & (pawnAttacks[side] | pawnPush(pawnAttacks[side], !side));
				BitBoard doubledPawns = allyPawns & (pawnFillEx(allyPawns, !side));
				
				BitBoard noWestNeighbour = allyPawns & ~(westOne(fileFill(allyPawns)));
				BitBoard noEastNeighbour = allyPawns & ~(eastOne(fileFill(allyPawns)));
				BitBoard isolatedPawns = noWestNeighbour & noEastNeighbour;
				BitBoard halfIsolatedPawns = noWestNeighbour ^ noEastNeighbour;

				BitBoard backwardPawns = (allyPawns & ~isolatedPawns) & (pawnPush(allyPawns, side) & attacks[!side]) & ~pawnFillEx(getPawnAttacks(allyPawns, !side), side);

				int score = 0;
				
				score += CONNECTED_PAWN_VALUE       * populationCount(connectedPawns);
				score += DOUBLED_PAWN_VALUE         * populationCount(doubledPawns);
				score += ISOLATED_PAWN_VALUE        * populationCount(isolatedPawns & ~passedPawns);
				score += HALF_ISOLATED_PAWN_VALUE   * populationCount(halfIsolatedPawns & ~passedPawns);
				score += BACKWARD_PAWN_VALUE        * populationCount(backwardPawns & ~passedPawns);			
				score += PASSED_ISOLATED_PAWN_VALUE * populationCount(passedPawns & isolatedPawns);
				score += PASSED_BACKWARD_PAWN_VALUE * populationCount(passedPawns & backwardPawns);

				return score;
			}

			constexpr int evaluate(const Position& position) {
				this->position = position;
				ally = position.status.isWhite;
				enem = !ally;

				int eval = 0;

				prepareEval();

				eval += countMaterial(ally) - countMaterial(enem);

				eval += mobility(ally) - mobility(enem);

				eval += pawnStructure(ally) - pawnStructure(enem);

				return eval;
			}

		};


	}

}