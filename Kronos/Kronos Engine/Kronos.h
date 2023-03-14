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
#include "FEN.h"
#include "AlgMove.h"
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
		int searchPositionIndex = 0;

		int NUM_THREADS = std::thread::hardware_concurrency();
		EVALUATION::PARAMS::Eval_Parameters params;		
	public:
		KronosEngine();
		~KronosEngine();

		// applies a move to the current game
		inline void makeMove(Move move) {
			game.makeMove(move);
			searchPositionIndex = game.getPly();
		}			
	
		// undos a move
		void unmakeMove() {
			game.undoMove();
			searchPositionIndex = game.getPly();
		}

		// starts a timed search
		void startSearchForBestMove() {
			if (!busyTimed && !busyInfinite) {
				std::cout << "Began timed search" << std::endl;
				searchThread = std::async(&SEARCH::Search_Manager::getBestMove, &search, game.getPositions(), game.getPly(), timeForSearch, searchDepth);
				busyTimed = true;
			}
		}

		// checks if the timed search has finished
		bool searchFinished() {
			if (busyTimed) {
				return searchThread.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
			}
			return false;
		}

		// stops the timed search
		void stopTimedSearch() {
			if (busyTimed) {
				std::cout << "Stopped timed search" << std::endl;
				search.cancelTimedSearch();
				searchThread.get();
				busyTimed = false;
			}
		}

		// returns the best move once the timed search has finished
		Move getBestMove() {
			if (busyTimed && searchThread.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
				busyTimed = false;
				return searchThread.get();
			}
			return NULL_MOVE;
		}

		// makes KRONOS passive search the first position in a game
		void setSearchPositionIndexFirst() {
			searchPositionIndex = 0;
		}
		// makes KRONOS passive search the last position in a game
		void setSearchPositionIndexLast() {
			searchPositionIndex = game.getPly();
		}
		// makes KRONOS passive search the previous position in a game
		void decrementSearchPositionIndex() {
			searchPositionIndex = std::max(searchPositionIndex - 1, 0);
		}
		// makes KRONOS passive search the next position in a game
		void incrementSearchPostiionsIndex() {
			searchPositionIndex = std::min(searchPositionIndex + 1, game.getPly());
		}

		// starts the passive search
		void beginInfiniteSearch() {
			stopInfiniteSearch();
			infiniteThread = std::async(&SEARCH::Search_Manager::infiniteSearch, &search, game.getPositions(), searchPositionIndex, searchDepth);
			busyInfinite = true;
			std::cout << "Began infinite search" << std::endl;
		}
	
		// stops the passive search
		void stopInfiniteSearch() {
			if (busyInfinite) {
				search.cancelInfiniteSearch();
				infiniteThread.get();
				std::cout << "Stopped infinite search" << std::endl;
				busyInfinite = false;
			}
		}

		// checks if the passive search has finished
		void checkIfInfiniteThreadFinished() {
			if (busyInfinite && infiniteThread.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
				infiniteThread.get();
				busyInfinite = false;
				std::cout << "Infinite search finished" << std::endl;
			}
		}

		// creates a new game
		template <GAME_TYPE type>
		void createGame(const std::string& fen = FEN_START_POSITION) {
			game.createGame<type>(fen);
			search.transTable.clear();
			search.transTable.resetAge();
			search.evalTable.clear();
			searchPositionIndex = 0;
		}

		// when a game is finished, the user can analyse that game
		void changeGameToAnalysis() {
			game.changeToAnalysis();
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

		BoardStatus* getStatusPointer(int ply) {
			return &game.getPositions()->at(ply).status;
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

		
		GAME_TYPE getGameType() {
			return game.getGameType();
		}

		GAME_STATE getGameState() {
			return game.getGameState();
		}


		std::string getFen() {
			return game.getFen();
		}

	};

}