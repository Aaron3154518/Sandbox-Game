#ifndef UNIVERSE_SELECTOR_H
#define UNIVERSE_SELECTOR_H

#include <dirent.h>
#include <SDL.h>

#include "../Definitions.h"
#include "Selector.h"
#include "../Utils/Rect.h"

class UniverseSelector : public Selector {
public:
	UniverseSelector() = default;
	~UniverseSelector() = default;

	void handleEvents(Event& e);
	void draw() { drawScroll(); drawOverlay(); }
	void drawOverlay();

private:
	std::vector<std::string> files;

	void loadFiles();
	SDL_Texture* drawItem(int idx);
};

#endif