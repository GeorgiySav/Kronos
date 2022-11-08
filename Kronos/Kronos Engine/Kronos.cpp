#include "Kronos.h"

#include "polyBook.h"
#include "SyzygyTB.h"

using namespace KRONOS;

KronosEngine::KronosEngine()
{
	KRONOS::initRays();
	KRONOS::initMagics();
	
	perftTest(FEN_START_POSITION, 5);
	
	KRONOS::EVALUATION::initEvalVars();
	KRONOS::SEARCH::initVars();

	if (SEARCH::SYZYGY::initSYZYGY("./Syzygy endgame tablebases/Tablebases/")) {
		std::cout << "initialised syzygy" << std::endl;
	}
	else
		std::cout << "failed to initialise syzygy" << std::endl;


	tuner.setTimePerMove(1000);
	tuner.giveGame(&game);

	game.setGame(GAME_TYPE::AI_GAME);

	NUM_THREADS = std::thread::hardware_concurrency();
	search.initSearchThreads(1);

}

KronosEngine::~KronosEngine()
{
	deleteMagics();
	SEARCH::SYZYGY::freeSYZYGY();
}