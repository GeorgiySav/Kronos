#include "Move_Ordering.h"

#include "consts.h"

namespace KRONOS
{
	namespace SEARCH
	{

		inline void sortMoves(Position* position, std::vector<Move>* moves, Move HashMove)
		{
			
			static int16_t pieceScores[] = {
				100, 300, 325, 500, 900, 1000
			};

			// push the move from previous interations to the start of the list as that is the best known move atm
			if (HashMove != NULL_MOVE)
			{
				auto hmIndex = std::find(moves->begin(), moves->end(), HashMove);
				if (hmIndex != moves->end())
					std::swap(moves->at(0), moves->at(hmIndex - moves->begin()));
				else
					assert(true);
			}
			
			/*
				1. Hash move from hash tables
				2. Winning captures/promotions
				3. Equal captures/promotions
				4. Killer moves (non capture), often with mate killers first
				5. Non-captures sorted by history heuristic and that like
				6. Losing captures
			*/

			// to calculate this, we will do victim - agressor
			// rook captured by a pawn  - 5-1= 4
			// pawn captured by a queen - 1-9=-8

			// gives a score to each move, and once all moves have been rated, the vector will be
			// sorted according to the scores
			std::vector<int> moveScore(moves->size());

			for (int index = 1; index < moves->size(); index++)
			{
				int score = 0;
				// assign a score based on the MVV-LVA algorithm
				if (moves->at(index).flag & CAPTURE)
				{
					score = pieceScores[position->getPieceType(moves->at(index).to)] - pieceScores[moves->at(index).moved_Piece];
				}
				else
				{

				}
				
				if (moves->at(index).flag & PROMOTION)
				{
					score += 1000;
				}
			}
			
			// bubble sort the moves based on the scores	
			for (int index = 1; index < moves->size(); index++)
			{
				for (int index2 = 1; index2 < moves->size() - 1; index2++)
				{
					if (moveScore[index2] < moveScore[index2 + 1])
					{
						std::swap(moveScore[index2], moveScore[index2 + 1]);
						std::swap(moves->at(index2), moves->at(index2 + 1));
					}
				}
			}

		}
	} // SEARCH
} // KRONOS