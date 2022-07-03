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

		enum class BOUND : u8 {
			BETA,
			ALPHA,
			EXACT,
		};

		struct TransEntry {
			uint16_t bestMove;      // 2 bytes
			HashLower hashLower;    // 2 bytes
			int16_t eval;           // 2 bytes
			int16_t static_eval;    // 2 bytes
			uint8_t ageFlag;		// 1 bytes
			u8 depth;			    // 1 byte
									// 10 bytes

			void saveEntry(uint64_t hash, Move move, int depth, int score, int static_eval, uint8_t flag, uint8_t generation);
			
			inline u8 flag() { return (u8)(ageFlag & 3); }
			inline u8 age() { return (u8)(ageFlag >> 2); }
			inline int ageDiff(u8 generation) {
				return (generation - age()) & 0x3F;
			}
		};

		struct TransCluster {
			TransEntry entries[BUCKET_SIZE];
			// add pading so that it is a multiple of 32 bytes
			char padding[2];

			TransCluster() : entries(), padding() {}
		};	

		class Transposition_Table {
		private:
			TransCluster* table;
			size_t size;
			u64 mask;
			u8 generation;
		public:
			Transposition_Table() : table(nullptr), size(0), mask(0), generation(0) {}
			~Transposition_Table();
			void setSize(int sizeMb);
			void clean();

			void startSearch();
			std::pair<bool, TransEntry*> probeHash(u64 hash);

			u8 getGeneration() { return generation; }

		};

		extern inline int ScoreToTranpositionTable(int score, u8 ply);
		extern inline int TranspositionTableToScore(int score, u8 ply);

	}

}


