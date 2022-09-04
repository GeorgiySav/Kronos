#pragma once

#include "utility.h"
#include "Move_Generation.h"
#include "Eval_Params.h"

namespace KRONOS
{

	enum class GAME_STATE {
		PLAYING,
		WHITE_DEALT_CHECKMATE,
		BLACK_DEALT_CHECKMATE,
		DRAW_TO_50_MOVE,
		DRAW_TO_STALEMATE,
		DRAW_TO_REPETITION,
		DRAW_TO_LACK_OF_MATERIAL,
		CANCELLED_GAME,
	};

	static std::string GAME_STATE_STRINGS[] = {
		"PLAYING",
		"WHITE_DEALT_CHECKMATE",
		"BLACK_DEALT_CHECKMATE",
		"DRAW_TO_50_MOVE",
		"DRAW_TO_STALEMATE",
		"DRAW_TO_REPETITION",
		"DRAW_TO_LACK_OF_MATERIAL",
		"CANCELLED_GAME"
	};

	enum class GAME_TYPE {
		AI_GAME,
		HUMAN_GAME
	};

	class Game
	{
	private:
		std::vector<Position> positions;
		std::vector<Move> moveHistory;
		int ply;

		GAME_STATE gameState;
		GAME_TYPE gameType;
		

		Move_List<256> moves;

		void checkGameState();
	public:
		Game();
		~Game();
		
		void setGame(GAME_TYPE gt, std::string FEN);
		void setGame(GAME_TYPE gt);


		bool makeMove(Move move);
		void undoMove();

		void clear();
		
		GAME_STATE getGameState() {
			return gameState;
		}

		std::vector<Position>* getPositions() {
			return &positions;
		}

		int getPly() {
			return ply;
		}

		Board* getBoardPointer() {
			return &positions[ply].board;
		}

		BoardStatus* getStatusPointer() {
			return &positions[ply].status;
		}

		Move_List<256>* getMovesPointer() {
			return &moves;
		}
	
	};
	

	

} // KRONOS