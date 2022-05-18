#pragma once

#include "Move_Generation.h"

namespace KRONOS {
	
	namespace EVALUATION {
		
		const int PAWN_VALUE = 100;
		const int KNIGHT_VALUE = 300;
		const int BISHOP_VALUE = 300;
		const int ROOK_VALUE = 500;
		const int QUEEN_VALUE = 900;
		const int KING_VALUE = 20000;

		CompileTime int countMaterial(Position* position, bool isWhite) {		
			int count = 0;

			count += populationCount(position->board.pieceLocations[isWhite][PAWN])    * PAWN_VALUE;
			count += populationCount(position->board.pieceLocations[isWhite][KNIGHT])  * KNIGHT_VALUE;
			count += populationCount(position->board.pieceLocations[isWhite][BISHOP])  * BISHOP_VALUE;
			count += populationCount(position->board.pieceLocations[isWhite][ROOK])    * ROOK_VALUE;
			count += populationCount(position->board.pieceLocations[isWhite][QUEEN])   * QUEEN_VALUE;
			count += populationCount(position->board.pieceLocations[isWhite][KING])    * KING_VALUE;

			count -= populationCount(position->board.pieceLocations[!isWhite][PAWN])   * PAWN_VALUE;
			count -= populationCount(position->board.pieceLocations[!isWhite][KNIGHT]) * KNIGHT_VALUE;
			count -= populationCount(position->board.pieceLocations[!isWhite][BISHOP]) * BISHOP_VALUE;
			count -= populationCount(position->board.pieceLocations[!isWhite][ROOK])   * ROOK_VALUE;
			count -= populationCount(position->board.pieceLocations[!isWhite][QUEEN])  * QUEEN_VALUE;
			count -= populationCount(position->board.pieceLocations[!isWhite][KING])   * KING_VALUE;

			return count;
		}

		CompileTime int staticEvaluation(Position* position, bool isWhite) {
			int eval = 0;

			eval += countMaterial(position, isWhite);
			
			return eval;
		}

	}

}