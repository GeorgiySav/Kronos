#pragma once
#include <iostream>
#include <string>
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
#include "PGN.h"
#include "consts.h"

namespace KRONOS
{

	class KronosEngine
	{
		Game game;

		SEARCH::Search_Manager search;
		
		std::future<Move> searchThread;
		std::future<bool> infiniteThread;
		bool busyTimed = false;
		bool busyInfinite = false;
		int searchDepth = 20;
		int timeForSearch = 1000;

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
			game.setGameFEN(FEN);
		}

		void startSearchForBestMove() {
			if (!busyTimed && !busyInfinite) {
				std::cout << "Began timed search" << std::endl;
				searchThread = std::async(&SEARCH::Search_Manager::getBestMove, &search, game.getPositions(), game.getPly(), timeForSearch, searchDepth);
				busyTimed = true;
			}
		}

		bool searchFinished() {
			if (busyTimed) {
				return searchThread.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
			}
			return false;
		}

		void stopTimedSearch() {
			if (busyTimed) {
				std::cout << "Stopped timed search" << std::endl;
				search.cancelTimedSearch();
				searchThread.get();
				busyTimed = false;
			}
		}

		Move getBestMove() {
			if (busyTimed && searchThread.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
				busyTimed = false;
				return searchThread.get();
			}
			return NULL_MOVE;
		}

		void stopInfiniteSearch() {
			if (busyInfinite) {
				search.cancelInfiniteSearch();
				infiniteThread.get();
				std::cout << "Stopped infinite search" << std::endl;
				busyInfinite = false;
			}
		}

		void beginInfiniteSearch() {
			stopInfiniteSearch();
			infiniteThread = std::async(&SEARCH::Search_Manager::infiniteSearch, &search, game.getPositions(), game.getPly(), searchDepth);
			busyInfinite = true;
			std::cout << "Began infinite search" << std::endl;
		}

		void checkIfInfiniteThreadFinished() {
			if (busyInfinite && infiniteThread.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
				infiniteThread.get();
				busyInfinite = false;
				std::cout << "Infinite search finished" << std::endl;
			}
		}

		bool isInfiniting() { return busyInfinite; }
		bool isTimedSearching() { return busyTimed; }


		int getDepthSearchedTo() {
			return search.getBestDepth();
		}

		int getScoreEvaluated() {
			return search.getCurrentScore();
		}

		std::string getBestMoveSoFar() {
			return KronosMoveToAlgebraic(search.getBestMoveSoFar(), game.getPositions()->at(game.getPly()));
		}

		std::string getPgnMove(Move move) {
			return KronosMoveToAlgebraic(move, game.getPositions()->at(game.getPly()));
		}

		Board* getBitBoardsPointer() {
			return game.getBoardPointer();
		}

		BoardStatus* getStatusPointer() {
			return game.getStatusPointer();
		}
		
		KRONOS::Move_List* getMovesPointer() {
			return game.getMovesPointer();
		}

		Board* getBoard(int ply) {
			return &game.getPositions()->at(ply).board;
		}

		int getMaterialScore(bool side) {
			return game.getMaterial(side);
		}

		void setNumCores(int n) {
			search.initSearchThreads(n);
		}

		int getNumCores() {
			return search.getNumberOfCores();
		}

		void setTransSize(int mb) {
			search.changeTransTableSize(mb);
		}

		size_t getTransSize() {
			return search.getTransSize();
		}

		void setSearchDepth(int newDepth) {
			searchDepth = newDepth;
		}

		int getSearchDepth() {
			return searchDepth;
		}

		void setTimeForSearch(int newTime) {
			timeForSearch = newTime;
		}

		int getTimeForSearch() {
			return timeForSearch;
		}

		template <GAME_TYPE type>
		void createGame(const std::string& fen = FEN_START_POSITION) {
			game.createGame<type>(fen);
		}

	};

}