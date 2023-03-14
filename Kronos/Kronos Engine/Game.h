#pragma once

#include "utility.h"
#include "Move_Generation.h"
#include "Eval_Params.h"
#include "FEN.h"

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
	};

	static std::string GAME_STATE_STRINGS[] = {
		"Playing",
		"White dealt checkmate",
		"Black dealt checkmate",
		"Draw due to 50 move rule",
		"Stalemate",
		"Draw due to repetition",
		"Draw due to lack of material",
	};

	enum class GAME_TYPE {
		EMPTY,
		HUMAN_VS_AI,
		HUMAN_VS_HUMAN,
		ANALYSIS
	};

	class Game
	{
	private:
		std::vector<Position> positions;
		std::vector<Move> moveHistory;
		int ply;

		GAME_STATE gameState;	
		GAME_TYPE gameType;
		int materialScore[2];

		Move_List moves;

		// used to check if the game has ended yet
		void checkGameState();
		// used to calculate the material difference
		void calculateMaterial();
	public:
		Game();
		~Game();

		// clears all information
		void clear();

		// creates a game
		template <GAME_TYPE type>
		void createGame(const std::string& FEN)
		{
			this->clear();
			positions.at(ply) = FENtoBoard(FEN);

			checkGameState();
			gameType = type;
		}

		// makes a move
		bool makeMove(Move move);
		// unmakes a move
		void undoMove();
	
		GAME_STATE getGameState() {
			return gameState;
		}

		GAME_TYPE getGameType() {
			return gameType;
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

		Move_List* getMovesPointer() {
			return &moves;
		}

		int getMaterial(bool side) {
			return materialScore[side];
		}

		void changeToAnalysis() {
			gameType = GAME_TYPE::ANALYSIS;
		}

		std::string getFen() {
			return BoardToFEN(&positions.at(ply));
		}
	
	};
	

	

} // KRONOS