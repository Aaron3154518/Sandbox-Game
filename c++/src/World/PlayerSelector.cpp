#include "PlayerSelector.h"

PlayerSelector::PlayerSelector() {
	loadFiles();
}

void PlayerSelector::resize(Rect* rect) {
	Selector::resize(rect);
	scrollRect.h -= itemH;

	int half = (int)(itemH / 2);
	int quarter = (int)(itemH / 4);
	int eigth = (int)(itemH / 8);
	buttonPlay = Rect(itemW - half, 0, half, half);
	buttonDelete = Rect(buttonPlay.x, buttonPlay.y2(), half, half);
	buttonNew = Rect(itemW - half - quarter, quarter, half, half);
	inputName = Rect(eigth, eigth, itemW - itemH, itemH - quarter);
	newPlayerR = Rect(scrollRect.x, scrollRect.x2(), itemW, itemH);

	maxScroll = std::max(0, (int)files.size() * itemH - scrollRect.h);
}

void PlayerSelector::handleEvents(Event& e) {
	Selector::handleEvents(e);
}

void PlayerSelector::drawOverlay() {

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