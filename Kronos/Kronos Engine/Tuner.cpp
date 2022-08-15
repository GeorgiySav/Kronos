#include "Tuner.h"

namespace KRONOS
{
	
	TUNER::TUNER()
	{
		
	}

	TUNER::~TUNER()
	{
		
	}

	void TUNER::setTimePerMove(int time)
	{
		timePerMove = time;
	}

	void TUNER::giveGame(Game* ptr)
	{
		game = ptr;
	}

	void TUNER::playGame()
	{
		game->setGame(GAME_TYPE::AI_GAME);

		Move bestMove;
		while (game->getGameState() == GAME_STATE::PLAYING)
		{
			if (game->getPly() % 2 == 0) {
				search.setEvalParams(&PARAM_1);
			}
			else {
				search.setEvalParams(&PARAM_2);
			}
			bestMove = search.search(game->getPositions(), game->getPly(), timePerMove);
			game->makeMove(bestMove);
		}
		
		std::cout << "Game ended due to: " << GAME_STATE_STRINGS[(int)game->getGameState()] << std::endl;

	}

} // kRONOS