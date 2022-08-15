#pragma once

#include "Evaluation.h"
#include "Game.h"
#include "Search.h"

namespace KRONOS {
	
	class TUNER {
	private:
		EVALUATION::PARAMS::Eval_Parameters PARAM_1;
		EVALUATION::PARAMS::Eval_Parameters PARAM_2;

		Game* game = nullptr;

		int timePerMove = 0;

		SEARCH::SearchTree search;

	public:
		TUNER();
		~TUNER();

		void setTimePerMove(int time);
		void giveGame(Game* ptr);

		void playGame();

		EVALUATION::PARAMS::Eval_Parameters* getParam1() {
			return &PARAM_1;
		}

		EVALUATION::PARAMS::Eval_Parameters* getParam2() {
			return &PARAM_2;
		}
	};

} // KRONOS
