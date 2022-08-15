#include "Transposition_Table.h"

#include "consts.h"

namespace KRONOS {
	namespace HASH {

		bool Transposition_Table::probe(u64 hash, transEntry& entry)
		{
			transEntry* ety = &getEntry(hash)->bucket[0];
			HashLower h =lock(hash);
			for (int t = BUCKET_SIZE; t--; ++ety) {
				if (ety->hashLock == h) {
					entry = *ety;
					entry.setAge(currentAge);
					return true;
				}
			}
			return false;
		}

		void Transposition_Table::saveEntry(u64 hash, u16 move, int depth, int16_t eval, int bound)
		{
			int highest = -INFINITE;
			transEntry* entry = &getEntry(hash)->bucket[0];
			transEntry* replace = entry;
			for (int t = BUCKET_SIZE; t--; ++entry) {
				if (entry->hashLock == lock(hash)) {
					if (bound == (int)BOUND::EXACT || depth >= entry->depth) {
						replace = entry;
						break;
					}
					else return;
				}
				int score = (((64 + currentAge - entry->getAge()) % 64) << 8) - entry->depth;
				if (score > highest) {
					highest = score;
					replace = entry;
				}
			}
			replace->hashLock = lock(hash);
			replace->move = move;
			replace->depth = depth;
			replace->eval = eval;
			replace->setAgeBound(currentAge, bound);
		}

		void ABDADA_TABLE::setBusy(u64 hash, u16 move, int depth)
		{
			int lowest = INFINITE;
			MOVE_HASH* entry = &getEntry(hash)->bucket[0];
			MOVE_HASH* replace = entry;
			for (int t = ABDADA_BUCKET_SIZE; t--; ++entry) {
				if (entry->depth == depth && entry->hashLock == mhlock(hash) && entry->move == move) {
					return;
				}
				if (entry->depth < lowest) {
					lowest = entry->depth;
					replace = entry;
				}
			}
			replace->hashLock = mhlock(hash);
			replace->move = move;
			replace->depth = depth;
		}

		void ABDADA_TABLE::resetBusy(u64 hash, u16 move, int depth)
		{
			MOVE_HASH* entry = &getEntry(hash)->bucket[0];
			for (int t = ABDADA_BUCKET_SIZE; t--; ++entry) {
				if (entry->depth == depth && entry->hashLock == mhlock(hash) && entry->move == move) {
					entry->hashLock = 0;
					entry->depth = 0;
					entry->move = 0;
					return;
				}
			}
		}

		bool ABDADA_TABLE::isBusy(u64 hash, u16 move, int depth) {
			MOVE_HASH* entry = &getEntry(hash)->bucket[0];
			for (int t = ABDADA_BUCKET_SIZE; t--; ++entry) {
				if (entry->depth == depth && entry->hashLock == mhlock(hash) && entry->move == move) {
					return true;
				}
			}
			return false;
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