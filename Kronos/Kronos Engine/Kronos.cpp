#include "Kronos.h"

#include "polyBook.h"
#include "SyzygyTB.h"

using namespace KRONOS;

KronosEngine::KronosEngine()
{
	KRONOS::initRays();
	KRONOS::initMagics();
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

	//processFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
	//
	//std::cout << _BitBoard(positions[ply].board.occupied[BOTH]) << std::endl;
	//
	//auto begin = std::chrono::steady_clock::now();
	//
	//std::cout << "Ply 1: " << perft(1) << std::endl;
	//std::cout << "In: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << "ms\n" << std::endl;
	//
	//begin = std::chrono::steady_clock::now();
	//std::cout << "Ply 2: " << perft(2) << std::endl;
	//std::cout << "In: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << "ms\n" << std::endl;
	//
	//begin = std::chrono::steady_clock::now();
	//std::cout << "Ply 3: " << perft(3) << std::endl;
	//std::cout << "In: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << "ms\n" << std::endl;
	//
	//begin = std::chrono::steady_clock::now();
	//std::cout << "Ply 4: " << perft(4) << std::endl;
	//std::cout << "In: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << "ms\n" << std::endl;
	//
	//begin = std::chrono::steady_clock::now();
	//std::cout << "Ply 5: " << perft(5) << std::endl;
	//std::cout << "In: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << "ms\n" << std::endl;
	//
	//begin = std::chrono::steady_clock::now();
	//std::cout << "Ply 6: " << perft(6) << std::endl;
	//std::cout << "In: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << "ms\n" << std::endl;
}

KronosEngine::~KronosEngine()
{
	deleteMagics();
	deleteRays();
	SEARCH::SYZYGY::freeSYZYGY();
}