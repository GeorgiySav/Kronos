#pragma once
#include <memory>

#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include "Kronos Engine/Kronos.h"

#include "Kronos_UI.h"

class Kronos_Application
{
private:

	bool running = true;

	// ui
	sf::RenderWindow window;
	sf::Event events; // input events

	sf::Clock deltaClock;

	Kronos_Board_UI boardUI;
	PromotionPopUp promotionPopUp;

	// evaluation bar
	sf::RectangleShape whiteBar;
	sf::RectangleShape blackBar;
	sf::Text evalText;
	
	sf::Font font;
	ImFont* mainFont;
	float moveFontScale = 1.25f;

	bool whiteBottom = true;

	sf::Vector2f boardPos;
	sf::Vector2f playerInfoPos;
	float playerInfoOffset = 1.5f + 50.0f + 600.0f + 1.5f;
	sf::Vector2f evalBarPos;

	bool renderEndOfGamePopUp = false;

	// engine
	std::unique_ptr<KRONOS::KronosEngine> kronosEngine;
	bool playerSide = KRONOS::WHITE;
	std::vector<std::string> pgnMoves;
	int previewPly = 0;

public:
	Kronos_Application();
	~Kronos_Application();

	void processInputs();
	void render();
	void run(); // application loop

	// creates an input field for a given setting in the settings page
	void createKronosSetting(const std::string& enterMessage, const std::string& inputId, const std::string& enterId, std::function<void(int)> setFunc, int& sliderValue, int min, int max);

	// flips the board
	void flipBoard();
};