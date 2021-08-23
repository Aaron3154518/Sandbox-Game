#ifndef WORLD_SELECTOR_H
#define WORLD_SELECTOR_H

#include <SDL.h>

#include "Selector.h"
#include "../Definitions.h"
#include "../Utils/Rect.h"

class WorldSelector : public Selector {
public:
	WorldSelector() = default;
	~WorldSelector() = default;

	void handleEvents(Event& e);
	void draw() { drawScroll(); drawOverlay(); }
	void drawOverlay();

private:
	std::vector<std::string> files;

	void loadFiles();
};

#endif