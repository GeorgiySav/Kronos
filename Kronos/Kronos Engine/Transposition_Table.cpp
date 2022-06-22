#include "Transposition_Table.h"

namespace KRONOS {
	namespace HASH {

		Transposition_Table::~Transposition_Table()
		{
			if (table)
				delete[] table;
		}

		void Transposition_Table::setSize(int sizeMb)
		{
			size = (sizeMb * 1000 * 1000) / sizeof(TransCluster);
			mask = size - 1;
			if (table) delete[] table;
			table = new TransCluster[size];
			if (table == NULL) {
				std::cout << "Error: Cannot allocate memory for transposition table" << std::endl;
				exit(1);
			}
			clean();
		}

		void Transposition_Table::clean()
		{
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < BUCKET_SIZE; j++) {
					this->table[i].entrys[j].ancient = true;
					this->table[i].entrys[j].bestMove = Move();
					this->table[i].entrys[j].depth = 0;
					this->table[i].entrys[j].eval = 0;
					this->table[i].entrys[j].hashLower = 0;
					this->table[i].entrys[j].bound = 0;
				}
			}
		}

		void TransEntry::saveEntry(Move bestMove, HashLower hashLower, basic_score eval, u8 depth, BOUND bound)
		{
			if (bound == BOUND::EXACT || depth > this->depth || this->ancient) {
				this->depth = depth;
				this->bestMove = bestMove;
				this->hashLower = hashLower;
				this->eval = eval;
				this->bound = (u8)bound;
				this->ancient = true;
			}
		}
		
		/* returns a pair of a bool and a pointer to a transposition entry. If the boolean is true,
		   that means that the search should use this entry, otherwise it should replace that entry */
		std::pair<bool, TransEntry*> Transposition_Table::probeHash(u64 hash)
		{
			unsigned long long index = hash & mask;
			TransCluster* cluster = &table[index];
			HashLower hLower = HASHLOWER(hash);
			for (int i = 0; i < BUCKET_SIZE; ++i) {
				if (cluster->entrys[i].hashLower == hLower) {
					cluster->entrys[i].ancient = false;
					return { true, &cluster->entrys[i] };
				}
			}

			TransEntry* replace = &cluster->entrys[0];
			for (int i = 1; i < BUCKET_SIZE; ++i) {
				if (replace->depth > cluster->entrys[i].depth || (!replace->ancient && cluster->entrys[i].ancient) || replace->bound < cluster->entrys[i].bound) {
					replace = &cluster->entrys[i];
				}
			}

			return { false, replace };
			
		}

		void Transposition_Table::setAncient()
		{
			for (int i = 0; i < size; ++i) {
				for (int j = 0; j < BUCKET_SIZE; ++j) {
					this->table[i].entrys[j].ancient = true;
				}
			}
		}


	}
}