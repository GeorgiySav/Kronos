#pragma once
#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include <map>
#include <sstream>
#include <array>
#include <chrono>
#include <memory>
#include <thread>
#include <future>

#include "utility.h"
#include "BitBoard.h"
#include "Magic.h"
#include "Rays.h"
#include "Board.h"
#include "Move_Generation.h"
#include "Zobrist_Hashing.h"
#include "Search_Manager.h"
#include "Game.h"
#include "Tuner.h"
#include "FEN.h"

namespace KRONOS
{

	class KronosEngine
	{
		Game game;

		TUNER tuner;

		SEARCH::Search_Manager search;
		
		std::future<Move> searchThread;
		bool busy = false;
		
		int NUM_THREADS = std::thread::hardware_concurrency();
		EVALUATION::PARAMS::Eval_Parameters params;

	public:

		KronosEngine();
		~KronosEngine();

		int BoardCoordsToIndex(std::string coord) {
			return ((coord[1] - '1') * 8) + (coord[0] - 'a');
		}

		std::string BoardIndexToCoords(int index) {
			return std::string(1, 'a' + (index % 8)) + std::string(1, '1' + (index / 8));
		}

		inline void makeMove(Move move) {
			game.makeMove(move);
		}			
		
		void unmakeMove() {
			game.undoMove();
		}

		void setFen(std::string FEN) {
			game.clear();
			game.setGame(GAME_TYPE::AI_GAME, FEN);
		}

		void startSearchForBestMove() {
			if (!busy) {
				searchThread = std::async(&SEARCH::Search_Manager::getBestMove, &search, game.getPositions(), game.getPly(), 1000);
				busy = true;
			}
		}

		bool searchFinished() {
			if (busy) {
				return searchThread._Is_ready();
			}
			return false;
		}

		Move getBestMove() {
			if (searchThread._Is_ready()) {
				busy = false;
				return searchThread.get();
			}
			else {
				return NULL_MOVE;
			}
		}

		void beginAutoGame() {
			std::thread thread(&TUNER::playGame, &tuner);
			thread.detach();
		}

		EVALUATION::PARAMS::Eval_Parameters* getEvalParam1() {
			return tuner.getParam1();
		}

		EVALUATION::PARAMS::Eval_Parameters* getEvalParam2() {
			return tuner.getParam2();
		}

		Board* getBitBoardsPointer() {
			return game.getBoardPointer();
		}

		BoardStatus* getStatusPointer() {
			return game.getStatusPointer();
		}
		
		KRONOS::Move_List<256>* getMovesPointer() {
			return game.getMovesPointer();
		}
		
		void traceEval() {
			//search.evaluate.tracedEval(positions[ply]);
			//std::cout << "Evaluate at a depth of 5: " << search.searchWithAlphaBeta(&positions, ply, 5) << std::endl << std::endl;
		}

		friend std::ostream& operator<<(std::ostream& os, const KronosEngine& bb) {

			//os << "White Pawns: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][PAWN]) << std::endl;
			//os << "White Knights: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][KNIGHT]) << std::endl;
			//os << "White Bishops: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][BISHOP]) << std::endl;
			//os << "White Rooks: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][ROOK]) << std::endl;
			//os << "White Queens: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][QUEEN]) << std::endl;
			//os << "White King: \n" << _BitBoard(positions[ply].board.pieceLocations[WHITE][KING]) << std::endl;
			//
			//os << "Black Pawns: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][PAWN]) << std::endl;
			//os << "Black Knights: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][KNIGHT]) << std::endl;
			//os << "Black Bishops: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][BISHOP]) << std::endl;
			//os << "Black Rooks: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][ROOK]) << std::endl;
			//os << "Black Queens: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][QUEEN]) << std::endl;
			//os << "Black King: \n" << _BitBoard(positions[ply].board.pieceLocations[BLACK][KING]) << std::endl;;

			return os;
		}

	};

}