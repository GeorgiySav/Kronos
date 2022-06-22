#pragma once

#include "utility.h"
#include "Move_Generation.h"

namespace KRONOS {
	
	namespace EVALUATION {

		// evaluates a position from the player to move perspective
		class Evaluation {
			
		private:


			Score PST[2][6][64];
			
			Position position;
			bool ally{}, enem{};

			int phase{};
			int totalPhase;

			BitBoard attackedBy2[2] = {};
			BitBoard attacks[2][7] = {};

			BitBoard kingRing[2] = {};
			int kingAttackersCount[2] = {};
			int kingAttacksCount[2] = {};
			int kingAttackersWeight[2] = {};

			BitBoard pinned = 0ULL;

			BitBoard mobilityMask = 0ULL;

			constexpr void initialise(bool side);
			constexpr Score countMaterial(bool side);
			template <Pieces pieceType>
			constexpr Score evaluatePiece(bool side);
			constexpr Score pawnStructure(bool side);
			inline Score kingSafety(bool side);
			constexpr Score threats(bool side);
			constexpr Score control(bool side);
			
		public:

			Evaluation();
			~Evaluation();

			int evaluate(const Position& position);
			int tracedEval(const Position& position);

		};


	}

}