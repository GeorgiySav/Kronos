#include "Eval_Params.h"
#include "Board.h"

#include <iostream>
#include <fstream>

namespace KRONOS {
	
	namespace EVALUATION {
		
		namespace PARAMS {
			
			Eval_Parameters::Eval_Parameters()
			{
				
				for (int tile = 0; tile < 64; tile++) {

					int bTile = ((7 - std::floor(tile / 8)) * 8) + (tile % 8);

					this->PST[WHITE][PAWN][tile] = PARAMS::pawnPST[tile];
					this->PST[WHITE][KNIGHT][tile] = PARAMS::knightPST[tile];
					this->PST[WHITE][BISHOP][tile] = PARAMS::bishopPST[tile];
					this->PST[WHITE][ROOK][tile] = PARAMS::rookPST[tile];
					this->PST[WHITE][QUEEN][tile] = PARAMS::queenPST[tile];
					this->PST[WHITE][KING][tile] = PARAMS::kingPST[tile];

					this->PST[BLACK][PAWN][bTile] = PARAMS::pawnPST[tile];
					this->PST[BLACK][KNIGHT][bTile] = PARAMS::knightPST[tile];
					this->PST[BLACK][BISHOP][bTile] = PARAMS::bishopPST[tile];
					this->PST[BLACK][ROOK][bTile] = PARAMS::rookPST[tile];
					this->PST[BLACK][QUEEN][bTile] = PARAMS::queenPST[tile];
					this->PST[BLACK][KING][bTile] = PARAMS::kingPST[tile];

				}

				initMatTab();

			}

			Eval_Parameters::~Eval_Parameters()
			{
			}
			
			void Eval_Parameters::clearParameters()
			{
				// set all variables to 0
				this->PAWN_VALUE = SCORE_ZERO;
				this->KNIGHT_VALUE = SCORE_ZERO;
				this->BISHOP_VALUE = SCORE_ZERO;
				this->ROOK_VALUE = SCORE_ZERO;
				this->QUEEN_VALUE = SCORE_ZERO;
				this->KING_VALUE = SCORE_ZERO;

				this->CONNECTED_PAWN_VALUE = SCORE_ZERO;
				this->DOUBLED_PAWN_VALUE = SCORE_ZERO;
				this->ISOLATED_PAWN_VALUE = SCORE_ZERO;
				this->BACKWARD_PAWN_VALUE = SCORE_ZERO;
				this->PASSED_ISOLATED_PAWN_VALUE = SCORE_ZERO;
				this->PASSED_BACKWARD_PAWN_VALUE = SCORE_ZERO;

				for (int i = 0; i < 8; i++) {
					this->PASSER_DIST_ALLY[i] = SCORE_ZERO;
					this->PASSER_DIST_ENEMY[i] = SCORE_ZERO;
					this->PASSER_BONUS[i] = SCORE_ZERO;
					this->UNBLOCKED_PASSER[i] = SCORE_ZERO;
					this->PASSER_SAFE_PUSH[i] = SCORE_ZERO;
					this->PASSER_SAFE_PROM[i] = SCORE_ZERO;
				}

				for (int tile = 0; tile < 64; tile++) {
					this->PST[WHITE][PAWN][tile] = SCORE_ZERO;
					this->PST[WHITE][KNIGHT][tile] = SCORE_ZERO;
					this->PST[WHITE][BISHOP][tile] = SCORE_ZERO;
					this->PST[WHITE][ROOK][tile] = SCORE_ZERO;
					this->PST[WHITE][QUEEN][tile] = SCORE_ZERO;
					this->PST[WHITE][KING][tile] = SCORE_ZERO;

					this->PST[BLACK][PAWN][tile] = SCORE_ZERO;
					this->PST[BLACK][KNIGHT][tile] = SCORE_ZERO;
					this->PST[BLACK][BISHOP][tile] = SCORE_ZERO;
					this->PST[BLACK][ROOK][tile] = SCORE_ZERO;
					this->PST[BLACK][QUEEN][tile] = SCORE_ZERO;
					this->PST[BLACK][KING][tile] = SCORE_ZERO;
				}
				
				for (int p = 0; p < 6; p++) {
					for (int i = 0; i < 28; i++) {
						this->MOBILITY_BONUS[p] = SCORE_ZERO;
					}
				}
				
				for (int i = 0; i < 5; i++) {
					ATK_ON_KING_WEIGHT[i] = 0;
				}

				this->BISHOP_PAWN_PENALTY = SCORE_ZERO;

				this->ROOK_SEMI_OPEN_FILE_BONUS = SCORE_ZERO;
				this->ROOK_OPEN_FILE_BONUS = SCORE_ZERO;
				
				this->FLANK_ATTACKS = SCORE_ZERO;
				this->PAWNLESS_FLANK = SCORE_ZERO;

				this->THREAT_PAWN_PUSH_VALUE = SCORE_ZERO;
				this->THREAT_WEAK_PAWNS_VALUE = SCORE_ZERO;
				this->THREAT_PAWNSxMINORS_VALUE = SCORE_ZERO;
				this->THREAT_MINORSxMINORS_VALUE = SCORE_ZERO;
				this->THREAT_MAJORSxWEAK_MINORS_VALUE = SCORE_ZERO;
				this->THREAT_PAWN_MINORSxMAJORS_VALUE = SCORE_ZERO;
				this->THREAT_ALLxQUEENS_VALUE = SCORE_ZERO;
				this->THREAT_KINGxMINORS_VALUE = SCORE_ZERO;
				this->THREAT_KINGxROOKS_VALUE = SCORE_ZERO;

				this->PIECE_SPACE = SCORE_ZERO;
				this->EMPTY_SPACE = SCORE_ZERO;

				this->PAWN_PHASE = 0;
				this->KNIGHT_PHASE = 0;
				this->BISHOP_PHASE = 0;
				this->ROOK_PHASE = 0;
				this->QUEEN_PHASE = 0;
				this->TEMPO = 0;
			}

			void Eval_Parameters::saveParams()
			{
				time_t now = time(0);
				std::string dt = ctime(&now);
				dt.erase(remove(dt.begin(), dt.end(), '\n'), dt.end());
				dt.erase(remove(dt.begin(), dt.end(), ' '), dt.end());
				dt.erase(remove(dt.begin(), dt.end(), ':'), dt.end());
				dt = "../Kronos/Eval Parameters/" + dt + ".txt";
				std::ofstream outFile(dt, std::ios::app);
				if (outFile.fail())
					return;
				outFile.write((char*)this, sizeof(*this));
				outFile.close();
			}

			void Eval_Parameters::loadParams(std::string filePath)
			{
				std::ifstream inFile;
				inFile.open(filePath, std::ios::in);
				if (inFile.fail())
					return;
				inFile.seekg(0);
				inFile.read((char*)this, sizeof(*this));
				inFile.close();
			}

		} // PARAMS

	} // EVALUATION

} // KRONOS