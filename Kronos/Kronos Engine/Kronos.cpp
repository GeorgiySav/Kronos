#include "Kronos.h"

#include "polyBook.h"
#include "SyzygyTB.h"

namespace KRONOS
{
	KronosEngine::KronosEngine()
	{
		// initialise tables
		KRONOS::initRays();
		KRONOS::initMagics();
		KRONOS::initMGVars();
		
		KRONOS::EVALUATION::initEvalVars();
		KRONOS::SEARCH::initVars();

		if (SEARCH::SYZYGY::initSYZYGY("./Syzygy endgame tablebases/Tablebases/"))
			std::cout << "initialised syzygy" << std::endl;
		else
			std::cout << "failed to initialise syzygy" << std::endl;

		// doesn't initialise the search with the maximum number of cores as too allow some threads to focus on the application and not the engine
		search.initSearchThreads(std::max((NUM_THREADS - 3), 1));
	}

	KronosEngine::~KronosEngine()
	{
		SEARCH::SYZYGY::freeSYZYGY();
	}
}
