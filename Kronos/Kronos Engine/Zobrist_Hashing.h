#pragma once
#include <random>

#include "utility.h"
#include "Move_Generation.h"
#include "BitBoard.h"

namespace KRONOS {
	
	namespace HASH {
		
		class ZobristGenerator
		{
		private:
			u64 pieceVals[2][6][64];
			u64 sideToMove[2];
			u64 castlingRights[4][2];
			u64 enPassantFiles[8];
		public:
			ZobristGenerator();
			~ZobristGenerator() {};

			u64 generateHash(const Position& position);
			// applies a move to a hash
			void updateHash(const Position& prevPos, Position& position, Move& move);
		};

		inline ZobristGenerator zobrist;

	}

}