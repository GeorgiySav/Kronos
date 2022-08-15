#pragma once

namespace KRONOS
{
	static const int MAX_PLY = 200;

	static const int MAX_ITERATIVE_DEPTH = 9;

	static const int MATE = 32000;
	static const int INFINITE = 32001;
	static const int UNDEFINED = 32002;
	static const int CANCELLED = 32003;
	static const int MATE_IN_MAX_PLY = MATE - MAX_PLY;
	static const int MATED_IN_MAX_PLY = -MATE + MAX_PLY;
	static const int TB_WIN = MATE_IN_MAX_PLY - MAX_PLY - 1;

}