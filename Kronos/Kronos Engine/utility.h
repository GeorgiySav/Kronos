#pragma once
#include <vector>
#include <string>
#include <limits>

#define getBit(bb, i) (bb & (1ULL << i)) 
#define setBit(bb, i) (bb |= (1ULL << i))
#define popBit(bb, i) (bb &= ~(1ULL << i))

#define sinline static inline
#define CompileTime static constexpr

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef unsigned short uShort;

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