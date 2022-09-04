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