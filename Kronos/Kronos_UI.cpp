#include "Kronos_UI.h"

Kronos_Board_UI::Kronos_Board_UI() {
	sprites[KRONOS::WHITE][KRONOS::PAWN]  .set("Images/chess_sprite_sheet.png", sf::IntRect(      0, 0, 256, 256));
	sprites[KRONOS::WHITE][KRONOS::KNIGHT].set("Images/chess_sprite_sheet.png", sf::IntRect(    256, 0, 256, 256));
	sprites[KRONOS::WHITE][KRONOS::BISHOP].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 2, 0, 256, 256));
	sprites[KRONOS::WHITE][KRONOS::ROOK]  .set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 3, 0, 256, 256));
	sprites[KRONOS::WHITE][KRONOS::QUEEN] .set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 4, 0, 256, 256));
	sprites[KRONOS::WHITE][KRONOS::KING]  .set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 5, 0, 256, 256));

	sprites[KRONOS::BLACK][KRONOS::PAWN]  .set("Images/chess_sprite_sheet.png", sf::IntRect(      0, 256, 256, 256));
	sprites[KRONOS::BLACK][KRONOS::KNIGHT].set("Images/chess_sprite_sheet.png", sf::IntRect(    256, 256, 256, 256));
	sprites[KRONOS::BLACK][KRONOS::BISHOP].set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 2, 256, 256, 256));
	sprites[KRONOS::BLACK][KRONOS::ROOK]  .set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 3, 256, 256, 256));
	sprites[KRONOS::BLACK][KRONOS::QUEEN] .set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 4, 256, 256, 256));
	sprites[KRONOS::BLACK][KRONOS::KING]  .set("Images/chess_sprite_sheet.png", sf::IntRect(256 * 5, 256, 256, 256));


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

void Kronos_Board_UI::selectPiece(sf::RenderWindow& window, KRONOS::Board* board, KRONOS::Move_List* moves, bool isWhite, bool whiteBottom) {
	sf::Vector2i relPos = sf::Mouse::getPosition(window) - sf::Vector2i(boardPosition);
	int boardIndex = getBoardIndex(relPos.x, relPos.y);
	if (!whiteBottom)
		boardIndex = 63 - boardIndex;

	for (auto& piece : { KRONOS::PAWN, KRONOS::KNIGHT, KRONOS::BISHOP, KRONOS::ROOK, KRONOS::QUEEN, KRONOS::KING }) {
		if (board->pieceLocations[isWhite][piece] & (1ULL << boardIndex)) {
			selected.pos = boardIndex;
			selected.sprite = &sprites[isWhite][piece];
			selected.colour = isWhite;
			selected.type = piece;
			for (int i = 0; i < moves->size(); i++) {
				if (moves->at(i).from == boardIndex) {
					selected.moves.push_back(moves->at(i));
				}
			}
			break;
		}
	}
}

std::optional<KRONOS::Move> Kronos_Board_UI::dropPiece(sf::RenderWindow& window, KRONOS::Board* board, bool whiteBottom) {
	if (selected.pos != KRONOS::no_Tile) {
		sf::Vector2i relPos = sf::Mouse::getPosition(window) - sf::Vector2i(boardPosition);
		int boardIndex = getBoardIndex(relPos.x, relPos.y);
		if (!whiteBottom)
			boardIndex = 63 - boardIndex;

		if (!selected.moves.empty()) {
			for (auto m : selected.moves) {
				if (m.to == boardIndex) {
					selected = SelectedPiece();
					return m;
				}
			}
		}
	}
	selected = SelectedPiece();
	return {};
}

void Kronos_Board_UI::renderKronosBoard(sf::RenderWindow& window, KRONOS::Board* board, bool whiteBottom) {
	window.draw(boardSprite);

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

PromotionPopUp::PromotionPopUp() : showPromotionPopUp(false), flag(0) {}
PromotionPopUp::~PromotionPopUp() {}

void PromotionPopUp::createPromotionPopUp(KRONOS::Move move) {
	this->move = move;
	showPromotionPopUp = true;
	flag = move.flag & KRONOS::CAPTURE;
}

std::optional<KRONOS::Move> PromotionPopUp::renderPromotionPopUp() {
	if (showPromotionPopUp) {
		ImGui::OpenPopup("Promotion Pop Up");
		if (ImGui::BeginPopupModal("Promotion Pop Up", NULL, ImGuiWindowFlags_AlwaysUseWindowPadding |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Select piece to promote to");
			if (ImGui::Button("Queen", buttonSize)) {
				showPromotionPopUp = false;
				ImGui::CloseCurrentPopup();
				return KRONOS::Move(move.from, move.to, flag | KRONOS::QUEEN_PROMOTION, move.moved_Piece);
			}
			ImGui::SameLine();
			if (ImGui::Button("Rook", buttonSize)) {
				showPromotionPopUp = false;
				ImGui::CloseCurrentPopup();
				return KRONOS::Move(move.from, move.to, flag | KRONOS::ROOK_PROMOTION, move.moved_Piece);
			}
			ImGui::SameLine();
			if (ImGui::Button("Bishop", buttonSize)) {
				showPromotionPopUp = false;
				ImGui::CloseCurrentPopup();
				return KRONOS::Move(move.from, move.to, flag | KRONOS::BISHOP_PROMOTION, move.moved_Piece);
			}
			ImGui::SameLine();
			if (ImGui::Button("Knight", buttonSize)) {
				showPromotionPopUp = false;
				ImGui::CloseCurrentPopup();
				return KRONOS::Move(move.from, move.to, flag | KRONOS::KNIGHT_PROMOTION, move.moved_Piece);
			}
			ImGui::EndPopup();
		}
	}
	return {};
}