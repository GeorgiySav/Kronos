#pragma once

#include "utility.h"

namespace KRONOS
{

	struct Move {

		u8 from;
		u8 to;
		u8 flag : 4;
		u8 moved_Piece : 4;

		constexpr Move() : from(0), to(0), flag(0), moved_Piece(0) {}
		constexpr Move(int from, int to, int flag, u8 moved_Piece) : from(from), to(to), flag(flag), moved_Piece(moved_Piece) {}
		constexpr ~Move() {}

		uint16_t toIntMove();

		friend bool operator == (const Move& lhs, const Move& rhs) {
			return lhs.from == rhs.from && lhs.to == rhs.to && lhs.flag == rhs.flag && lhs.moved_Piece == rhs.moved_Piece;
		}
	};

} // namespace KRONOS


