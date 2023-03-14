#include "Kronos_Application.h"
#include "./Kronos Engine/AlgMove.h"

#include <cstdio>

Kronos_Application::Kronos_Application()
	: window(sf::RenderWindow(sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), "Chess"))
	, boardUI(Kronos_Board_UI())
	, events(sf::Event())
{
	// initialises the engine
	kronosEngine = std::make_unique<KRONOS::KronosEngine>();

	// initialises ImGui by giving it the SFML window
	ImGui::SFML::Init(window);

	// sets the positions of SFML UI elements
	boardPos = { 33.f, 75.f };
	playerInfoPos = { 33.f, 23.5f };
	evalBarPos = { 1.5f, boardPos.y };

	boardUI.setScale(0.75);
	boardUI.setPosition(boardPos);

	whiteBar.setFillColor(sf::Color::White);
	blackBar.setFillColor(sf::Color::Black);

	// initalises the postiions of the evaluation bar
	if (!whiteBottom) {
		whiteBar.setPosition(evalBarPos);
		whiteBar.setSize(sf::Vector2f(30, boardUI.getBoardWidth() * 0.5));

		blackBar.setPosition(sf::Vector2f(whiteBar.getPosition().x, whiteBar.getPosition().y + whiteBar.getGlobalBounds().height));
		blackBar.setSize(sf::Vector2f(30, boardUI.getBoardWidth() - whiteBar.getGlobalBounds().height));
	}
	else {
		blackBar.setPosition(evalBarPos);
		blackBar.setSize(sf::Vector2f(30, boardUI.getBoardWidth() * 0.5));

		whiteBar.setPosition(sf::Vector2f(blackBar.getPosition().x, blackBar.getPosition().y + blackBar.getGlobalBounds().height));
		whiteBar.setSize(sf::Vector2f(30, boardUI.getBoardWidth() - blackBar.getGlobalBounds().height));
	}

	// intialises the fonts
	if (!font.loadFromFile("./JetBrains Mono NL Bold Nerd Font Complete Mono Windows Compatible.ttf")) {
		std::cout << "Couldn't load font" << std::endl;
	}
	else {
		evalText.setFont(font);
		evalText.setCharacterSize(12);
		evalText.setFillColor(sf::Color(128, 128, 128));
	}	

	// ImGui styling
	{

		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		style.GrabRounding = style.FrameRounding = 2.3f;
	}

}

Kronos_Application::~Kronos_Application()
{
	// shutdowns ImGui
	ImGui::SFML::Shutdown();
	// makes sure that all threads are sleeping before destroying the engine
	kronosEngine->stopTimedSearch();
	kronosEngine->stopInfiniteSearch();
}

// calculates the size of the evaluation bar depending on score
float calculateEvalBar(int evaluation, bool whiteBottom) {
	float value = 1 / (1 + exp(-0.005 * evaluation));
	if (whiteBottom)
		return 1 - value;
	else
		return value;
}

void Kronos_Application::createKronosSetting(const std::string& enterMessage, const std::string& inputId, const std::string& enterId, std::function<void(int)> setFunc, int& sliderValue, int min, int max)
{
	static int inputWidth = 100.0f;

	ImGui::Text(enterMessage.c_str());

	ImGui::NextColumn();

	ImGui::PushItemWidth(inputWidth);
	ImGui::InputInt(inputId.c_str(), &sliderValue, 1, 1);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// makes sure that the input is within range, will not allow for the user to enter a value out side of the range
	sliderValue = std::min(max, sliderValue);
	sliderValue = std::max(min, sliderValue);

	if (ImGui::Button(enterId.c_str())) {
		// ensures that all engine threads are idle when changing the engine variables
		if (kronosEngine->isInfiniting()) {
			kronosEngine->stopInfiniteSearch();
			setFunc(sliderValue);
			kronosEngine->beginInfiniteSearch();
		}
		else if (kronosEngine->isTimedSearching()) {
			kronosEngine->stopTimedSearch();
			setFunc(sliderValue);
			kronosEngine->startSearchForBestMove();
		}
		else {
			setFunc(sliderValue);
		}
	}

	ImGui::Separator();
	ImGui::NextColumn();
}

void Kronos_Application::run()
{
	// by default, the application will start with the starting position
	kronosEngine->createGame<KRONOS::GAME_TYPE::ANALYSIS>();
	kronosEngine->beginInfiniteSearch();

	// keeps tract of evaluation
	static int previousEval = 32001;

	// loads the font into ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	ImFont* mainFont = io.Fonts->AddFontFromFileTTF("./JetBrains Mono NL Bold Nerd Font Complete Mono Windows Compatible.ttf", 15);
	ImGui::SFML::UpdateFontTexture();
	IM_ASSERT(mainFont != NULL);

	// sets the seed of the random generator
	srand(time(0));

	while (running) {
		// checks to see if the thread has finished to release resources	
		kronosEngine->checkIfInfiniteThreadFinished();
	
		// checks to see if a timed search has finished and to apply it
		if (kronosEngine->searchFinished()) {
			KRONOS::Move bMove = kronosEngine->getBestMove();
			if (bMove != KRONOS::NULL_MOVE) {
				pgnMoves.push_back(kronosEngine->getPgnMove(bMove));
				kronosEngine->makeMove(bMove);
				previewPly = pgnMoves.size();
				if (kronosEngine->getGameState() != KRONOS::GAME_STATE::PLAYING) {
					renderEndOfGamePopUp = true;
				}
				else {
					kronosEngine->beginInfiniteSearch();
				}
			}
		}

		// evaluation bar, only chanege things if the evaluation has changed
		if (previousEval != kronosEngine->getScoreEvaluated()) {

			if (kronosEngine->getGameType() == KRONOS::GAME_TYPE::ANALYSIS)
				previousEval = kronosEngine->getScoreEvaluated();
			else
				previousEval = 0;

			float eval = previousEval * (kronosEngine->getStatusPointer(previewPly)->isWhite ? 1 : -1);
			float evalBarScale = calculateEvalBar(eval, whiteBottom);

			// checks to see if the engine has found a mate in n moves
			if (std::abs(eval) >= 32000 - kronosEngine->getSearchDepth()) {
				evalText.setString("M" + std::to_string(int(32000 - std::abs(eval))));
			}
			// otherwise just display the evaluation
			else {
				float scaledEval = std::abs(eval) * 0.01;
				std::stringstream ss;
				ss << std::setprecision(3) << scaledEval;
				evalText.setString(ss.str());
			}

			// set the positions
			if (!whiteBottom) {
				whiteBar.setSize(sf::Vector2f(30, boardUI.getBoardWidth() * evalBarScale));

				blackBar.setPosition(sf::Vector2f(whiteBar.getPosition().x, whiteBar.getPosition().y + whiteBar.getGlobalBounds().height));
				blackBar.setSize(sf::Vector2f(30, boardUI.getBoardWidth() - whiteBar.getGlobalBounds().height));

				if (evalBarScale > 0.5) {
					evalText.setPosition(whiteBar.getPosition().x + (whiteBar.getGlobalBounds().width - evalText.getGlobalBounds().width) * 0.5, whiteBar.getPosition().y + whiteBar.getGlobalBounds().height - evalText.getCharacterSize() - 2.5);
				}
				else {
					evalText.setPosition(whiteBar.getPosition().x + (whiteBar.getGlobalBounds().width - evalText.getGlobalBounds().width) * 0.5, blackBar.getPosition().y + 2.5);
				}
			}
			else {
				blackBar.setSize(sf::Vector2f(30, boardUI.getBoardWidth() * evalBarScale));

				whiteBar.setPosition(sf::Vector2f(blackBar.getPosition().x, blackBar.getPosition().y + blackBar.getGlobalBounds().height));
				whiteBar.setSize(sf::Vector2f(30, boardUI.getBoardWidth() - blackBar.getGlobalBounds().height));

				if (evalBarScale > 0.5) {
					evalText.setPosition(blackBar.getPosition().x + (blackBar.getGlobalBounds().width - evalText.getGlobalBounds().width) * 0.5, blackBar.getPosition().y + blackBar.getGlobalBounds().height - evalText.getCharacterSize() - 2.5);
				}
				else {
					evalText.setPosition(blackBar.getPosition().x + (blackBar.getGlobalBounds().width - evalText.getGlobalBounds().width) * 0.5, whiteBar.getPosition().y + 2.5);
				}
			}
		}

		// process inputs for SFML and ImGui
		processInputs();
		ImGui::SFML::Update(window, deltaClock.restart());
		
		ImGui::PushFont(mainFont);

		// promotion pop up
		if (auto move = promotionPopUp.renderPromotionPopUp()) {
			pgnMoves.push_back(kronosEngine->getPgnMove(move.value()));
			kronosEngine->makeMove(move.value());
			previewPly = pgnMoves.size();
			if (kronosEngine->getGameState() != KRONOS::GAME_STATE::PLAYING) {
				renderEndOfGamePopUp = true;
			}
			else if (kronosEngine->getGameType() == KRONOS::GAME_TYPE::HUMAN_VS_AI) {
				kronosEngine->startSearchForBestMove();
			}
			else {
				if (kronosEngine->getGameType() == KRONOS::GAME_TYPE::HUMAN_VS_HUMAN)
					flipBoard();
				kronosEngine->beginInfiniteSearch();
			}
		}

		// menu bar
		if (ImGui::BeginMainMenuBar()) {
			// import a position through FEN			
			if (ImGui::BeginMenu("Import")) {
				if (ImGui::BeginMenu("FEN")) {
					static char buffer[100];
					ImGui::PushItemWidth(500.f);
					if (ImGui::InputText("##", buffer, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
						kronosEngine->stopInfiniteSearch();
						kronosEngine->stopTimedSearch();
						kronosEngine->createGame<KRONOS::GAME_TYPE::ANALYSIS>(std::string(buffer));
						kronosEngine->beginInfiniteSearch();
						pgnMoves.clear();
						previewPly = 0;
						memset(buffer, 0, sizeof(buffer));
					}
					ImGui::PopItemWidth();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			// export a position through FEN
			if (ImGui::BeginMenu("Export")) {
				static char buffer[100];
				strcpy(buffer, kronosEngine->getFen().c_str());
				ImGui::PushItemWidth(500.f);
				ImGui::InputText("##fe", buffer, 100, ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemWidth();
				ImGui::EndMenu();
			}

			// settings page
			if (ImGui::BeginMenu("Settings")) {
				// flips the board
				if (ImGui::Button("Flip Board", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
					previousEval = 32001;
					flipBoard();
				}
				// settings for kronos
				if (ImGui::BeginMenu("Kronos Settings")) {

					ImGui::BeginChild("##ks", ImVec2(470, 130), true);

					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 300);

					// change the number of cores
					static int numCores = 0;
					createKronosSetting(
						"Enter new number of cores: ",
						"##nc",
						"Enter##nc",
						[&](int x) { kronosEngine->setNumCores(x); },
						numCores,
						1,
						std::thread::hardware_concurrency());

					// change the size of the transposition table
					static int transSize = 0;
					createKronosSetting(
						"Enter new transposition table size (MB): ",
						"##tts",
						"Enter##tts",
						[&](int x) { kronosEngine->setTransSize(x); },
						transSize,
						1,
						1024
					);

					// change the maximum depth the engine can search to
					static int maxDepth = 0;
					createKronosSetting(
						"Enter maximum depth (ply): ",
						"##md",
						"Enter##md",
						[&](int x) { kronosEngine->setSearchDepth(maxDepth); },
						maxDepth,
						1,
						KRONOS::MAX_ITERATIVE_DEPTH
					);

					// change the response time of the AI
					static int timePerMove = 0;
					createKronosSetting(
						"Enter maximum time per move (ms): ",
						"##tm",
						"Enter##tm",
						[&](int x) { kronosEngine->setTimeForSearch(timePerMove); },
						timePerMove,
						1,
						1800000
					);

					ImGui::Columns();
					ImGui::EndChild();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			// create a game
			if (ImGui::BeginMenu("Options")) {

				static bool clearWarningPopUp = false, enterGameOptionsPopUp = false, createAIgame = false, createOtB = false, createAnalysis = false;

				if (ImGui::Button("Play against the AI")) {
					createAIgame = true;
					ImGui::OpenPopup("Clear Warning");
					clearWarningPopUp = true;
				}
				if (ImGui::Button("Play Over the board")) {
					createOtB = true;
					ImGui::OpenPopup("Clear Warning");
					clearWarningPopUp = true;
				}
				if (ImGui::Button("Analysis", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
					createAnalysis = true;
					ImGui::OpenPopup("Clear Warning");
					clearWarningPopUp = true;
				}

				// makes sure that the user doesn't accidentally clear their current game
				if (ImGui::BeginPopupModal("Clear Warning", &clearWarningPopUp, ImGuiWindowFlags_AlwaysUseWindowPadding |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove)) {
					ImGui::Text("The current board will be cleared");
					ImGui::Text("Do you wish to proceed?");

					if (ImGui::Button("Yes", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.4775, 0))) {
						clearWarningPopUp = false;

						enterGameOptionsPopUp = true;

						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("No", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.4775, 0))) {
						clearWarningPopUp = false;
						createAIgame = false;
						createOtB = false;
						createAnalysis = false;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				if (enterGameOptionsPopUp)
					ImGui::OpenPopup("Enter Game Options");

				// gives the user options about the game they are about to create
				if (ImGui::BeginPopupModal("Enter Game Options", &enterGameOptionsPopUp, ImGuiWindowFlags_AlwaysUseWindowPadding |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove)) {
					ImGui::Text("Enter the FEN for the position");
					ImGui::Text("Leaving it empty will result in the starting position");

					// the user can import a position to play
					static char buffer[100];
					ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
					ImGui::InputText("##ef", buffer, 100);
					ImGui::PopItemWidth();

					static int selectedSide = KRONOS::WHITE;

					// the user can select what side they want to play as during an AI game
					if (createAIgame) {
						if (selectedSide == KRONOS::WHITE)
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
						bool selected = ImGui::Button("WHITE", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.32, 0));
						if (selectedSide == KRONOS::WHITE)
							ImGui::PopStyleColor();
						if (selected) selectedSide = KRONOS::WHITE;
						
						ImGui::SameLine();

						if (selectedSide == KRONOS::BOTH)
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
						selected = ImGui::Button("RANDOM", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.315, 0));
						if (selectedSide == KRONOS::BOTH)
							ImGui::PopStyleColor();
						if (selected) selectedSide = KRONOS::BOTH;
					
						ImGui::SameLine();

						if (selectedSide == KRONOS::BLACK)
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
						selected = ImGui::Button("BLACK", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.32, 0));
						if (selectedSide == KRONOS::BLACK)
							ImGui::PopStyleColor();
						if (selected) selectedSide = KRONOS::BLACK;
					}

					if (ImGui::Button("Enter", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
						// makes sure the threads are idle since the position is about to change
						if (kronosEngine->isInfiniting())
							kronosEngine->stopInfiniteSearch();
						else if (kronosEngine->isTimedSearching())
							kronosEngine->stopTimedSearch();

						// checks to see if the FEN is empty
						if (buffer[0] == '\0') {
							strcpy(buffer, KRONOS::FEN_START_POSITION.c_str());
						}

						// creates the games
						if (createAIgame) {
							switch (selectedSide)
							{
							case KRONOS::WHITE:
								if (!whiteBottom) flipBoard();
								playerSide = KRONOS::WHITE;
								break;
							case KRONOS::BOTH:
								playerSide = rand() % 2;
								if (playerSide != whiteBottom) flipBoard();
								break;
							case KRONOS::BLACK:
								if (whiteBottom) flipBoard();
								playerSide = KRONOS::BLACK;
								break;
							}
							kronosEngine->createGame<KRONOS::GAME_TYPE::HUMAN_VS_AI>(buffer);
							if (playerSide != kronosEngine->getStatusPointer(0)->isWhite) {
								kronosEngine->startSearchForBestMove();
							}
							createAIgame = false;
						}
						else if (createOtB) {
							kronosEngine->createGame<KRONOS::GAME_TYPE::HUMAN_VS_HUMAN>(buffer);
							if (kronosEngine->getStatusPointer(0)->isWhite) {
								if (!whiteBottom) flipBoard();
							}
							else {
								if (whiteBottom) flipBoard();
							}
							createOtB = false;
						}
						else if (createAnalysis) {
							if (!whiteBottom) flipBoard();
							kronosEngine->createGame<KRONOS::GAME_TYPE::ANALYSIS>(buffer);
							createAnalysis = false;
						}

						pgnMoves.clear();
						previewPly = 0;

						memset(&buffer, '\0', sizeof(buffer));

						enterGameOptionsPopUp = false;
						ImGui::CloseCurrentPopup();

						if (!kronosEngine->isInfiniting() && !kronosEngine->isTimedSearching())
							kronosEngine->beginInfiniteSearch();
					}

					ImGui::EndPopup();
				}

				ImGui::EndMenu();
			}

			// help page
			if (ImGui::BeginMenu("Help")) {
				static char helpText[] =
					"How to use create and export games\n"\
					"To begin a game or analysis session, click on \"options\" on the menu bar, select your desired game and follow the instructions provided.\n"\
					"If you want to import a position, please use the FEN notation and then paste it into \"import\" on the menu bar or when creating a new game.\n"\
					"If you want to export a position, click on \"export\" and you will have the option to copy the FEN of the most recent position.\n\n"\
					"How to traverse a game\n"\
					"While a game is present, on the right of the board, there is a move list that allows for the user to preview previous positions using the buttons above.\n"\
					"The undo button will remove the most recent position from the game.\n\n"\
					"How to configure KRONOS\n"\
					"Click on \"settings\" on the menu bar and then \"Kronos settings\". You will then be given options to change variables that control the engine\n"\
				;
				if (ImGui::BeginChild("##cht", ImVec2(500, 260))) {
					ImGui::TextWrapped(helpText);
					ImGui::EndChild();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// kronos information box
		if (ImGui::Begin("KRONOS Information", NULL, ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove)) {
		
			// information about the current search
			if (kronosEngine->getGameType() == KRONOS::GAME_TYPE::ANALYSIS && ImGui::BeginTable("##searchInfo", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
				ImGui::TableSetupColumn("Depth");
				ImGui::TableSetupColumn("Score");
				ImGui::TableSetupColumn("Best Move");
				ImGui::TableHeadersRow();

				ImGui::TableNextColumn();
				ImGui::Text(std::to_string(kronosEngine->getDepthSearchedTo()).c_str());
				
				ImGui::TableNextColumn();
				ImGui::Text(std::to_string(kronosEngine->getScoreEvaluated()).c_str());

				ImGui::TableNextColumn();
				ImGui::Text(kronosEngine->getBestMoveSoFar().c_str());

				ImGui::EndTable();
			}

			// information about how much hardware the engine is using
			if (ImGui::BeginTable("##hardwareInfo", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
				ImGui::TableSetupColumn("Number of cores");
				ImGui::TableSetupColumn("Transposition Size (MB)");
				ImGui::TableHeadersRow();

				ImGui::TableNextColumn();
				ImGui::Text(std::to_string(kronosEngine->getNumCores()).c_str());

				ImGui::TableNextColumn();
				ImGui::Text(std::to_string(kronosEngine->getTransSize()).c_str());

				ImGui::EndTable();
			}
			
			ImGui::End();
		}

		// moves list
		if (ImGui::Begin("Moves", NULL, ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove)) {

			// gives the user the option to analyse their finished game
			if ((kronosEngine->getGameType() == KRONOS::GAME_TYPE::HUMAN_VS_AI 
				|| kronosEngine->getGameType() == KRONOS::GAME_TYPE::HUMAN_VS_HUMAN)
				&& kronosEngine->getGameState() != KRONOS::GAME_STATE::PLAYING 
				&& ImGui::Button("Analyse Game", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {

				kronosEngine->changeGameToAnalysis();

				ImGui::CloseCurrentPopup();
				renderEndOfGamePopUp = false;
			}

			// preview buttons
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			if (ImGui::Button("<<", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2, 0))) {
				previewPly = 0;
				// change what position the engine is passively searching
				// do not start a search if the engine is searching for a new move
				if (!kronosEngine->isTimedSearching()) {
					kronosEngine->stopInfiniteSearch();
					kronosEngine->setSearchPositionIndexFirst();
					kronosEngine->beginInfiniteSearch();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("<", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2, 0))) {
				previewPly = std::max(0, previewPly - 1);
				if (!kronosEngine->isTimedSearching()) {
					kronosEngine->stopInfiniteSearch();
					kronosEngine->decrementSearchPositionIndex();
					kronosEngine->beginInfiniteSearch();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(">", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2, 0))) {
				previewPly = std::min(int(pgnMoves.size()), previewPly + 1);
				if (!kronosEngine->isTimedSearching()) {
					kronosEngine->stopInfiniteSearch();
					kronosEngine->incrementSearchPostiionsIndex();
					kronosEngine->beginInfiniteSearch();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(">>", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2, 0))) {
				previewPly = pgnMoves.size();
				if (!kronosEngine->isTimedSearching()) {
					kronosEngine->stopInfiniteSearch();
					kronosEngine->setSearchPositionIndexLast();
					kronosEngine->beginInfiniteSearch();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("undo", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2, 0))) {
				if (kronosEngine->isInfiniting())
					kronosEngine->stopInfiniteSearch();
				else if (kronosEngine->isTimedSearching())
					kronosEngine->stopTimedSearch();
				kronosEngine->unmakeMove();
				kronosEngine->beginInfiniteSearch();
				if (pgnMoves.size())
					pgnMoves.pop_back();
				previewPly = pgnMoves.size();
			}
			// table to show the moves made
			float h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginChild("##scrolling", ImVec2(0, h), true)) {
				if (ImGui::BeginTable("Moves", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders)) {
					for (int i = 0; i < pgnMoves.size(); i++) {
						ImGui::TableNextColumn();
						ImGui::SetWindowFontScale(moveFontScale);
						if (!(i % 2)) {
							int row = (i / 2) + 1;
							ImGui::Text("%d", row);
							ImGui::TableNextColumn();
						}
						if (i == previewPly - 1) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
						ImGui::Text(pgnMoves.at(i).c_str());
						if (i == previewPly - 1) ImGui::PopStyleColor();
					}
					ImGui::EndTable();
				}
				ImGui::EndChild();
			}

			ImGui::PopStyleVar();
			ImGui::End();
		}

		// player information
		if (ImGui::Begin("WHITE", NULL, ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove)) {
			ImGui::Text(std::string("Material difference: " + std::to_string(kronosEngine->getMaterialScore(KRONOS::WHITE))).c_str());
			ImGui::End();
		}
		if (ImGui::Begin("BLACK", NULL, ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove)) {
			ImGui::Text(std::string("Material difference: " + std::to_string(kronosEngine->getMaterialScore(KRONOS::BLACK))).c_str());
			ImGui::End();
		}

		// tells the user how the game ended
		if (renderEndOfGamePopUp)
			ImGui::OpenPopup("End Of Game");
		if (ImGui::BeginPopupModal("End Of Game", &renderEndOfGamePopUp, ImGuiWindowFlags_AlwaysUseWindowPadding |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove)) {
			ImGui::Text("Game Finished");
			ImGui::Text(KRONOS::GAME_STATE_STRINGS[(int)kronosEngine->getGameState()].c_str());
			if (kronosEngine->getGameType() == KRONOS::GAME_TYPE::HUMAN_VS_AI || kronosEngine->getGameType() == KRONOS::GAME_TYPE::HUMAN_VS_HUMAN) {
				if (ImGui::Button("Analyse Game", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
					kronosEngine->changeGameToAnalysis();
					ImGui::CloseCurrentPopup();
					renderEndOfGamePopUp = false;
				}
			}
			if (ImGui::Button("Close", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
				ImGui::CloseCurrentPopup();
				renderEndOfGamePopUp = false;
			}
			ImGui::EndPopup();
		}

		ImGui::PopFont();
	
		// renders the application
		render();
	}
}

void Kronos_Application::render()
{
	// clears the screen
	window.clear(sf::Color(60.f, 60.f, 60.f, 255.f));
	// renders the board
	boardUI.renderKronosBoard(window, kronosEngine->getBoard(previewPly), whiteBottom);
	// renders the evaluation bar
	window.draw(whiteBar);
	window.draw(blackBar);
	window.draw(evalText);
	// renders the ImGui elements
	ImGui::SFML::Render(window);
	// displays the window
	window.display();
}

void Kronos_Application::processInputs() {

	// loops through each input event
	while (window.pollEvent(events)) {
		static sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		mousePos = sf::Mouse::getPosition(window);
		sf::Vector2i relPos;

		ImGui::SFML::ProcessEvent(events);

		// checks to see if the user has requested to close the window
		if (events.type == sf::Event::Closed) {
			window.close();
			running = false;
		}

		// checks for key presses
		if (events.type == sf::Event::KeyPressed) {
			if (events.key.code == sf::Keyboard::Left) {
				previewPly = std::max(0, previewPly - 1);
				if (!kronosEngine->isTimedSearching()) {
					kronosEngine->stopInfiniteSearch();
					kronosEngine->decrementSearchPositionIndex();
					kronosEngine->beginInfiniteSearch();
				}
			}
			if (events.key.code == sf::Keyboard::Right) {
				previewPly = std::min(int(pgnMoves.size()), previewPly + 1);
				if (!kronosEngine->isTimedSearching()) {
					kronosEngine->stopInfiniteSearch();
					kronosEngine->incrementSearchPostiionsIndex();
					kronosEngine->beginInfiniteSearch();
				}
			}
		}
	
		// checks for mouse presses
		if (events.type == sf::Event::MouseButtonPressed) {
			if (events.key.code == sf::Mouse::Left) {
				// attempts to select a piece
				if (kronosEngine->getGameState() == KRONOS::GAME_STATE::PLAYING && previewPly == pgnMoves.size() && boardUI.getBoardSpritePointer()->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					boardUI.selectPiece(window, kronosEngine->getBitBoardsPointer(), kronosEngine->getMovesPointer(), kronosEngine->getStatusPointer(pgnMoves.size())->isWhite, whiteBottom);
				}
			}
		}

		// checks for mouse releases
		if (events.type == sf::Event::MouseButtonReleased) {
			if (events.key.code == sf::Mouse::Left) {
				// attempts to drop a piece
				if (boardUI.getBoardSpritePointer()->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					if (auto move = boardUI.dropPiece(window, kronosEngine->getBitBoardsPointer(), whiteBottom)) {
						kronosEngine->stopInfiniteSearch();
						if (move.value().flag & KRONOS::PROMOTION) {
							promotionPopUp.createPromotionPopUp(move.value());
						}
						else {
							pgnMoves.push_back(kronosEngine->getPgnMove(move.value()));
							kronosEngine->makeMove(move.value());
							previewPly = pgnMoves.size();
							if (kronosEngine->getGameState() != KRONOS::GAME_STATE::PLAYING ) {
								renderEndOfGamePopUp = true;
							}
							else if (kronosEngine->getGameType() == KRONOS::GAME_TYPE::HUMAN_VS_AI) {
								kronosEngine->startSearchForBestMove();
							}
							else {
								if (kronosEngine->getGameType() == KRONOS::GAME_TYPE::HUMAN_VS_HUMAN)
									flipBoard();
								kronosEngine->beginInfiniteSearch();
							}
						}
					}
				}
			}
		}

	}

}

void Kronos_Application::flipBoard()
{
	// changes the positions of UI elements when the board is flipped
	whiteBottom = !whiteBottom;
	if (whiteBottom == true) {
		blackBar.setPosition(evalBarPos);
		whiteBar.setPosition(sf::Vector2f(blackBar.getPosition().x, blackBar.getPosition().y + blackBar.getGlobalBounds().height));

		ImGui::SetWindowPos("BLACK", ImVec2(playerInfoPos.x, playerInfoPos.y));
		ImGui::SetWindowPos("WHITE", ImVec2(playerInfoPos.x, playerInfoPos.y + playerInfoOffset));
	}
	else {
		whiteBar.setPosition(evalBarPos);
		blackBar.setPosition(sf::Vector2f(whiteBar.getPosition().x, whiteBar.getPosition().y + whiteBar.getGlobalBounds().height));

		ImGui::SetWindowPos("WHITE", ImVec2(playerInfoPos.x, playerInfoPos.y));
		ImGui::SetWindowPos("BLACK", ImVec2(playerInfoPos.x, playerInfoPos.y + playerInfoOffset));
	}
}
