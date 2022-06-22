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
		
		enum class BOUND : int {
			BETA,
			ALPHA,
			EXACT,
		};

		struct TransEntry {
			Move bestMove;          // 3 bytes
			HashLower hashLower;    // 2 bytes
			basic_score eval;       // 2 bytes
			u8 depth;			    // 1 byte
			bool ancient : 4;       
			u8 bound : 4;           // 1 byte   
									// 9 bytes

			void saveEntry(Move bestMove, HashLower hashLower, basic_score eval, u8 depth, BOUND bound);
		};

		struct TransCluster {
			TransEntry entrys[BUCKET_SIZE];
			
			TransCluster() : entrys() {}
		};	

		class Transposition_Table {
		private:
			TransCluster* table;
			size_t size;
			u64 mask;
		public:
			Transposition_Table() : table(nullptr), size(0), mask(0) {}
			~Transposition_Table();
			void setSize(int sizeMb);
			void clean();
			void setAncient();
			std::pair<bool, TransEntry*> probeHash(u64 hash);


		};

	}

}


