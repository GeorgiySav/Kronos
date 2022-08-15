#include "utility.h"
#include "Move_Generation.h"

namespace KRONOS
{
	
	extern Position    FENtoBoard(std::string FEN);
	extern std::string BoardToFEN(const Position* position);

	static const std::string FEN_START_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

} // KRONOS