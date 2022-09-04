#include "Move_Picker.h"

#include "consts.h"

namespace KRONOS
{
	namespace SEARCH
	{
		MOVE_PICKER::MOVE_PICKER(const Position* pos, bool isQui, int margin, Search_Thread* t, Move hMove, Move killer1, Move killer2)
		: position(pos), threshold(margin), HashMove(hMove), killer1(killer1), killer2(killer2), thread(t) {
			generateMoves(position->status.isWhite, position->board, position->status, moves);
			skipQuiets = isQui;
			stage = STAGE_HASH_MOVE;
		}

		MOVE_PICKER::~MOVE_PICKER() { }

		int MOVE_PICKER::MVVLVA(Move& move) {
			return (PieceValues[position->getPieceType(move.to)]) + ((move.flag & PROMOTION) - 7) - PieceValues[move.moved_Piece];
		}

		void MOVE_PICKER::rateCaptures() {
			for (int i = 0; i < captures.size; i++) {
				values[i] = MVVLVA(captures.at(i));
			}

			for (int i = 0; i < captures.size; i++) {
				for (int j = i + 1; j < captures.size; j++) {
					if (values[i] < values[j]) {
						std::swap(values[i], values[j]);
						std::swap(captures.at(i), captures.at(j));
					}
				}
			}
		}

		void MOVE_PICKER::rateQuiets() {
			for (int i = 0; i < quiets.size; i++) {
				quietValues[i] = thread->history[position->status.isWhite][quiets.at(i).moved_Piece][quiets.at(i).to];
			}
			for (int i = 0; i < quiets.size; i++) {
				for (int j = i + 1; j < quiets.size; j++) {
					if (quietValues[i] < quietValues[j]) {
						std::swap(quietValues[i], quietValues[j]);
						std::swap(quiets.at(i), quiets.at(j));
					}
				}
			}
		}

		bool MOVE_PICKER::getMove(Move& move)
		{
			switch (stage)
			{
			case STAGE_HASH_MOVE:
				stage++;
				if (HashMove != NULL_MOVE &&
					moves.contains(HashMove)) {
					move = HashMove;
					return true;
				}
			case STAGE_FILTER_MOVES:
				stage++;
				while (index < moves.size) {
					Move& tMove = moves.at(index++);
					if (tMove.flag & CAPTURE || tMove.flag & PROMOTION) {
						// i@irfy.co.uk 
						captures.add(tMove);
					}
					else {
						quiets.add(tMove);
					}
				}
				rateCaptures();
				index = 0;
			case STAGE_WINNING_CAPTURES:
				while (index < captures.size) {
					Move& tMove = captures.at(index++);
					if (tMove == HashMove) continue;
					if (!position->SEE(tMove, threshold)) {
						badCaptures.add(tMove);
						continue;
					}
					move = tMove;
					return true;
				}
				stage++;
			case STAGE_KILLER_1:
				stage++;
				if (!skipQuiets && killer1 != NULL_MOVE && killer1 != HashMove && quiets.contains(killer1)) {
					move = killer1;
					return true;
				}
			case STAGE_KILLER_2:
				stage++;
				if (!skipQuiets && killer2 != NULL_MOVE && killer2 != HashMove && quiets.contains(killer2)) {
					move = killer2;
					return true;
				}
				index = 0;
			case STAGE_RATE_QUIETS:
				stage++;
				rateQuiets();
			case STAGE_QUIETS:
				if (!skipQuiets) {
					while (index < quiets.size) {
						Move& tMove = quiets.at(index++);
						if (tMove == HashMove || tMove == killer1 || tMove == killer2) continue;
						move = tMove;
						return true;
					}
				}
				stage++;
				index = 0;
			case STAGE_LOSING_CAPTURES:
				while (index < badCaptures.size) {
					Move& tMove = badCaptures.at(index++);
					if (tMove == HashMove) continue;
					move = tMove;
					return true;
				}
				if (skipQuiets) return false;
				stage++;
				index = 0;
			case STAGE_DEFERRED:
				while (index < deferredMoves.size) {
					Move& tMove = deferredMoves.at(index++);
					move = tMove;
					return true;
				}
				stage++;
			case STAGE_FINISHED:
				return false;
			}
			return false;
		}
		
	} // SEARCH
} // KRONOS