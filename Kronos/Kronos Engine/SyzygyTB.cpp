#include "SyzygyTB.h"

#include <fstream>
#include <algorithm>

#include "../Syzygy endgame tablebases/tbprobe.h"

namespace KRONOS
{
	namespace SEARCH
	{
		namespace SYZYGY
		{

			bool syzygyInitialised = false;
			int SYZYGYLargest = 0;

			bool initSYZYGY(const char* filePath)
			{
				syzygyInitialised = tb_init(filePath);
				if (syzygyInitialised) {
					SYZYGYLargest = int(TB_LARGEST);
					return true;
				}
				return false;
			}

			void freeSYZYGY()
			{
				tb_free();
			}

			inline unsigned convertToTBCastling(const Position* position) {
				unsigned castling = 0;
				if (position->status.WKcastle)
					castling |= 1;
				if (position->status.WQcastle)
					castling |= 2;
				if (position->status.BKcastle)
					castling |= 4;
				if (position->status.BQcastle)
					castling |= 8;
				return castling;
			}

			int resultToWDL(unsigned result) {
				if (result == TB_LOSS)
					return (int)SyzygyResult::SYZYGY_LOSS;
				else if (result == TB_BLESSED_LOSS || result == TB_DRAW || result == TB_CURSED_WIN)
					return (int)SyzygyResult::SYZYGY_DRAW;
				else if (result == TB_WIN)
					return (int)SyzygyResult::SYZYGY_WIN;
				else if (result == TB_RESULT_FAILED)
					return (int)SyzygyResult::SYZYGY_FAIL;
				return -1;
			}

			inline Move convertToKronosMove(unsigned tbMove, const Position* position)
			{
				int from = TB_GET_FROM(tbMove);
				int to = TB_GET_TO(tbMove);
				int flag = 0;
				int pieceType = 0;

				switch (TB_GET_PROMOTES(tbMove)) {
				case TB_PROMOTES_QUEEN:
					flag |= QUEEN_PROMOTION;
					break;
				case TB_PROMOTES_ROOK:
					flag |= ROOK_PROMOTION;
					break;
				case TB_PROMOTES_BISHOP:
					flag |= BISHOP_PROMOTION;
					break;
				case TB_PROMOTES_KNIGHT:
					flag |= KNIGHT_PROMOTION;
					break;
				}

				if (getTileBB(to) & position->board.occupied[BOTH]) {
					flag |= CAPTURE;
				}
				
				for (int p = 0; p < 6; p++) {
					if (getTileBB(from) & position->board.pieceLocations[position->status.isWhite][p]) {
						pieceType = p;
						break;
					}
				}

				if (pieceType == PAWN) {
					if (to == position->status.EP)
						flag = ENPASSANT;
				}
				else if (pieceType == KING) {
					if (from == E1)
						if (to == G1)
							flag = KING_CASTLE;
						else if (to == C1)
							flag = QUEEN_CASTLE;
					else if (from == E8)
						if (to == G8)
							flag = KING_CASTLE;						
						else if (to == C8)
							flag = QUEEN_CASTLE;				
				}

				return Move(from, to, flag, pieceType);

			}

			inline int probeWDL(const Position* position)
			{
				if (populationCount(position->board.occupied[BOTH]) > SYZYGYLargest 
					|| position->halfMoves < 100 
					|| (!position->status.WKcastle && !position->status.WQcastle && !position->status.BKcastle && !position->status.BQcastle))
					return (int)SyzygyResult::SYZYGY_FAIL;

				unsigned result = tb_probe_wdl(
					position->board.occupied[WHITE],
					position->board.occupied[BLACK],
					(position->board.pieceLocations[WHITE][KING] | position->board.pieceLocations[BLACK][KING]),
					(position->board.pieceLocations[WHITE][QUEEN] | position->board.pieceLocations[BLACK][QUEEN]),
					(position->board.pieceLocations[WHITE][ROOK] | position->board.pieceLocations[BLACK][ROOK]),
					(position->board.pieceLocations[WHITE][BISHOP] | position->board.pieceLocations[BLACK][BISHOP]),
					(position->board.pieceLocations[WHITE][KNIGHT] | position->board.pieceLocations[BLACK][KNIGHT]),
					(position->board.pieceLocations[WHITE][PAWN] | position->board.pieceLocations[BLACK][PAWN]),
					position->halfMoves,
					convertToTBCastling(position),
					position->status.EP == no_Tile ? 0 : unsigned(position->status.EP),
					position->status.isWhite
					);

				return resultToWDL(result);
			}

			inline int probeDTZ(const Position* position, Move* bestMove)
			{
				unsigned results[TB_MAX_MOVES];
				
				if (populationCount(position->board.occupied[BOTH]) > TB_LARGEST)
					return (int)SyzygyResult::SYZYGY_FAIL;

				unsigned result = tb_probe_root(
					position->board.occupied[WHITE],
					position->board.occupied[BLACK],
					(position->board.pieceLocations[WHITE][KING]   | position->board.pieceLocations[BLACK][KING]),
					(position->board.pieceLocations[WHITE][QUEEN]  | position->board.pieceLocations[BLACK][QUEEN]),
					(position->board.pieceLocations[WHITE][ROOK]   | position->board.pieceLocations[BLACK][ROOK]),
					(position->board.pieceLocations[WHITE][BISHOP] | position->board.pieceLocations[BLACK][BISHOP]),
					(position->board.pieceLocations[WHITE][KNIGHT] | position->board.pieceLocations[BLACK][KNIGHT]),
					(position->board.pieceLocations[WHITE][PAWN]   | position->board.pieceLocations[BLACK][PAWN]),
					position->halfMoves,
					convertToTBCastling(position),
					position->status.EP == no_Tile ? 0 : position->status.EP,
					position->status.isWhite,
					results
				);

				if (result == TB_RESULT_STALEMATE || result == TB_RESULT_CHECKMATE || result == TB_RESULT_FAILED)
					return (int)SyzygyResult::SYZYGY_FAIL;

				*bestMove = convertToKronosMove(result, position);

				return resultToWDL(result);

			}

		} // namespace SYZYGY
	} // namespace SEARCH
} // namespace KRONOS