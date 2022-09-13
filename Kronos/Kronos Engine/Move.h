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

	template<int N>
	struct Move_List {
	public:
		Move* begin() { return &list[0]; }
		Move* end() { return &list[size - 1]; }
		Move const* begin() const { return &list[0]; }
		Move const* end() const { return &list[size - 1]; }

		Move_List() : size(0) {}
		void add(const Move& m) { list[size++] = m; }
		void clear() { size = 0; }
		Move& at(int index) { return list[index]; }
		bool contains(const Move& m) { 
			for (int i = 0; i < size; i++)
				if (list[i] == m) 
					return true; 
			return false;
		}
		void insert(const Move& m, int index) {
			for (int i = size - 1; i >= index; i--) {
				list[i + 1] = list[i];
			}
			list[index] = m;
			size++;
		}

		int size;

	private:
		Move list[N];
	};

	inline const Move NULL_MOVE(0, 0, 0, 0);

} // namespace KRONOS


