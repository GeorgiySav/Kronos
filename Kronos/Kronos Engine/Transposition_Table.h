#pragma once
#include <cstdint>
#include <optional>

#include "Move_Generation.h"
#include "utility.h"
#include "Evaluation.h"

namespace KRONOS {
	
	namespace HASH {
		
		typedef uint32_t HashLower;
#define HASHLOWER(x) (HashLower)(x >> 32)
#define BUCKET_SIZE 3
#define ABDADA_BUCKET_SIZE 4

		enum class BOUND : u8 {
			EXACT,
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
			HashLower hashLock; // 32
			u16 move; // 16
			u8 depth; // 8
			int16_t eval; // 16
			// total = 80
			int getAge() { return age & 63; }
			int getBound() { return age >> 6; }
			void setAgeBound(int a, int b) { age = a | (b << 6); }
			void setAge(int a) { age &= 192; age |= a; }
		private:
			u8 age; // 8
		};
		#pragma pack(pop)

		struct transBucket {
			transEntry bucket[BUCKET_SIZE]; // 80 * 3 = 240
			int16_t Padding;
		};

		class Transposition_Table : public HASH_TABLE<transBucket>
		{
		private:
			int currentAge = 0;
		public:
			void resetAge() { currentAge = 0; }
			void updateAge() { currentAge = (currentAge + 1) % 64; }
			void saveEntry(u64 hash, u16 move, int depth, int16_t eval, int bound);
			bool probe(u64 hash, transEntry& entry);
		};
		
		extern inline int ScoreToTranpositionTable(int score, u8 ply);
		extern inline int TranspositionTableToScore(int score, u8 ply);

		#pragma pack(push, 1)
		struct evalEntry
		{
			HashLower hashLock;
			int16_t eval;
		};
		#pragma pack(pop)

		struct Eval_Bucket {
			evalEntry bucket[5]; // 48 * 5 = 240
			int16_t padding;
		};

		struct Eval_Table : public HASH_TABLE<Eval_Bucket>
		{
			int16_t getEval(Position& position, EVALUATION::Evaluation& evaluation);
		};

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


