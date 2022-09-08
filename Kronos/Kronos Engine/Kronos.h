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

namespace KRONOS
{
	
	struct perftResults {
		int nodes;
		int captures;
		int eps;
		int castles;
		int promotions;
		
		perftResults() : nodes(0), captures(0), eps(0), castles(0), promotions(0) {}

		perftResults operator + (const perftResults& other) const {
			perftResults result;
			result.nodes = this->nodes + other.nodes;
			result.captures = this->captures + other.captures;
			result.eps = this->eps + other.eps;
			result.castles = this->castles + other.castles;
			result.promotions = this->promotions + other.promotions;
			return result;
		}			
		
		friend std::ostream& operator << (std::ostream& os, const perftResults& results) {
			os << "Nodes: " << results.nodes << std::endl;
			os << "Captures: " << results.captures << std::endl;
			os << "Eps: " << results.eps << std::endl;
			os << "Castles: " << results.castles << std::endl;
			os << "Promotions: " << results.promotions << std::endl;
			return os;
		}

	};

	class KronosEngine
	{
		Game game;

		TUNER tuner;

		SEARCH::Search_Manager search;
		
		std::future<Move> searchThread;
		bool busy = false;
		
		int NUM_THREADS = std::thread::hardware_concurrency();
		EVALUATION::Evaluation eval;
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

		//int perft(int depth) {
		//	if (depth == 0) {
		//		//perftResults result = perftResults();
		//		//if (moveHistory[ply].flag == ENPASSANT) {
		//		//	result.eps++;
		//		//}
		//		//else if (moveHistory[ply].flag & PROMOTION) {
		//		//	result.promotions++;
		//		//}
		//		//else if (moveHistory[ply].flag == KING_CASTLE || moveHistory[ply].flag == QUEEN_CASTLE) {
		//		//	result.castles++;
		//		//}
		//		//
		//		//if (moveHistory[ply].flag & CAPTURE) {
		//		//	result.captures++;
		//		//}
		//		//
		//		//result.nodes++;
		//		//return result;
		//		return 1;
		//	}
		//
		//	int count = 0;
		//
		//	generateMoves();
		//	std::vector<Move> moves = this->moves;
		//
		//	for (Move move : moves) {
		//		makeMove(move);
		//		count += perft(depth - 1);
		//		unmakeMove();
		//	}
		//
		//	return count;
		//
		//}
		//
		//perftResults perftDiv(int depth) {
		//	
		//	perftResults count = perftResults();
		//
		//	generateMoves();
		//	std::vector<Move> moves = this->moves;
		//
		//	for (Move move : moves) {
		//		makeMove(move);
		//		std::cout << BoardIndexToCoords(move.from) << BoardIndexToCoords(move.to) << ":\n" << perft(depth - 1) << '\n';
		//		unmakeMove();
		//	}
		//
		//	return count;
		//
		//}

		void setFen(std::string FEN) {
			game.clear();
			game.setGame(GAME_TYPE::AI_GAME, FEN);
		}

		void getStaticEval() {
			std::cout << "Static Evaluation: " << eval.evaluate(game.getPositions()->at(game.getPly())) << std::endl;
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