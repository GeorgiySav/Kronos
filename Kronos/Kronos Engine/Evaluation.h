#pragma once

#include "utility.h"
#include "Move_Generation.h"
#include "Eval_Params.h"

namespace KRONOS {
	
	namespace EVALUATION {

		using namespace PARAMS;

		inline BitBoard KING_RING[2][64];
		inline BitBoard KING_SHELTER_MASK_1[2][3];
		inline BitBoard KING_SHELTER_MASK_2[2][3];
		inline BitBoard KING_SHELTER_MASK_3[2][3];
		inline const int KING_SQUARE[2][3] = { {B8, E8, G8}, {B1, E1, G1} };
		inline const int FILE_TO_SHELTER[8] = { 0, 0, 0, 1, 1, 2, 2, 2 };

		static void initEvalVars() {
			for (int c = 0; c < 2; c++) {
				for (int tile = 0; tile < 64; tile++) {
					KING_RING[c][tile] = getKingAttacks(1ULL << tile) | (1ULL << tile);
					if (c == WHITE && tile < 8) KING_RING[c][tile] |= (KING_RING[c][tile] << 8);
					if (c == BLACK && tile > 55) KING_RING[c][tile] |= (KING_RING[c][tile] >> 8);
					KING_RING[c][tile] |= tile % 8 == 0 ? KING_RING[c][tile] << 1 : 0;
					KING_RING[c][tile] |= tile % 8 == 7 ? KING_RING[c][tile] >> 1 : 0;
				}
			}
			
			for (int c = 0; c < 2; c++) {
				for (int s = 0; s < 3; s++) {
					int tile = KING_SQUARE[c][s];
					KING_SHELTER_MASK_1[c][s] = (getKingAttacks(1ULL << tile) | (1ULL << tile)) & (c == WHITE ? rankMask[RANK_2] : rankMask[RANK_7]);
					KING_SHELTER_MASK_2[c][s] = pawnPush(KING_SHELTER_MASK_1[c][s], c);
					KING_SHELTER_MASK_3[c][s] = pawnPush(KING_SHELTER_MASK_2[c][s], c);
				}
			}
		}

		// evaluates a position from the player to move perspective
		class Evaluation {
			
		private:
			PARAMS::Eval_Parameters PARAM;
			
			Position position;
			bool ally{}, enem{};

			int phase = 0;
			int totalPhase;

			BitBoard attackedBy2[2] = {};
			BitBoard attacks[2][7] = {};

			BitBoard kingRing[2] = {};
			
			basic_score atksOnKingScore[2] = { 0, 0 };

			BitBoard mobilityMask = 0ULL;
			BitBoard outpostTiles = 0ULL;

			int pieceCountImb[2][6] = {};

			constexpr void initialise(bool side);
			constexpr Score countMaterial(bool side);
			template <Pieces pieceType>
			constexpr Score evaluatePiece(bool side);
			constexpr Score pawnStructure(bool side);
			constexpr basic_score pawnStorm(bool side, int tile);
			constexpr Score passedPawns(bool side);
			inline Score kingPressure(bool side);
			constexpr Score threats(bool side);
			constexpr Score control(bool side);


		public:

			Evaluation();
			~Evaluation();

			int evaluate(const Position& position);

			void setParams(PARAMS::Eval_Parameters* newParams) {
				PARAM = *newParams;
			}

		};


	}

}