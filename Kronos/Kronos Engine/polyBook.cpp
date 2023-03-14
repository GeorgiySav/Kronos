#include "polyKeys.h"
#include "polyBook.h"

#include <algorithm>
#include <fstream>
#include  <random>
#include  <iterator>

namespace KRONOS
{
	namespace POLY
	{

		Opening_Book::Opening_Book()
		{
			mode = POLY_MODE::RANDOM;
		}

		Opening_Book::~Opening_Book()
		{
			if (book)
				delete[] book;
		}

		void Opening_Book::setMode(POLY_MODE mode)
		{
			this->mode = mode;
		}

		void Opening_Book::readBook(const char* fileName)
		{
			if (book)
				delete[] book;

			std::ifstream bookFile(fileName, std::ios::binary | std::ios::out);
			if (!bookFile) {
				std::cout << "File not read" << std::endl;
				return;
			}

			bookFile.seekg(0, std::ios::end);
			long long position = bookFile.tellg();

			if (position < sizeof(POLY_BOOK_ENTRY)) {
				std::cout << "No entries found" << std::endl;
				return;
			}

			numEntries = position / sizeof(POLY_BOOK_ENTRY);
			std::cout << numEntries << " entries found in file" << std::endl;

			book = new POLY_BOOK_ENTRY[numEntries];

			bookFile.seekg(0, std::ios::beg);

			bookFile.read((char*)book, numEntries * sizeof(POLY_BOOK_ENTRY));
			bookFile.close();
			if (!bookFile.good()) {
				std::cout << "Error while reading into book" << std::endl;
				return;
			}
		}

		u64 Opening_Book::polyKeyFromPosition(const Position* position)
		{
			/*
				black pawn    0
				white pawn    1
				black knight  2
				white knight  3
				black bishop  4
				white bishop  5
				black rook    6
				white rook    7
				black queen   8
				white queen   9
				black king   10
				white king   11
			*/
			
			u64 polyKey = 0ULL;

			for (int p = 0; p < 6; p++)	{
				for (int c = 0; c < 2; c++) {
					BitBoard pBB = position->board.pieceLocations[c][p];
					while (pBB) {
						int tile = bitScanForward(pBB);
						popBit(pBB, tile);
						int offset = 64 * ((2 * p) + c) + 8 * std::floor(tile / 8) + (tile % 8);
						polyKey ^= polyNums[offset];
					}
				}
			}

			polyKey ^= (position->status.WKcastle ? polyNums[768] : 0ULL);
			polyKey ^= (position->status.WQcastle ? polyNums[769] : 0ULL);
			polyKey ^= (position->status.BKcastle ? polyNums[770] : 0ULL);
			polyKey ^= (position->status.BQcastle ? polyNums[771] : 0ULL);

			if (position->status.EP != no_Tile) {
				if (getTileBB(position->status.EP) & (generatePawnAttacks(position->board.pieceLocations[position->status.isWhite][PAWN], position->status.isWhite))) {
					int offset = 772 + (position->status.EP % 8);
					polyKey ^= polyNums[offset];
				}
			}

			polyKey ^= (position->status.isWhite ? polyNums[780] : 0ULL);
			return polyKey;
		}

		constexpr Move Opening_Book::decodeU16Move(u16 move, const Position* position)
		{
			/*
				000 000 000 000 000 move code
				000 000 000 000 111 to file
				000 000 000 111 000 to rank
				000 000 111 000 000 from file
				000 111 000 000 000 from rank
				111 000 000 000 000 promotion
			*/

			constexpr int toFileMask =   0b111;
			constexpr int toRankMask =   0b111 << 3;
			constexpr int fromFileMask = 0b111 << (3 * 2);
			constexpr int fromRankMask = 0b111 << (3 * 3);
			constexpr int promoMask =    0b111 << (3 * 4);

			Move dMove;

			dMove.to = (move & toFileMask) + ((move & toRankMask) >> 3) * 8;
			dMove.from = ((move & fromFileMask) >> 6) + ((move & fromRankMask) >> 9) * 8;

			int flag = 0;

			if (getTileBB(dMove.to) & position->board.occupied[!position->status.isWhite]) {
				flag = CAPTURE;
			}

			// check for en passant
			if (position->status.EP != no_Tile
				&& ((getTileBB(dMove.from) & position->board.pieceLocations[position->status.isWhite][PAWN]) && (getTileBB(dMove.to) & getTileBB(position->status.EP)))) {
				flag = ENPASSANT;
				dMove.moved_Piece = PAWN;
			}
			else {
				// check for castling
				if (getTileBB(dMove.from) & position->board.pieceLocations[position->status.isWhite][KING]) {
					if (dMove.from == E1)
					{
						if (dMove.to == A1) {
							flag = QUEEN_CASTLE;
							dMove.to = C1;
						}
						else if (dMove.to == H1) {
							flag = KING_CASTLE;
							dMove.to = G1;
						}
					}
					else if (dMove.from == E8)
					{
						if (dMove.to == A8) {
							flag = QUEEN_CASTLE;
							dMove.to = C8;
						}
						else if (dMove.to == H8) {
							flag = KING_CASTLE;
							dMove.to = G8;
						}
					}
					dMove.moved_Piece = KING;
				}
				else
				{
					// check for promotions
					if (move & promoMask) {
						switch ((move & promoMask) >> 12)
						{
						case 1:
							flag |= KNIGHT_PROMOTION;
							break;
						case 2:
							flag |= BISHOP_PROMOTION;
							break;
						case 3:
							flag |= ROOK_PROMOTION;
							break;
						case 4:
							flag |= QUEEN_PROMOTION;
							break;
						}
						dMove.moved_Piece = PAWN;
					}

					for (int p = 0; p < 5; p++) {
						if (getTileBB(dMove.from) & position->board.pieceLocations[position->status.isWhite][p]) {
							dMove.moved_Piece = p;
							break;
						}
					}
				}

			}

			dMove.flag = flag;
			return dMove;
		}

		// polyglot uses a different endian system so I have to swap their values to match mine
		u16 endian_swap_u16(u16 x)
		{
			x = (x >> 8) |
				(x << 8);
			return x;
		}

		u64 endian_swap_u64(u64 x)
		{
			x = (x >> 56) |
			   ((x << 40) & 0x00FF000000000000) |
			   ((x << 24) & 0x0000FF0000000000) |
			   ((x << 8) & 0x000000FF00000000) |
			   ((x >> 8) & 0x00000000FF000000) |
			   ((x >> 24) & 0x0000000000FF0000) |
			   ((x >> 40) & 0x000000000000FF00) |
				(x << 56);
			return x;
		}

		// returns a random move with the chance of picking a move being dependant on its weight
		POLY_BOOK_ENTRY* getRandomMove(std::vector<POLY_BOOK_ENTRY>* moves)
		{
			unsigned int totalWeights = 0;
			for (auto& move : *moves) {
				totalWeights += move.weight;
			}
			float p = (rand() / static_cast<float>(RAND_MAX) * totalWeights);
			POLY_BOOK_ENTRY* current = &moves->at(0);
			while ((p -= current->weight) > 0) {
				current++;
			}
			return current;
		}

		Move Opening_Book::getBookMove(const Position* position)
		{
			u64 polyKey = polyKeyFromPosition(position);
			std::vector<POLY_BOOK_ENTRY> entries;

			for (int i = 0; i < numEntries; i++) {
				if (polyKey == endian_swap_u64(book[i].key)) {		
					entries.push_back(book[i]);
				}
			}

			if (!entries.empty()) {
				srand(time(NULL));
				if (mode == POLY_MODE::RANDOM) {
					return decodeU16Move(endian_swap_u16(getRandomMove(&entries)->move), position);
				}
				else if (mode == POLY_MODE::BEST_WEIGHT) {
					// sort the opening book moves based on their weight
				    std::sort(entries.begin(), entries.end(), [](const POLY_BOOK_ENTRY& X, const POLY_BOOK_ENTRY& Y) -> bool { return X.weight > Y.weight; });
					
					int upperLimit = 0;
					
					while (upperLimit < entries.size() - 1 && entries[upperLimit].weight == entries[upperLimit + 1].weight)
						upperLimit++;

					// if there are multiple best moves, then pick one randomly
					if (upperLimit)
						return decodeU16Move(endian_swap_u16(entries[rand() % upperLimit].move), position);
					else
						return decodeU16Move(endian_swap_u16(entries[0].move), position);
				}
			}
			else {
				return Move();
			}

		}
	}
}