#include "Kronos.h"

#include "polyBook.h"
#include "SyzygyTB.h"

using namespace KRONOS;

KronosEngine::KronosEngine()
{
	KRONOS::initRays();
	KRONOS::initMagics();
	KRONOS::initMGVars();
	
	//perftTest(FENtoBoard(FEN_START_POSITION), 6);
	
	KRONOS::EVALUATION::initEvalVars();
	KRONOS::SEARCH::initVars();

	if (SEARCH::SYZYGY::initSYZYGY("./Syzygy endgame tablebases/Tablebases/")) {
		std::cout << "initialised syzygy" << std::endl;
	}
	else
		std::cout << "failed to initialise syzygy" << std::endl;

	game.setGame();

	NUM_THREADS = std::thread::hardware_concurrency();
	search.initSearchThreads(NUM_THREADS - 3);

}

KronosEngine::~KronosEngine()
{
	deleteMagics();
	SEARCH::SYZYGY::freeSYZYGY();
}