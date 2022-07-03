#include "Transposition_Table.h"

#include "consts.h"

namespace KRONOS {
	namespace HASH {

		Transposition_Table::~Transposition_Table()
		{
			if (table)
				delete[] table;
		}

		void Transposition_Table::setSize(int sizeMb)
		{
			size = (sizeMb * 1024 * 1024) / sizeof(TransCluster);
			mask = size - 1;
			if (table) 
				delete[] table;
			table = new TransCluster[size];
			if (table == nullptr) {
				std::cout << "Error: Cannot allocate memory for transposition table" << std::endl;
				return;
			}
			clean();
		}

		void Transposition_Table::clean()
		{
			memset(table, 0, sizeof(table));
			generation = 0;
		}

		void TransEntry::saveEntry(uint64_t hash, Move move, int depth, int score, int static_eval, uint8_t flag, uint8_t generation)
		{
			HashLower hL = HASHLOWER(hash);

			if (move != Move() || hL != this->hashLower)
				this->bestMove = move.toIntMove();

			if (flag == (u8)HASH::BOUND::EXACT || hL != this->hashLower || depth > this->depth - 1)
			{
				this->hashLower = hL;
				this->depth = depth;
				this->eval = score;
				this->static_eval = static_eval;
				this->ageFlag = (generation << 2) | flag;
			}

		}
		
		void Transposition_Table::startSearch()
		{
			generation = (generation + 1) % 64;
		}


		/* returns a pair of a bool and a pointer to a transposition entry. If the boolean is true,
		   that means that the search should use this entry, otherwise it should replace that entry */
		std::pair<bool, TransEntry*> Transposition_Table::probeHash(u64 hash)
		{
			unsigned long long index = hash & mask;
			TransCluster* cluster = &table[index];
			HashLower hLower = HASHLOWER(hash);
			
			for (int i = 0; i < BUCKET_SIZE; i++) {
				if (cluster->entries[i].hashLower == 0)
					return { false, &cluster->entries[i] };
				if (hLower == cluster->entries[i].hashLower) {
					cluster->entries[i].ageFlag = (generation << 2) | cluster->entries[i].flag();
					return { true, &cluster->entries[i] };
				}
			}

			TransEntry* replacement = &cluster->entries[0];
			for (int i = 1; i < BUCKET_SIZE; i++)
				if (cluster->entries[i].depth - cluster->entries[i].ageDiff(generation) * 16 
				  < replacement->depth - replacement->ageDiff(generation) * 16)
					replacement = &cluster->entries[i];		

			return { false, replacement };

		}

		int ScoreToTranpositionTable(int score, u8 ply) {
			if (score >= MATE_IN_MAX_PLY) {
				return score + ply;
			}
			else if (score <= MATED_IN_MAX_PLY) {
				return score - ply;
			}
			else {
				return score;
			}
		}

		int TranspositionTableToScore(int score, u8 ply) {
			if (score >= MATE_IN_MAX_PLY) {
				return score - ply;
			}
			else if (score <= MATED_IN_MAX_PLY) {
				return score + ply;
			}
			else {
				return score;
			}
		}


	}
}