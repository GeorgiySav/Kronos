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

		constexpr bool isTactical() { return flag & 0b1100; }
	};

	class  Move_List {
	public:
		constexpr Move_List() { moves.reserve(30); }
		constexpr void add(const Move& m) { moves.emplace_back(m); }
		constexpr void clear() { moves.clear(); }
		constexpr Move& at(const int index) { return moves.at(index); }
		constexpr bool contains(const Move& m) {
			for (const auto& move : moves)	if (move == m) return true;
			return false;
		}
		constexpr void insert(const Move& m, int index) {
			moves.insert(moves.begin() + index, m);
		}
		constexpr u8 size() { return moves.size(); }

	private:
		std::vector<Move> moves;
	};

	inline const Move NULL_MOVE(0, 0, 0, 0);

} // namespace KRONOS


