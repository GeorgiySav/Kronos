#include "Move_Picker.h"

#include "Search.h"

namespace KRONOS {
	namespace SEARCH {

		Move_Picker::Move_Picker(Position& position, bool isQuie, Move hashMove, Move k1, Move k2) 
			: position(position), quiescenceSearch(isQuie), hashMove(hashMove), killer1(k1), killer2(k2) {
			generateMoves(position.status.isWhite, position.board, position.status, moves);
			if (hashMove == NULL_MOVE)
				stage = STAGE_FILTER_MOVES;
			else
				stage = STAGE_HASH_MOVE;
		}

		Move_Picker::~Move_Picker() {

		}

		int Move_Picker::MVV_LVA(Move& move) {
			static const int PieceValues[5] = { 100, 300, 350, 500, 900 };
			return (PieceValues[position.getPieceType(move.to)]) - PieceValues[move.moved_Piece];
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
				std::vector<int> tacticalScores;
				tacticalScores.reserve(10);
				std::vector<int> quietScores;
				quietScores.reserve(20);
				int currentScore = 0;
				for (int i = 0; i < moves.size(); i++) {
					if (moves.at(i).isTactical()) {
						if (moves.at(i).flag & CAPTURE)
							currentScore = MVV_LVA(moves.at(i));
						if (moves.at(i).flag & PROMOTION) {
							switch (moves.at(i).flag & 0b1011)
							{
							case (KNIGHT_PROMOTION):
								currentScore = 1001;
								break;
							case (BISHOP_PROMOTION):
								currentScore = 999;
								break;
							case (ROOK_PROMOTION):
								currentScore = 1000;
								break;
							case (QUEEN_PROMOTION):
								currentScore = 1002;
								break;
							}
						}
						// insert it into the array where it is sorted in descending order
						int j = 0;
						while (j < tacticals.size() && currentScore < tacticalScores[j])
							j++;
						tacticals.insert(moves.at(i), j);
						tacticalScores.insert(tacticalScores.begin() + j, currentScore);
					}
					else {
						currentScore = sData.getHistoryValue(position.status.isWhite, moves.at(i));

						int j = 0;
						while (j < quiets.size() && currentScore < quietScores[j])
							j++;
						quiets.insert(moves.at(i), j);
						quietScores.insert(quietScores.begin() + j, currentScore);
					}
				}
			}
				index = 0;
			case (STAGE_WINNING_TACTICALS):
				while (index < tacticals.size()) {
					Move& move = tacticals.at(index++);
					if (move == hashMove)
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
			case (STAGE_KILLER_1):
				stage++;
				if (!quiescenceSearch) {
					if (killer1 != NULL_MOVE
						&& killer1 != hashMove
						&& quiets.contains(killer1)) {
						nextMove = killer1;
						return true;
					}
				}
			case (STAGE_KILLER_2):
				stage++;
				if (!quiescenceSearch) {
					if (killer2 != NULL_MOVE
						&& killer2 != hashMove
						&& quiets.contains(killer2)) {
						nextMove = killer2;
						return true;
					}
				}
			case (STAGE_QUIETS):
				if (!quiescenceSearch) {
					while (index < quiets.size()) {
						Move& move = quiets.at(index++);
						if (move == hashMove || move == killer1 || move == killer2)
							continue;
						nextMove = move;
						return true;
					}
				}
				stage++;
				index = 0;
			case (STAGE_LOSING_TACTICALS) :
				while (index < badTacticals.size()) {
					Move& move = badTacticals.at(index++);
					if (move == hashMove)
						continue;
					nextMove = move;
					return true;
				}
				stage++;
			case (STAGE_FINISHED):
				return false;
			}
			return false;
		}

	} // SEARCH
} // KRONSO