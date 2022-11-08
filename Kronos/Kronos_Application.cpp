#include "Kronos_Application.h"

Kronos_Application::Kronos_Application()
	: window(sf::RenderWindow(sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), "Chess"))
	, boardUI(Kronos_Board_UI())
	, events(sf::Event())
{

	kronosEngine = std::make_unique<KRONOS::KronosEngine>();

	ImGui::SFML::Init(window);

	//kronosEngine->setFen("r4rk1/pp2pp1p/3p2p1/6B1/3Q3P/1P6/1RPKnPP1/q6R w - - 0 1");

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
		
		if (kronosEngine->searchFinished()) {
			kronosEngine->makeMove(kronosEngine->getBestMove());
		}

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

		if (ImGui::Begin("FEN")) {
			static char buffer[100];
			if (ImGui::InputText("FEN", buffer, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
				kronosEngine->setFen(std::string(buffer));
				memset(buffer, 0, sizeof(buffer));
			}
			ImGui::End();
		}

		render();
	}
}

void Kronos_Application::render()
{
	window.clear();
	boardUI.renderKronosBoard(window, kronosEngine->getBitBoardsPointer(), true);
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
					kronosEngine->unmakeMove();
					//pgnMoves.pop_back();	
			}
			if (events.key.code == sf::Keyboard::G) {
				kronosEngine->startSearchForBestMove();
			}
			if (events.key.code == sf::Keyboard::P) {
				kronosEngine->beginAutoGame();
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
				}
			}
		}

	}

}

void renderParamEditor(KRONOS::EVALUATION::PARAMS::Eval_Parameters* param)
{
	
	ImGui::BeginChild(" ", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::PushItemWidth(100);
	ImGui::Text("PAWN VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##0", &param->PAWN_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##1", &param->PAWN_VALUE.endGame);
	
	ImGui::Text("KNIGHT VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##2", &param->KNIGHT_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##3", &param->KNIGHT_VALUE.endGame);
	
	ImGui::Text("BISHOP VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##4", &param->BISHOP_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##5", &param->BISHOP_VALUE.endGame);
	
	ImGui::Text("ROOK VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##6", &param->ROOK_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##7", &param->ROOK_VALUE.endGame);

	ImGui::Text("QUEEN VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##8", &param->QUEEN_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##9", &param->QUEEN_VALUE.endGame);
	
	ImGui::Text("KING VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##10", &param->KING_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##11", &param->KING_VALUE.endGame);
	
	ImGui::Text("CONNECTED PAWN VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##12", &param->CONNECTED_PAWN_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##13", &param->CONNECTED_PAWN_VALUE.endGame);
	
	ImGui::Text("DOUBLED PAWN VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##16", &param->DOUBLED_PAWN_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##17", &param->DOUBLED_PAWN_VALUE.endGame);
	
	ImGui::Text("ISOLATED PAWN VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##18", &param->ISOLATED_PAWN_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##19", &param->ISOLATED_PAWN_VALUE.endGame);

	ImGui::Text("BACKWARD PAWN VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##22", &param->BACKWARD_PAWN_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##23", &param->BACKWARD_PAWN_VALUE.endGame);
	
	ImGui::Text("PASSED ISOLATED PAWN VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##24", &param->PASSED_ISOLATED_PAWN_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##25", &param->PASSED_ISOLATED_PAWN_VALUE.endGame);
	
	ImGui::Text("PASSED BACKWARD PAWN VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##26", &param->PASSED_BACKWARD_PAWN_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##27", &param->PASSED_BACKWARD_PAWN_VALUE.endGame);
	
	ImGui::Text("BISHOP_PAWN_PENALTY: ");
	ImGui::SameLine();
	ImGui::InputInt("##32", &param->BISHOP_PAWN_PENALTY.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##33", &param->BISHOP_PAWN_PENALTY.endGame);

	ImGui::Text("ROOK_SEMI_OPEN_FILE_BONUS: ");
	ImGui::SameLine();
	ImGui::InputInt("##34", &param->ROOK_SEMI_OPEN_FILE_BONUS.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##35", &param->ROOK_SEMI_OPEN_FILE_BONUS.endGame);

	ImGui::Text("ROOK_OPEN_FILE_BONUS: ");
	ImGui::SameLine();
	ImGui::InputInt("##36", &param->ROOK_OPEN_FILE_BONUS.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##37", &param->ROOK_OPEN_FILE_BONUS.endGame);

	ImGui::Text("FLANK_ATTACKS: ");
	ImGui::SameLine();
	ImGui::InputInt("##38", &param->FLANK_ATTACKS.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##39", &param->FLANK_ATTACKS.endGame);
	
	ImGui::Text("PAWNLESS_FLANK: ");
	ImGui::SameLine();
	ImGui::InputInt("##40", &param->PAWNLESS_FLANK.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##41", &param->PAWNLESS_FLANK.endGame);
	
	ImGui::Text("THREAT_PAWN_PUSH_VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##42", &param->THREAT_PAWN_PUSH_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##43", &param->THREAT_PAWN_PUSH_VALUE.endGame);
	
	ImGui::Text("THREAT_WEAK_PAWNS_VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##44", &param->THREAT_WEAK_PAWNS_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##45", &param->THREAT_WEAK_PAWNS_VALUE.endGame);
	
	ImGui::Text("THREAT_PAWNSxMINORS_VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##46", &param->THREAT_PAWNSxMINORS_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##47", &param->THREAT_PAWNSxMINORS_VALUE.endGame);
	
	ImGui::Text("THREAT_MINORSxMINORS_VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##48", &param->THREAT_MINORSxMINORS_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##49", &param->THREAT_MINORSxMINORS_VALUE.endGame);
	
	ImGui::Text("THREAT_MAJORSxWEAK_MINORS_VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##50", &param->THREAT_MAJORSxWEAK_MINORS_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##51", &param->THREAT_MAJORSxWEAK_MINORS_VALUE.endGame);
	
	ImGui::Text("THREAT_PAWN_MINORSxMAJORS_VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##52", &param->THREAT_PAWN_MINORSxMAJORS_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##53", &param->THREAT_PAWN_MINORSxMAJORS_VALUE.endGame);

	ImGui::Text("THREAT_ALLxQUEENS_VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##54", &param->THREAT_ALLxQUEENS_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##55", &param->THREAT_ALLxQUEENS_VALUE.endGame);
	
	ImGui::Text("THREAT_KINGxMINORS_VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##56", &param->THREAT_KINGxMINORS_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##57", &param->THREAT_KINGxMINORS_VALUE.endGame);

	ImGui::Text("THREAT_KINGxROOKS_VALUE: ");
	ImGui::SameLine();
	ImGui::InputInt("##58", &param->THREAT_KINGxROOKS_VALUE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##59", &param->THREAT_KINGxROOKS_VALUE.endGame);
	
	ImGui::Text("PIECE SPACE");
	ImGui::SameLine();
	ImGui::InputInt("##60", &param->PIECE_SPACE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##61", &param->PIECE_SPACE.endGame);
	
	ImGui::Text("EMPTY SPACE");
	ImGui::SameLine();
	ImGui::InputInt("##62", &param->EMPTY_SPACE.middleGame);
	ImGui::SameLine();
	ImGui::InputInt("##63", &param->EMPTY_SPACE.endGame);
	
	if (ImGui::Button("Save params")) {
		param->saveParams();
	}

	static char buffer[50];
	if (ImGui::InputText("Enter filename", buffer, 50, ImGuiInputTextFlags_EnterReturnsTrue)) {
		param->loadParams("../Kronos/Eval Parameters/" + std::string(buffer));
	}

	ImGui::EndChild();
}