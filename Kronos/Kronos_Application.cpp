#include "Kronos_Application.h"

Kronos_Application::Kronos_Application()
	: window(sf::RenderWindow(sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), "Chess"))
	, kronosEngine(std::make_unique<KRONOS::KronosEngine>(KRONOS::KronosEngine()))
	, boardUI(Kronos_Board_UI())
	, events(sf::Event())
{
	// must initialise these values for move generation to work
	KRONOS::initRays();
	KRONOS::initMagics();
	KRONOS::ZOBRIST::initZobrists();

	ImGui::SFML::Init(window);

	kronosEngine->processFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
	kronosEngine->generateMoves();

	boardUI.setScale(0.75);
	boardUI.setPosition({ 5, 25 });

	// imgui styling
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Alpha = 1.0;
		style.Alpha = 0.83f;
		style.ChildRounding = 3;
		style.WindowRounding = 3;
		style.GrabRounding = 1;
		style.GrabMinSize = 20;
		style.FrameRounding = 3;

		style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.04f, 0.10f, 0.09f, 0.51f);
	}

}

Kronos_Application::~Kronos_Application()
{
	ImGui::SFML::Shutdown();
}

void Kronos_Application::run()
{
	while (running) {
	
		processInputs();
		ImGui::SFML::Update(window, deltaClock.restart());

		// menu bar
		if (ImGui::BeginMainMenuBar()) {

			if (ImGui::BeginMenu("Settings")) {
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Play")) {

				if (ImGui::BeginMenu("Local")) {

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Multiplayer")) {

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("AI")) {

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		render();
	}
}

void Kronos_Application::render()
{
	window.clear();
	boardUI.renderKronosBoard(window, kronosEngine->getBitBoardsPointer(), kronosEngine->getStatusPointer()->isWhite);
	ImGui::SFML::Render(window);
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
					boardUI.selectPiece(window, kronosEngine->getBitBoardsPointer(), kronosEngine->getMovesPointer(), kronosEngine->getStatusPointer()->isWhite, kronosEngine->getStatusPointer()->isWhite);
				}
			}
		}

		if (events.type == sf::Event::MouseButtonReleased) {
			if (events.key.code == sf::Mouse::Left) {
				if (auto move = boardUI.dropPiece(window, kronosEngine->getBitBoardsPointer(), kronosEngine->getStatusPointer()->isWhite)) {
					kronosEngine->makeMove(move.value());
					kronosEngine->generateMoves();
				}
			}
		}

	}

}