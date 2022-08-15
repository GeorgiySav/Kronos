#include "Game.h"

#include "consts.h"
#include "Zobrist_Hashing.h"
#include "FEN.h"

namespace KRONOS
{

	Game::Game()
	{
		this->clear();
	}

	Game::~Game()
	{
		
	}

	void Game::setGame(GAME_TYPE gt, std::string FEN)
	{
		this->gameType = gt;
		
		this->clear();
		positions[ply] = FENtoBoard(FEN);

		checkGameState();
	}

	void Game::setGame(GAME_TYPE gt)
	{
		this->gameType = gt;
		
		this->clear();
		positions[ply] = FENtoBoard(FEN_START_POSITION);
		
		gameState = GAME_STATE::PLAYING;

		moves.clear();
		generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, &moves);
	}

	void Game::checkGameState()
	{
		// check to see if the game has ended through checkmate or stalemate
		moves.clear();
		generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, &moves);

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
					if (count >= 3) {
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
				    this means that both sides have a king and another pieces
					game will end by insufficient material if both sides have either a:
						knight or
						bishop
				*/
				if ((positions[ply].board.pieceLocations[WHITE][KNIGHT] | positions[ply].board.pieceLocations[WHITE][BISHOP]) &&
					(positions[ply].board.pieceLocations[BLACK][KNIGHT] | positions[ply].board.pieceLocations[BLACK][KNIGHT])) {
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
				 || populationCount(positions[ply].board.pieceLocations[BLACK][BISHOP]) == 2) {
					gameState = GAME_STATE::DRAW_TO_LACK_OF_MATERIAL;
					return;
				}
				// syzygy tablebase draw for AIs
				if (gameType == GAME_TYPE::AI_GAME) {
					if (populationCount(positions[ply].board.pieceLocations[WHITE][KNIGHT]) == 2
					 || populationCount(positions[ply].board.pieceLocations[BLACK][KNIGHT]) == 2) {
						gameState = GAME_STATE::DRAW_TO_LACK_OF_MATERIAL;
						return;
					}
				}
			}
		}
	}

	bool Game::makeMove(Move move)
	{
		
		if (gameState != GAME_STATE::PLAYING)
			return false;

		ply = ply + 1;
		if (ply >= MAX_PLY && gameType == GAME_TYPE::AI_GAME)
		{
			ply = ply - 1;
			gameState = GAME_STATE::CANCELLED_GAME;
			return false;
		}
		
		if (ply >= MAX_PLY && gameType == GAME_TYPE::HUMAN_GAME)
		{
			positions.push_back(positions[ply - 1]);
			moveHistory.push_back(move);
		}
		else {
			positions[ply] = positions[ply - 1];
		}
		updatePosition(positions[ply], move);
		positions[ply].hash = HASH::zobrist.generateHash(positions[ply]);
		moveHistory[ply] = move;
		
		checkGameState();
		
		return true;

	}

	void Game::undoMove() {
		if (ply > 0) {
			ply = ply - 1;
			gameState = GAME_STATE::PLAYING;
		}
		moves.clear();
		generateMoves(positions[ply].status.isWhite, positions[ply].board, positions[ply].status, &moves);
	}

	void Game::clear() {
		positions.clear();
		moveHistory.clear();
		moves.clear();
		positions.resize(MAX_PLY);
		moveHistory.resize(MAX_PLY);
		ply = 0;
	}

} // KRONOS