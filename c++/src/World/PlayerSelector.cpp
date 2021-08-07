#include "PlayerSelector.h"

PlayerSelector::PlayerSelector() {
	textInput = true;

	loadFiles();

	inputTextData.fontId = ITEM_FONT;
	inputTextData.color = WHITE;
	inputTextData.xMode = PosType::topleft;
	inputTextData.yMode = PosType::center;
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
						std::cerr << "Play: " << idx << std::endl;
					} else {
						std::cerr << "Delete: " << idx << std::endl;
					}
				}
			}
		} else if (SDL_PointInRect(&mouse, &buttonNew) && e.clicked(e.left)) {
			std::cerr << "New: \"" << currInput.str() << "\"" << std::endl;
			//createNew();
		}
	}
	if (e.keyReleased(SDLK_RETURN)) {
		std::cerr << "New: \"" << currInput.str() << "\"" << std::endl;
		//createNew();
	} else if (e.keyPressed(SDLK_BACKSPACE)) {
		std::string str = currInput.str();
		if (str.length() > 0) { str.erase(str.end() - 1); }
		currInput.str(str);
	} else {
		currInput << e.inputText;
	}
}

void PlayerSelector::drawOverlay() {
	// Fill black
	UI::setDrawColor(BLACK);
	Rect r = inputName + mRect.topLeft();
	SDL_RenderFillRect(UI::renderer(), &inputName);
	UI::resetDrawColor();

	// Draw current text input
	inputTextData.x = r.x;
	inputTextData.y = r.cY();
	inputTextData.text = currInput.str();
	if ((int)(gameTime / 500) % 2 == 0) { inputTextData.text.append("|"); }
	UI::assets().drawText(inputTextData, &r);

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
					files.push_back(name.substr(0, idx));
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
	UI::assets().drawText(itemText, NULL);

	UI::resetRenderTarget();
	return tex;
}
