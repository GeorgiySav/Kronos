#include "Transposition_Table.h"

#include "consts.h"

namespace KRONOS {
	namespace HASH {

		bool Transposition_Table::probe(u64 hash, transEntry& entry)
		{
			HashLower h =lock(hash);
			
			for (int i = 0; i < BUCKET_SIZE; i++) {
				transEntry& ety = getEntry(hash)->bucket[i];
				if (ety.hashLock == h) {
					entry = ety;
					entry.setAge(currentAge);
					return true;
				}
			}

			return false;
		}

		void Transposition_Table::saveEntry(u64 hash, u16 move, int depth, int16_t eval, int bound)
		{
			int highest = -INFINITE;
			transEntry& replace = getEntry(hash)->bucket[0];
			
			for (int i = 0; i < BUCKET_SIZE; i++) {
				transEntry& entry = getEntry(hash)->bucket[i];
				if (entry.hashLock == lock(hash)) {
					if (entry.depth >= depth || bound == (int)BOUND::EXACT) {
						replace = entry;
						break;
					}
					else return; // don't want to replace an entry with a worse one
				}
				int score = (((64 + currentAge - entry.getAge()) % 64) << 8) - entry.depth;
				if (score > highest) {
					highest = score;
					replace = entry;
				}
			}

			replace.hashLock = lock(hash);
			replace.move = move;
			replace.depth = depth;
			replace.eval = eval;
			replace.setAgeBound(currentAge, bound);
		}

		int16_t Eval_Table::getEval(Position& position, EVALUATION::Evaluation& evaluation)
		{
			HashLower hash = lock(position.hash);
			evalEntry* replace = &getEntry(position.hash)->bucket[0];
			for (int i = 0; i < 5; i++) {
				evalEntry& entry = getEntry(position.hash)->bucket[i];
				if (entry.hashLock == hash) {
					return entry.eval;
				}
			}
			replace->hashLock = hash;
			replace->eval = evaluation.evaluate(position);
			return replace->eval;
		}

		void ABDADA_TABLE::setBusy(u64 hash, u16 move, int depth)
		{
			int lowest = INFINITE;
			MOVE_HASH& replace = getEntry(hash)->bucket[0];

			for (int i = 0; i < ABDADA_BUCKET_SIZE; i++) {
				MOVE_HASH& entry = getEntry(hash)->bucket[i];
				if (entry.depth == depth && entry.hashLock == mhlock(hash) && entry.move == move) {
					return;
				}
				if (entry.depth < lowest && entry.hashLock == mhlock(hash)) {
					lowest = entry.depth;
					replace = entry;
				}
			}

			replace.hashLock = mhlock(hash);
			replace.move = move;
			replace.depth = depth;
		}

		void ABDADA_TABLE::resetBusy(u64 hash, u16 move, int depth){
			for (int i = 0; i < ABDADA_BUCKET_SIZE; i++) {
				MOVE_HASH& entry = getEntry(hash)->bucket[i];
				if (entry.depth == depth && entry.hashLock == mhlock(hash) && entry.move == move) {
					entry.hashLock = 0;
					entry.depth = 0;
					entry.move = 0;
				}
			}
		}

		bool ABDADA_TABLE::isBusy(u64 hash, u16 move, int depth) {
			for (int i = 0; i < ABDADA_BUCKET_SIZE; i++) {
				MOVE_HASH& entry = getEntry(hash)->bucket[i];
				if (entry.depth == depth && entry.hashLock == mhlock(hash) && entry.move == move) {
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