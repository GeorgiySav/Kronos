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
			bestMove = search.search(game->getPositions(), game->getPly(), timePerMove, manager);
			game->makeMove(bestMove);
		}
		
		std::cout << "Game ended due to: " << GAME_STATE_STRINGS[(int)game->getGameState()] << std::endl;

	}

} // kRONOS