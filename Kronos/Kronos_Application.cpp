#include "Kronos_Application.h"

Kronos_Application::Kronos_Application()
	: window(sf::RenderWindow(sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), "Chess"))
	, kronosEngine(std::make_unique<KRONOS::KronosEngine>(KRONOS::KronosEngine()))
	, boardUI(Kronos_Board_UI())
	, events(sf::Event())
{
	ImGui::SFML::Init(window);
	
	kronosEngine->processFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	kronosEngine->generateMoves();

	boardUI.setScale(0.5);

}

Kronos_Application::~Kronos_Application()
{
}

void Kronos_Application::run()
{
	while (running) {
	
		processInputs();

		//ImGui::SFML::Update(window, deltaClock.restart());
		render();
	}
}

void Kronos_Application::render()
{
	window.clear();
	boardUI.renderKronosBoard(window, kronosEngine->getBitBoardsPointer(), true);
	//ImGui::SFML::Render(window);
	window.display();
}

void Kronos_Application::processInputs() {
	
	while (window.pollEvent(events)) {
		static sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		mousePos = sf::Mouse::getPosition(window);
		sf::Vector2i relPos;

		ImGui::SFML::ProcessEvent(events);
		if (events.type == sf::Event::Closed) {
			window.close();
			running = false;
		}

		if (events.type == sf::Event::KeyPressed) {
			if (events.key.code == sf::Keyboard::Backspace) {
				if (kronosEngine->getPly() > 0) {
					kronosEngine->unmakeMove();
					//pgnMoves.pop_back();
					kronosEngine->generateMoves();
				}
			}
		}
		
		if (events.type == sf::Event::MouseButtonPressed) {
			if (events.key.code == sf::Mouse::Left) {
				if (boardUI.getBoardSpritePointer()->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					boardUI.selectPiece(window, kronosEngine->getBitBoardsPointer(), kronosEngine->getMovesPointer(), kronosEngine->getStatusPointer()->isWhite, true);
				}
			}
		}

		if (events.type == sf::Event::MouseButtonReleased) {
			if (events.key.code == sf::Mouse::Left) {
				if (auto move = boardUI.dropPiece(window, kronosEngine->getBitBoardsPointer(), true)) {
					kronosEngine->makeMove(move.value());
					kronosEngine->generateMoves();
				}
			}
		}

	}

}