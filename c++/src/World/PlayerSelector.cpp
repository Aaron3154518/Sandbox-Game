#include "PlayerSelector.h"
#include "UniverseSelector.h"

PlayerSelector::PlayerSelector() {
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

PlayerSelector::PlayerSelector(std::string univ) : PlayerSelector() {
	universe = univ;
}

void PlayerSelector::handleEvents(Event& e) {
	Selector::handleEvents(e);
	if (!running) { return; }
	if (SDL_PointInRect(&e.mouse, &mRect)) {
		SDL_Point mouse = e.mouse - mRect.topLeft();
		if (SDL_PointInRect(&mouse, &scrollRect)) {
			scroll = std::max(0,
				std::min(maxScroll, scroll - e.scroll * scrollAmnt));
			// Left click in scroll window
			if (e.clicked(e.left)) {
				mouse -= scrollRect.topLeft();
				mouse.y += scroll;
				int idx = (int)(mouse.y / itemH);
				// Clicked player or delete button
				if (idx < files.size() && mouse.x >= buttonPlay.x) {
					if (std::fmod(mouse.y, itemH) < buttonPlay.h) {
						if (universe.empty()) {
							nextUIs.push_back(std::make_shared<UniverseSelector>(files[idx]));
						} else {
							nextUIs.push_back(std::make_shared<Game>(files[idx], universe));
						}
						running = false;
						return;
					} else {
						deletePlayer(idx);
					}
				}
			}
		} else if (SDL_PointInRect(&mouse, &buttonNew) && e.clicked(e.left)) {
			newPlayer();
		}
	}
	if (e.keyReleased(SDLK_RETURN)) { newPlayer(); }
	input.handleEvents(e);
}

void PlayerSelector::drawOverlay() {
	// Draw current text input
	Rect r = inputName + mRect.topLeft();
	input.setRect(r);
	input.render();

	// Draw add button
	r = buttonNew + mRect.topLeft();
	UI::assets().drawTexture(ADD_IMG, r, NULL);
}

void PlayerSelector::resize(Rect* rect) {
	Selector::resize(rect);
	scrollRect.h -= itemH;

	int half = (int)(itemH / 2);
	int quarter = (int)(itemH / 4);
	int eigth = (int)(itemH / 8);
	buttonPlay = Rect(itemW - half, 0, half, half);
	buttonDelete = Rect(buttonPlay.x, buttonPlay.y2(), half, half);
	SDL_Point offset{ scrollRect.x, scrollRect.y2() };
	buttonNew = Rect(itemW - half - quarter, quarter, half, half) + offset;
	inputName = Rect(eigth, eigth, itemW - itemH, itemH - quarter) + offset;

	maxScroll = std::max(0, (int)files.size() * itemH - scrollRect.h);
}

void PlayerSelector::loadFiles() {
	files.clear();
	DIR* playerDir = opendir(PLAYERS);
	struct dirent* en;
	if (playerDir) {
		while ((en = readdir(playerDir)) != NULL) {
			std::string name = en->d_name;
			if (isFile(createFile(PLAYERS, name, ""))) {
				size_t idx = name.find_last_of(".");
				if (idx != std::string::npos && name.substr(idx) == PLAYER_EXT) {
					files.push_back(toDisplayName(name.substr(0, idx)));
				}
			}
		}
		closedir(playerDir);
	}
}

SDL_Texture* PlayerSelector::drawItem(int idx) {
	if (idx >= files.size()) { return NULL; }

	SDL_Texture* tex = SDL_CreateTexture(UI::renderer(),
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, itemW, itemH);
	UI::setRenderTarget(tex);

	UI::setDrawColor(BLACK);
	SDL_RenderFillRect(UI::renderer(), NULL);
	UI::resetDrawColor();

	UI::assets().drawTexture(PLAY_IMG, buttonPlay, NULL);
	UI::assets().drawTexture(DELETE_IMG, buttonDelete, NULL);

	itemText.text = files[idx];
	Rect r(0, 0, itemW - buttonPlay.w, itemH);
	UI::assets().drawText(itemText, &r);

	UI::resetRenderTarget();
	return tex;
}

void PlayerSelector::newPlayer() {
	std::string fileName = toFileName(input.getInput());
	if (fileName.empty()) { return; }
	std::string fullFile = createFile(PLAYERS, fileName, PLAYER_EXT);
	if (!isFile(fullFile)) {
		std::ofstream file(fullFile);
		file << "Wassup World" << std::endl;
		file.close();
		files.push_back(input.getInput());
		maxScroll += itemH;
		scroll = maxScroll;
		input.clearInput();
	}
}

void PlayerSelector::deletePlayer(int idx) {
	if (idx < 0 || idx >= files.size()) { return; }
	auto it = files.begin() + idx;
	std::string fullFile = createFile(PLAYERS, toFileName(*it), PLAYER_EXT);
	if (isFile(fullFile)) {
		std::remove(fullFile.c_str());
		files.erase(it);
		maxScroll -= itemH;
		scroll = std::min(scroll, maxScroll);
	}
}
