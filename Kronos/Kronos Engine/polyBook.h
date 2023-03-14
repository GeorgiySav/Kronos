#pragma once

#include "Move_Generation.h"
#include "utility.h"

namespace KRONOS
{
	
	namespace POLY
	{

		struct POLY_BOOK_ENTRY {
			u64 key;
			u16 move;
			u16 weight;
			u32 learn;
		};

		enum class POLY_MODE
		{
			// picks a random move based on the weight of each move
			RANDOM,
			// picks the move with the highest weight, if there are multiple equal weights, pick one of them randomly
			BEST_WEIGHT,
			DEFAULT = BEST_WEIGHT
		};

		class Opening_Book {
		private:
			POLY_BOOK_ENTRY* book = nullptr;
			int numEntries = 0;
			POLY_MODE mode;
			// decodes a polyglot integer move to a kronos move
			constexpr Move decodeU16Move(u16 move, const Position* position);
		public:
			Opening_Book();
			~Opening_Book();
			// sets the mode of the book
			void setMode(POLY_MODE mode);
			// loads a opening book into RAM
			void readBook(const char* filename);
			// calculates the polyglot hash of a position
			u64 polyKeyFromPosition(const Position* position);
			// returns a book move if the position is found in the opening book
			Move getBookMove(const Position* position);
		};


	}

}