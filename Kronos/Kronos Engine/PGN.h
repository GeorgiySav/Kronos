#pragma once
#include "utility.h"
#include "Move_Generation.h"
#include "Rays.h"
#include "Game.h"

namespace KRONOS {

	extern std::string KronosMoveToAlgebraic(const Move& move, const Position& position);
	extern void processPGN(std::string PGN, std::vector<Position>& positions, std::vector<Move>& moves, std::vector<std::string>& pgnMoves);
}