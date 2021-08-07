#ifndef UNIVERSE_SELECTOR_H
#define UNIVERSE_SELECTOR_H

#include <iostream>

#include <dirent.h>
#include <SDL.h>

#include "../Definitions.h"
#include "Selector.h"
#include "TextInput.h"
#include "../Utils/Rect.h"
#include "../Game/Game.h"

class UniverseSelector : public Selector {
public:
	UniverseSelector();
	UniverseSelector(std::string _player);
	~UniverseSelector() = default;

	void handleEvents(Event& e);
	void draw() { drawScroll(); drawOverlay(); }
	void drawOverlay();

private:
	SDL_Texture* drawItem(int idx);

	std::string player = "";

	std::vector<std::string> files;
	void loadFiles();

	TextInput input;
};

#endif