#pragma once
#include <vector>
#include <string>
#include <limits>

namespace KRONOS
{

#define getBit(bb, i) (bb & (1ULL << i)) 
#define setBit(bb, i) (bb |= (1ULL << i))
#define popBit(bb, i) (bb &= ~(1ULL << i))

#define sinline static inline
#define CompileTime static constexpr

	typedef uint64_t BitBoard;
	typedef uint64_t u64;
	typedef uint32_t u32;
	typedef uint16_t u16;
	typedef uint8_t u8;

	typedef unsigned short uShort;


	typedef int16_t basic_score;

	struct Score {
		basic_score middleGame;
		basic_score endGame;

		constexpr Score() : middleGame(0), endGame(0) {};
		constexpr Score(basic_score mm, basic_score ee) : middleGame(mm), endGame(ee) {}
		constexpr Score operator+(const Score& x) const { return Score(middleGame + x.middleGame, endGame + x.endGame); }
		constexpr Score operator-(const Score& x) const { return Score(middleGame - x.middleGame, endGame - x.endGame); }
		constexpr Score operator/(const Score& x) const { return Score(middleGame / x.middleGame, endGame / x.endGame); }
		constexpr Score operator*(const Score& x) const { return Score(middleGame * x.middleGame, endGame * x.endGame); }
		constexpr Score operator+(basic_score x)  const { return Score(middleGame + x, endGame + x); }
		constexpr Score operator-(basic_score x)  const { return Score(middleGame - x, endGame - x); }
		constexpr Score operator/(basic_score x)  const { return Score(middleGame / x, endGame / x); }
		constexpr Score operator*(basic_score x)  const { return Score(middleGame * x, endGame * x); }
		constexpr Score& operator+=(const Score& x) { middleGame += x.middleGame, endGame += x.endGame; return *this; }
		constexpr Score& operator-=(const Score& x) { middleGame -= x.middleGame, endGame -= x.endGame; return *this; }
		constexpr Score& operator/=(const Score& x) { middleGame /= x.middleGame, endGame /= x.endGame; return *this; }
		constexpr Score& operator*=(const Score& x) { middleGame *= x.middleGame, endGame *= x.endGame; return *this; }
		constexpr Score& operator+=(const basic_score x) { middleGame += x, endGame += x; return *this; }
		constexpr Score& operator-=(const basic_score x) { middleGame -= x, endGame -= x; return *this; }
		constexpr Score& operator/=(const basic_score x) { middleGame /= x, endGame /= x; return *this; }
		constexpr Score& operator*=(const basic_score x) { middleGame *= x, endGame *= x; return *this; }
		constexpr Score& operator= (const basic_score x) { middleGame = x, endGame = x;   return *this; }
		constexpr bool operator==(const Score& x) { return (x.endGame == endGame) && (x.middleGame == middleGame); }
	};

	const Score SCORE_ZERO = Score(0, 0);

	struct Move {

		u8 from;
		u8 to;
		u8 flag : 4;
		u8 moved_Piece : 4;

		constexpr Move() : from(0), to(0), flag(0), moved_Piece(0) {}
		constexpr Move(int from, int to, int flag, u8 moved_Piece) : from(from), to(to), flag(flag), moved_Piece(moved_Piece) {}
		constexpr ~Move() {}

		friend bool operator == (const Move& lhs, const Move& rhs) {
			return lhs.from == rhs.from && lhs.to == rhs.to && lhs.flag == rhs.flag && lhs.moved_Piece == rhs.moved_Piece;
		}
	};



	static int count(const std::string& str, char aChar)
	{
		int count = 0;
		for (int i = 0; i < str.length(); i++) {
			if (str.at(i) == aChar) {
				count++;
			}
		}
		return count;
	}

	template <typename T>
	static int count(const std::vector<T> list, T item) {
		int count = 0;
		for (int i = 0; i < list.size(); i++) {
			if (list[i] == item) {
				count++;
			}
		}
		return count;
	}

	static bool instr(const std::string& str, char aChar)
	{
		return strpbrk(str.c_str(), &aChar);
	}

	static bool is_number(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}

} // namespace KRONOS