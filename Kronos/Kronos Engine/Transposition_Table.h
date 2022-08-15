#pragma once
#include <cstdint>
#include <optional>

#include "Move_Generation.h"
#include "utility.h"

namespace KRONOS {
	
	namespace HASH {
		
		typedef uint16_t HashLower;
#define HASHLOWER(x) (HashLower)(x)
#define BUCKET_SIZE 3
#define ABDADA_BUCKET_SIZE 4

		enum class BOUND : u8 {
			EXACT = 1,
			BETA,
			ALPHA,
		};

		template <typename hashEntry>
		class HASH_TABLE
		{
		protected:
			hashEntry* table;
			u64 size;
			u64 mask;
			
		public:
			HASH_TABLE() : table(nullptr), size(0), mask(0) {}
			~HASH_TABLE() { delete[] table; }
			
			hashEntry* getEntry(const u64 hash) { return &table[hash & mask]; }
			void clear() { memset(table, 0, size * sizeof(hashEntry)); }
			void setSize(int sizeMB) {
				size = (1 << 20) / sizeof(hashEntry); // sizeMb must be atleast 1 MB
				for (sizeMB <<= 19; size * sizeof(hashEntry) <= sizeMB; size <<= 1);
				mask = size - 1;
				delete[] table;
				table = new hashEntry[size];
				clear();
			}
			HashLower lock(u64 hash) const { return HASHLOWER(hash); }
		};

		#pragma pack(push, 1)
		struct transEntry
		{
			HashLower hashLock; // 16
			u16 move; // 16
			u8 depth; // 8
			int16_t eval; // 16
			// total = 64
			int getAge() { return age & 63; }
			int getBound() { return age >> 6; }
			void setAgeBound(int a, int b) { age = a | (b << 6); }
			void setAge(int a) { age &= 192; age |= a; }
		private:
			u8 age; // 8
		};
		#pragma pack(pop)

		struct transBucket {
			transEntry bucket[BUCKET_SIZE]; // 192
			// char Padding[];
		};

		class Transposition_Table : public HASH_TABLE<transBucket>
		{
		private:
			int currentAge;
		public:
			void resetAge() { currentAge = 0; }
			void updateAge() { currentAge = (currentAge + 1) % 64; }
			void saveEntry(u64 hash, u16 move, int depth, int16_t eval, int bound);
			bool probe(u64 hash, transEntry& entry);
		};
		
		extern inline int ScoreToTranpositionTable(int score, u8 ply);
		extern inline int TranspositionTableToScore(int score, u8 ply);


		struct MOVE_HASH {
			u16 move;
			u8 depth;
			u8 hashLock;
		};

		struct MOVE_HASH_BUCKET {
			MOVE_HASH bucket[ABDADA_BUCKET_SIZE];
		};

		struct ABDADA_TABLE : public HASH_TABLE<MOVE_HASH_BUCKET>
		{
			void setBusy(u64 hash, u16 move, int depth);
			void resetBusy(u64 hash, u16 move, int depth);
			bool isBusy(u64 hash, u16 move, int depth);
			u8 mhlock(u64 hash) { return (u8)(hash); }
		};

	}

}


