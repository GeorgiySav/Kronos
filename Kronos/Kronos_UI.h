#pragma once
#include <optional>

#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include "Kronos Engine/Kronos.h"

#define DEFAULT_WINDOW_WIDTH 994
#define DEFAULT_WINDOW_HEIGHT 729

// stores information about a piece
struct PieceSprite {
	sf::Texture texture;
	sf::Sprite sprite;

	PieceSprite() {
		texture = sf::Texture();
		sprite = sf::Sprite();
	}

	~PieceSprite() {}

	// loads a texture from a file
	void set(std::string picturePath, sf::IntRect area) {
		texture.loadFromFile(picturePath, area);
		sprite.setTexture(texture);
		sprite.setOrigin(sprite.getTexture()->getSize().x * 0.5, sprite.getTexture()->getSize().y * 0.5);
	}
};

// keeps track of a piece selected by the mouse
struct SelectedPiece {
	int pos;
	PieceSprite* sprite;
	std::vector<KRONOS::Move> moves;
	int colour, type;
	SelectedPiece() {
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
	PieceSprite sprites[2][6];

	sf::Color white = sf::Color(163, 162, 159, 255);
	sf::Color black = sf::Color(60, 60, 60, 255);

	sf::Texture boardTexture;
	sf::Sprite boardSprite;

	sf::Vector2f boardPosition;

	float pieceScale = 0.37f;
	float boardScale = 100.f;

	float scale = 1.f;

	sf::CircleShape possibleMove;

	SelectedPiece selected;

public:
	Kronos_Board_UI();
	~Kronos_Board_UI() {}

	// attempts to select a piece when the user clicks on the board
	void selectPiece(sf::RenderWindow& window, KRONOS::Board* board, KRONOS::Move_List* moves, bool isWhite, bool whiteBottom);
	// returns a possible move if the user dropped the piece onto a valid tile
	std::optional<KRONOS::Move> dropPiece(sf::RenderWindow& window, KRONOS::Board* board, bool whiteBottom);
	// renders the board
	void renderKronosBoard(sf::RenderWindow& window, KRONOS::Board* board, bool whiteBottom);

	int getBoardWidth() {
		return boardSprite.getGlobalBounds().width;
	}

	sf::Sprite* getBoardSpritePointer() {
		return &boardSprite;
	}

	void setPosition(sf::Vector2f pos) {
		boardPosition = pos;
		boardSprite.setPosition(boardPosition);
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

	// calculates the x and y pos on the window for a piece to rendered to
	sf::Vector2f boardIndexToBoardPos(int boardIndex) {
		return { float(boardPosition.x + (boardIndex % 8) * getBoardInterval() + (getBoardInterval() * 0.5)),
				 float(boardPosition.y + boardSprite.getGlobalBounds().height - (std::floor(boardIndex / 8) * getBoardInterval() + (getBoardInterval() * 0.5))) };
	}

};

class PromotionPopUp {
private:
	bool showPromotionPopUp;
	KRONOS::Move move;
	ImVec2 buttonSize = { 50.0f, 20.0f };
	uint8_t flag;

public:
	PromotionPopUp();
	~PromotionPopUp();

	void createPromotionPopUp(KRONOS::Move move);
	std::optional<KRONOS::Move> renderPromotionPopUp();
};
