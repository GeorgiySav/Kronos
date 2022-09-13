#include "Move_Picker.h"

namespace KRONOS {
	namespace SEARCH {

		Move_Picker::Move_Picker(Position& position, bool isQuie, Move hashMove) : position(position), quiescenceSearch(isQuie), hashMove(hashMove) {
			generateMoves(position.status.isWhite, position.board, position.status, moves);
			if (hashMove == NULL_MOVE)
				stage = STAGE_FILTER_MOVES;
			else
				stage = STAGE_HASH_MOVE;
		}

		Move_Picker::~Move_Picker() {

		}

		int Move_Picker::MVV_LVA(Move& move) {
			static const int PieceValues[5] = { 100, 300, 300, 500, 900 };
			return (PieceValues[position.getPieceType(move.to)]) + ((move.flag & PROMOTION) - 7) - PieceValues[move.moved_Piece];
		}

		bool Move_Picker::nextMove(Search_Thread& sData, Move& nextMove)
		{
			switch (stage)
			{
			case (STAGE_HASH_MOVE):
				stage++;
				if (moves.contains(hashMove)) {
					nextMove = hashMove;
					return true;
				}

			case (STAGE_FILTER_MOVES):
			{
				stage++;
				int tacticalScores[64] = { 0 };
				int currentScore = 0;
				for (int i = 0; i <= moves.size; i++) {
					if (moves.at(i).flag & (CAPTURE | PROMOTION)) {
						if (!(moves.at(i).flag & CAPTURE)) // pure promotion
							currentScore = 1000;
						else
							currentScore = MVV_LVA(moves.at(i));
						// insert it into the array where it is sorted in descending order
						if (tacticals.size) {
							int j = 0;
							while (j <= tacticals.size && currentScore > tacticalScores[j])
								j++;
							tacticals.insert(moves.at(i), j);
							for (int k = tacticals.size - 1; k >= j; k--)
								tacticalScores[k + 1] = tacticalScores[k];
							tacticalScores[j] = currentScore;
						}
						else {
							tacticals.add(moves.at(i));
							tacticalScores[0] = currentScore;
						}
					}
					else {
						quiets.add(moves.at(i));
					}
				}
			}
				index = 0;
			case (STAGE_WINNING_TACTICALS):
				while (index < tacticals.size) {
					Move& move = tacticals.at(index++);
					if (hashMove == move)
						continue;
					if (!position.SEE_GE(move, 0)) {
						badTacticals.add(move);
						continue;
					}
					nextMove = move;
					return true;
				}
				stage++;
				index = 0;
			case (STAGE_QUIETS):
				if (!quiescenceSearch) {
					while (index < quiets.size) {
						Move& move = quiets.at(index++);
						if (hashMove == move)
							continue;
						nextMove = move;
						return true;
					}
				}
				stage++;
				index = 0;
			case (STAGE_LOSING_TACTICALS) :
				while (index < badTacticals.size) {
					Move& move = badTacticals.at(index++);
					if (hashMove == move)
						continue;
					nextMove = move;
					return true;
				}
				stage++;
			case (STAGE_FINISHED):
				return false;
			}
		}

	} // SEARCH
} // KRONSO