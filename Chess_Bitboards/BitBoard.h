#pragma once

#include <cassert>

#include "utility.h"

namespace CHENG {
	
	typedef u64 BitBoard;
	
	const u64 EMPTY = 0ULL;
	const u64 UNIVERSE = 0xFFFFFFFFFFFFFFFFULL;
	
	const u64 notAFile = 18374403900871474942ULL;
	const u64 notHFile = 9187201950435737471ULL;
	
	const u64 fileMask[8]{
		72340172838076673ULL,
		144680345676153346ULL,
		289360691352306692ULL,
		578721382704613384ULL,
		1157442765409226768ULL,
		2314885530818453536ULL,
		4629771061636907072ULL,
		9259542123273814144ULL
	};
	
	enum fileIndex {
		A, B, C, D, E, F, G, H
	};

	const u64 rankMask[8]{
		255ULL,
		65280ULL,
		16711680ULL,
		4278190080ULL,
		1095216660480ULL,
		280375465082880ULL,
		71776119061217280ULL,
		18374686479671623680ULL
	};

	enum rankIndex {
		RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
	};

	static uint64_t getFileMask(uShort tile) {
		return fileMask[(tile % 8)];
	}
	
	static uint64_t getRankMask(uShort tile) {
		return rankMask[tile / 8];
	}

	const int BitTable[64] = {
		0, 47,  1, 56, 48, 27,  2, 60,
       57, 49, 41, 37, 28, 16,  3, 61,
       54, 58, 35, 52, 50, 42, 21, 44,
       38, 32, 29, 23, 17, 11,  4, 62,
       46, 55, 26, 59, 40, 36, 15, 53,
       34, 51, 20, 43, 31, 22, 10, 45,
       25, 39, 14, 33, 19, 30,  9, 24,
       13, 18,  8, 12,  7,  6,  5, 63
	};

	static const u64 debruijn64 = 0x03f79d71b4cb0a89;

	static uShort bitScanForward(u64 bb) {
		assert(bb != 0);
		return BitTable[((bb ^ (bb - 1)) * debruijn64) >> 58];
	}

	static uShort bitScanReverse(u64 bb) {
		assert(bb != 0);
		bb |= bb >> 1;
		bb |= bb >> 2;
		bb |= bb >> 4;
		bb |= bb >> 8;
		bb |= bb >> 16;
		bb |= bb >> 32;
		return BitTable[(bb * debruijn64) >> 58];
	}

	static uShort populationCount(u64 bb) {
		int count = 0;
		while (bb) {
			count++;
			bb &= bb - 1;
		}
		return count;
	}

	static u64 northOne(u64 b) { return (b << 8); }
	static u64 southOne(u64 b) { return (b >> 8); }
	static u64 eastOne(u64 b)  { return (b << 1) & notAFile; }
	static u64 westOne(u64 b)  { return (b >> 1) & notHFile; }
	static u64 NEOne(u64 b)    { return (b << 9) & notAFile; }
	static u64 NWOne(u64 b)    { return (b << 7) & notHFile; }
	static u64 SEOne(u64 b)    { return (b >> 7) & notAFile; }
	static u64 SWOne(u64 b)    { return (b >> 9) & notHFile; }

	static u64 flipX(u64 b)	   { static const u64 k1 = 0x00FF00FF00FF00FF;
								 static const u64 k2 = 0x0000FFFF0000FFFF;
								 b = ((b >> 8) & k1) | ((b & k1) << 8);
								 b = ((b >> 16) & k2) | ((b & k2) << 16);
								 b = (b >> 32) | (b << 32);
								 return b; }
	static u64 flipY(u64 b)	   { static const u64 k1 = 0x5555555555555555;
						         static const u64 k2 = 0x3333333333333333;
						         static const u64 k4 = 0x0f0f0f0f0f0f0f0f;
						         b = ((b >> 1) & k1) | ((b & k1) << 1);
						         b = ((b >> 2) & k2) | ((b & k2) << 2);
						         b = ((b >> 4) & k4) | ((b & k4) << 4);
						         return b; }
	static u64 flipA1H8(u64 b) { u64 t;
								 static const u64 k1 = 0x5500550055005500;
								 static const u64 k2 = 0x3333000033330000;
								 static const u64 k4 = 0x0f0f0f0f00000000;
								 t = k4 & (b ^ (b << 28));
								 b ^= t ^ (t >> 28);
								 t = k2 & (b ^ (b << 14));
								 b ^= t ^ (t >> 14);
								 t = k1 & (b ^ (b << 7));
								 b ^= t ^ (t >> 7);
								 return b; }	
	static u64 flipA8H1(u64 b) { u64 t;
								 static const u64 k1 = 0xaa00aa00aa00aa00;
								 static const u64 k2 = 0xcccc0000cccc0000;
								 static const u64 k4 = 0xf0f0f0f00f0f0f0f;
								 t = b ^ (b << 36);
								 b ^= k4 & (t ^ (b >> 36));
								 t = k2 & (b ^ (b << 18));
								 b ^= t ^ (t >> 18);
								 t = k1 & (b ^ (b << 9));
								 b ^= t ^ (t >> 9);
								 return b; }

	static u64 rotate180(u64 b)  { return flipX(flipY(b)); }
	static u64 rotate90C(u64 b)  { return flipX(flipA1H8(b)); }
	static u64 rotate90AC(u64 b) { return flipA1H8(flipY(b)); }

	static u64 shiftNorth(u64 b, uShort n) { for (uShort i = 0; i < n; i++) b = northOne(b); return b; }
	static u64 shiftSouth(u64 b, uShort n) { for (uShort i = 0; i < n; i++) b = southOne(b); return b; }
	static u64 shiftEast(u64 b, uShort n)  { for (uShort i = 0; i < n; i++) b = eastOne(b); return b; }
	static u64 shiftWest(u64 b, uShort n) { for (uShort i = 0; i < n; i++) b = westOne(b); return b; }

	static u64 shiftNE(u64 b, uShort n) { for (uShort i = 0; i < n; i++) b = NEOne(b); return b; }
	static u64 shiftNW(u64 b, uShort n) { for (uShort i = 0; i < n; i++) b = NWOne(b); return b; }
	static u64 shiftSE(u64 b, uShort n) { for (uShort i = 0; i < n; i++) b = SEOne(b); return b; }
	static u64 shiftSW(u64 b, uShort n) { for (uShort i = 0; i < n; i++) b = SWOne(b); return b; }

	static std::string _BitBoard(u64 b) {
		std::string str = "";
		for (int i = 56; i >= 0; i -= 8) {
			str += char('1' + i / 8); str += " ";
			for (int j = 0; j < 8; j++) {
				int tile = i + j;
				str += (getBit(b, tile) ? "1" : "."); str += " ";
			}
			str += "\n";
		}
		str += "  A B C D E F G H\n";
		str += "  Bitboard: " + std::to_string(b) + "\n";
		return str;
	}
	
}