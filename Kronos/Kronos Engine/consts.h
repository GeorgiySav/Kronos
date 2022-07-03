#pragma once

namespace KRONOS
{
	const int MAX_PLY = 200;

	const int MAX_ITERATIVE_DEPTH = 7;

	const int MATE = 32000;
	const int INFINITE = 32001;
	const int UNDEFINED = 32002;
	const int CANCELLED = 32003;
	const int MATE_IN_MAX_PLY = MATE - MAX_PLY;
	const int MATED_IN_MAX_PLY = -MATE + MAX_PLY;
	const int TB_WIN = MATE_IN_MAX_PLY - MAX_PLY - 1;
}