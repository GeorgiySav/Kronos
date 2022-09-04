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
			void updateHash(const Position& prevPos, Position& position, Move& move);
			void nullMove(u64& hash, int oldEP);
		};

		inline ZobristGenerator zobrist;

	}

}