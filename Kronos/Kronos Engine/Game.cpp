#include "Game.h"

#include "consts.h"
#include "Zobrist_Hashing.h"
#include "FEN.h"
#include "AlgMove.h"

namespace KRONOS
{

	Game::Game()
	{
		this->clear();
	}

	Game::~Game()
	{
		
	}

	void Game::checkGameState()
	{
		// check to see if the game has ended through checkmate or stalemate
		moves.clear();
		generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, moves);

		if (moves.size() == 0) {
			if (inCheck(positions[ply])) {
				gameState = positions[ply].status.isWhite ? GAME_STATE::BLACK_DEALT_CHECKMATE : GAME_STATE::WHITE_DEALT_CHECKMATE;
			}
			else {
				gameState = GAME_STATE::DRAW_TO_STALEMATE;
			}
			return;
		}
		
		// check to see if the game has ended through repitition
		if (ply >= 4) {
			int count = 0;
			u64 compHash = positions.at(ply).hash;
			int index = ply - 1;

			while (index >= 0) {
				if (positions.at(index).hash == compHash) {
					count = count + 1;
					if (count >= 2) {
						gameState = GAME_STATE::DRAW_TO_REPETITION;
						return;
					}
				}
				index = index - 1;
			}
		}

		// check to see if the game has ended through 50 move rule
		if (positions[ply].halfMoves >= 100)
		{
			gameState = GAME_STATE::DRAW_TO_50_MOVE;
			return;
		}

		// check to see if the game has ended through insufficient material
		int numPieces = populationCount(positions[ply].board.occupied[BOTH]);
		if (numPieces == 2) {
			gameState = GAME_STATE::DRAW_TO_LACK_OF_MATERIAL;
			return;
		}
		else if (numPieces == 3) {
			if ((positions[ply].board.pieceLocations[WHITE][KNIGHT] | positions[ply].board.pieceLocations[BLACK][KNIGHT])
			 || (positions[ply].board.pieceLocations[WHITE][BISHOP] | positions[ply].board.pieceLocations[BLACK][BISHOP])) {
				gameState = GAME_STATE::DRAW_TO_LACK_OF_MATERIAL;
				return;
			}
		}
		else if (numPieces == 4) {
			// check if both sides have equal number of pieces
			int whiteNum = populationCount(positions[ply].board.occupied[WHITE]);
			int blackNum = populationCount(positions[ply].board.occupied[BLACK]);
			
			if (whiteNum == blackNum) {
				/*
				    this means that both sides have a king and another piece
					game will end by insufficient material if both sides have either a:
						knight or
						bishop
				*/
				if ((positions[ply].board.pieceLocations[WHITE][KNIGHT] | positions[ply].board.pieceLocations[WHITE][BISHOP]) &&
					(positions[ply].board.pieceLocations[BLACK][KNIGHT] | positions[ply].board.pieceLocations[BLACK][BISHOP])) {
					gameState = GAME_STATE::DRAW_TO_LACK_OF_MATERIAL;
					return;
				}
			}
			else {
				/*
					a draw from insufficient material can also happen if one side has two bishops
					it is possible to checkmate with two knights, however it is very unlikely
				*/
				if (populationCount(positions[ply].board.pieceLocations[WHITE][BISHOP]) == 2
				 || populationCount(positions[ply].board.pieceLocations[BLACK][BISHOP]) == 2
				 || (positions[ply].board.pieceLocations[WHITE][BISHOP] && positions[ply].board.pieceLocations[WHITE][KNIGHT])
				 || (positions[ply].board.pieceLocations[BLACK][BISHOP] && positions[ply].board.pieceLocations[BLACK][KNIGHT])) {
					gameState = GAME_STATE::DRAW_TO_LACK_OF_MATERIAL;
					return;
				}
			}
		}
	}

	void Game::calculateMaterial()
	{
		static int materialScores[] = {
			1, 3, 3, 5, 9
		};
		int whiteScore = 0;
		whiteScore += materialScores[PAWN] * (populationCount(positions.at(ply).board.pieceLocations[WHITE][PAWN]) - populationCount(positions.at(ply).board.pieceLocations[BLACK][PAWN]));
		whiteScore += materialScores[KNIGHT] * (populationCount(positions.at(ply).board.pieceLocations[WHITE][KNIGHT]) - populationCount(positions.at(ply).board.pieceLocations[BLACK][KNIGHT]));
		whiteScore += materialScores[BISHOP] * (populationCount(positions.at(ply).board.pieceLocations[WHITE][BISHOP]) - populationCount(positions.at(ply).board.pieceLocations[BLACK][BISHOP]));
		whiteScore += materialScores[ROOK] * (populationCount(positions.at(ply).board.pieceLocations[WHITE][ROOK]) - populationCount(positions.at(ply).board.pieceLocations[BLACK][ROOK]));
		whiteScore += materialScores[QUEEN] * (populationCount(positions.at(ply).board.pieceLocations[WHITE][QUEEN]) - populationCount(positions.at(ply).board.pieceLocations[BLACK][QUEEN]));
		materialScore[WHITE] = whiteScore;
		materialScore[BLACK] = whiteScore * -1;
	}

	bool Game::makeMove(Move move)
	{	
		if (gameState != GAME_STATE::PLAYING)
			return false;

		// increment ply, copy the last position into the new postion, apply the move
		ply = ply + 1;
		if (ply == positions.size())
			positions.push_back(positions[ply - 1]);
		else
			positions[ply] = positions[ply - 1];
		
		updatePosition(positions[ply], move);
		positions[ply].hash = HASH::zobrist.generateHash(positions[ply]);
		moveHistory[ply] = move;
		
		checkGameState();	
		calculateMaterial();

		return true;
	}

	void Game::undoMove() {
		// decrement ply
		if (ply > 0) {
			ply = ply - 1;
			gameState = GAME_STATE::PLAYING;
		}
		moves.clear();
		generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, moves);
		calculateMaterial();
	}

	void Game::clear() {
		positions.clear();
		moveHistory.clear();
		moves.clear();
		positions.resize(MAX_PLY);
		moveHistory.resize(MAX_PLY);
		gameState = GAME_STATE::PLAYING;
		gameType = GAME_TYPE::EMPTY;
		materialScore[0] = 0, materialScore[1] = 0;
		ply = 0;
	}

} // KRONOS