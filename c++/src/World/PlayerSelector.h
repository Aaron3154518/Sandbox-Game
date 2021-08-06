#ifndef PLAYER_SELECTOR_H
#define PLAYER_SELECTOR_H

#include <dirent.h>
#include <SDL.h>

#include "../Definitions.h"
#include "Selector.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/AssetManager.h"

class PlayerSelector : public Selector {
public:
	PlayerSelector();
	~PlayerSelector() = default;

	void handleEvents(Event& e);
	void draw() { drawScroll(); drawOverlay(); }
	void drawOverlay();

	void resize(Rect* rect = nullptr);

private:
	std::vector<std::string> files;
	void loadFiles();

	Rect buttonPlay, buttonDelete;
	Rect buttonNew, inputName, newPlayerR;
	std::string currInput;
	bool cursor = false;
	SDL_Texture* drawItem(int idx);
};

#endif