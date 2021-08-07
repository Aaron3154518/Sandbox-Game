#include "UniverseSelector.h"

UniverseSelector::UniverseSelector() {
	textInput = true;

	loadFiles();

	TextData td;
	td.fontId = ITEM_FONT;
	td.color = WHITE;
	td.xMode = PosType::topleft;
	td.yMode = PosType::center;
	input.setBackground(BLACK);
	input.setTextData(td);
	input.setCharConstraint([](const char& ch) {
		return isalnum(ch) || ch == ' ';
		});
}

UniverseSelector::UniverseSelector(std::string _player) : UniverseSelector() {
	player = _player;
}

void UniverseSelector::handleEvents(Event& e) {
	nextUIs.push_back(std::make_shared<Game>(player, "universe"));
	running = false;
}

void UniverseSelector::drawOverlay() {

}

void UniverseSelector::loadFiles() {

}

SDL_Texture* UniverseSelector::drawItem(int idx) {

}