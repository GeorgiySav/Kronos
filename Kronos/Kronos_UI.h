#pragma once
#include <optional>

#include "SFML/Graphics.hpp"

#include "Kronos Engine/Kronos.h"

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

struct pieceSprite {
	sf::Texture texture;
	sf::Sprite sprite;

	pieceSprite() {
		texture = sf::Texture();
		sprite = sf::Sprite();
	}

	~pieceSprite() {

	}

	void set(std::string picturePath, sf::IntRect area) {
		texture.loadFromFile(picturePath, area);
		sprite.setTexture(texture);
		sprite.setOrigin(sprite.getTexture()->getSize().x * 0.5, sprite.getTexture()->getSize().y * 0.5);
	}
};

struct selectedPiece {
	int pos;
	pieceSprite* sprite;
	std::vector<KRONOS::Move> moves;
	int colour, type;
	selectedPiece() {
		pos = KRONOS::no_Tile;
		sprite = nullptr;
		moves.clear();
		colour = -1;
		type = -1;
	}
};

class Kronos_Board_UI
{
private:
	unsigned int size;

	pieceSprite sprites[2][6];

	sf::Color white = sf::Color(124, 76, 62, 255);
	sf::Color black = sf::Color(81, 42, 42, 255);

	sf::Texture boardTexture;
	sf::Sprite boardSprite;

	sf::Vector2f boardPosition;

	float pieceScale = 0.37f;
	float boardScale = 100.f;

	float scale = 1.f;

	sf::CircleShape possibleMove;

	selectedPiece selected;

public:
	Kronos_Board_UI() {

		sf::Sprite temp;
		temp.setScale({ pieceScale, pieceScale });

		sprites[KRONOS::WHITE][KRONOS::PAWN].set("Images/chess_sprite_sheet.png", sf::IntRect(0, 0, 256, 256));
		sprites[KRONOS::WHITE][KRONOS::KNIGHT].set("Images/chess_sprite_sheet.png", sf::IntRect(256, 0, 256, 256));
		sprites[KRONOS::WHITE][KRONOS::BISHOP].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 2, 0, 256, 256));
		sprites[KRONOS::WHITE][KRONOS::ROOK].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 3, 0, 256, 256));
		sprites[KRONOS::WHITE][KRONOS::QUEEN].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 4, 0, 256, 256));
		sprites[KRONOS::WHITE][KRONOS::KING].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 5, 0, 256, 256));

		sprites[KRONOS::BLACK][KRONOS::PAWN].set("Images/chess_sprite_sheet.png", sf::IntRect(0, 256, 256, 256));
		sprites[KRONOS::BLACK][KRONOS::KNIGHT].set("Images/chess_sprite_sheet.png", sf::IntRect(256, 256, 256, 256));
		sprites[KRONOS::BLACK][KRONOS::BISHOP].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 2, 256, 256, 256));
		sprites[KRONOS::BLACK][KRONOS::ROOK].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 3, 256, 256, 256));
		sprites[KRONOS::BLACK][KRONOS::QUEEN].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 4, 256, 256, 256));
		sprites[KRONOS::BLACK][KRONOS::KING].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 5, 256, 256, 256));


		sf::Image img;
		img.create(2, 2);

		img.setPixel(0, 0, white);
		img.setPixel(0, 1, black);
		img.setPixel(1, 0, black);
		img.setPixel(1, 1, white);

		boardTexture.loadFromImage(img);
		boardTexture.setRepeated(true);

		boardSprite.setTexture(boardTexture);
		boardSprite.setTextureRect(sf::IntRect(0, 0, 8, 8));
		boardSprite.setScale({ boardScale, boardScale });

		possibleMove = sf::CircleShape(15);
		possibleMove.setFillColor(sf::Color(255, 255, 255, 75));
		possibleMove.setOrigin(15, 15);

	}

	~Kronos_Board_UI() {

	}

	sf::Sprite* getBoardSpritePointer() {
		return &boardSprite;
	}

	void setPosition(sf::Vector2f pos) {
		boardPosition = pos;
	}

	void setScale(float scale) {

		boardSprite.setScale(boardScale * scale, boardScale * scale);
		for (auto& colour : { KRONOS::WHITE, KRONOS::BLACK }) {
			for (auto& piece : { KRONOS::PAWN, KRONOS::KNIGHT, KRONOS::BISHOP, KRONOS::ROOK, KRONOS::QUEEN, KRONOS::KING }) {
				sprites[colour][piece].sprite.setScale(pieceScale * boardSprite.getGlobalBounds().width / 8 / 100, pieceScale * boardSprite.getGlobalBounds().width / 8 / 100);
			}
		}
		possibleMove.setScale(scale, scale);

	}

	float getBoardInterval() {
		return boardSprite.getGlobalBounds().width / 8;
	}

	int getBoardIndex(int mouseX, int mouseY) {
		return (8 * (7 - std::floor(mouseY / getBoardInterval()))) + (std::floor(mouseX / getBoardInterval()));
	}

	sf::Vector2f boardIndexToBoardPos(int boardIndex) {
		return { float(boardPosition.x + (boardIndex % 8) * getBoardInterval() + (getBoardInterval() * 0.5)),
				float(boardPosition.y + boardSprite.getGlobalBounds().height - (std::floor(boardIndex / 8) * getBoardInterval() + (getBoardInterval() * 0.5))) };
	}

	void selectPiece(sf::RenderWindow& window, KRONOS::Board* board, KRONOS::Move_List<256>* moves, bool isWhite, bool whiteBottom) {
		sf::Vector2i relPos = sf::Mouse::getPosition(window) - sf::Vector2i(boardPosition);
		int boardIndex = getBoardIndex(relPos.x, relPos.y);
		if (!whiteBottom)
			boardIndex = 63 - boardIndex;

		selected.moves.clear();

		for (auto& piece : { KRONOS::PAWN, KRONOS::KNIGHT, KRONOS::BISHOP, KRONOS::ROOK, KRONOS::QUEEN, KRONOS::KING }) {
			if (board->pieceLocations[isWhite][piece] & (1ULL << boardIndex)) {
				selected.pos = boardIndex;
				selected.sprite = &sprites[isWhite][piece];
				selected.colour = isWhite;
				selected.type = piece;
				for (int i = 0; i < moves->size; i++) {
					if (moves->at(i).from == boardIndex) {
						selected.moves.push_back(moves->at(i));
					}
				}
			}
		}

	}

	std::optional<KRONOS::Move> dropPiece(sf::RenderWindow& window, KRONOS::Board* board, bool whiteBottom) {
		if (selected.pos != KRONOS::no_Tile && boardSprite.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)))) {
			sf::Vector2i relPos = sf::Mouse::getPosition(window) - sf::Vector2i(boardPosition);
			int boardIndex = getBoardIndex(relPos.x, relPos.y);
			if (!whiteBottom)
				boardIndex = 63 - boardIndex;

			if (!selected.moves.empty()) {
				for (auto& m : selected.moves) {
					if (m.to == boardIndex) {
						selected = selectedPiece();
						return m;
					}
				}
			}
		}
		selected = selectedPiece();
		return {};
	}

	void renderKronosBoard(sf::RenderWindow& window, KRONOS::Board* board, bool whiteBottom) {
		boardSprite.setPosition(boardPosition);
		window.draw(boardSprite);

		float bInterval = getBoardInterval();

		for (auto& colour : { KRONOS::WHITE, KRONOS::BLACK }) {
			for (auto& piece : { KRONOS::PAWN, KRONOS::KNIGHT, KRONOS::BISHOP, KRONOS::ROOK, KRONOS::QUEEN, KRONOS::KING }) {
				KRONOS::BitBoard bb = board->pieceLocations[colour][piece];
				while (bb) {
					int piecePos = KRONOS::bitScanForward(bb);
					popBit(bb, piecePos);
					if (selected.pos == piecePos)
						continue;
					if (!whiteBottom)
						piecePos = 63 - piecePos;
					sprites[colour][piece].sprite.setPosition(boardIndexToBoardPos(piecePos));
					window.draw(sprites[colour][piece].sprite);
				}
			}
		}


		if (selected.pos != KRONOS::no_Tile) {
			selected.sprite->sprite.setPosition(sf::Vector2f(sf::Mouse::getPosition(window)));
			window.draw(selected.sprite->sprite);
			if (!selected.moves.empty()) {
				for (auto& move : selected.moves) {
					int moveTo = move.to;
					if (!whiteBottom)
						moveTo = 63 - moveTo;
					possibleMove.setPosition(boardIndexToBoardPos(moveTo));
					window.draw(possibleMove);
				}
			}
		}

	}

	

};
