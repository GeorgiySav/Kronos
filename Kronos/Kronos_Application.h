#pragma once
#include <memory>

#include "SFML/Graphics.hpp"
#include "imgui-SFML.h"

#include "Kronos Engine/Kronos.h"

#include "Kronos_UI.h"

class Kronos_Application
{
private:

	bool running = true;

	// ui

	sf::RenderWindow window;
	sf::Event events;

	sf::Clock deltaClock;

	Kronos_Board_UI boardUI;

	// engine
	std::unique_ptr<KRONOS::KronosEngine> kronosEngine;

	// multiplayer

public:
	Kronos_Application();
	~Kronos_Application();

	void run();
	void render();

	void processInputs();

};