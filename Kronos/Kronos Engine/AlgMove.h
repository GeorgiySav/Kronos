#pragma once
#include "utility.h"
#include "Move_Generation.h"
#include "Rays.h"
#include "Game.h"

namespace KRONOS {
	// converts a Kronos move to algebraic notation
	extern std::string KronosMoveToAlgebraic(const Move& move, const Position& position);
}