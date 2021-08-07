#ifndef PLAYER_SELECTOR_H
#define PLAYER_SELECTOR_H

#include <vector>
#include <sstream>
#include <fstream>

#include <dirent.h>
#include <SDL.h>

#include "../Definitions.h"
#include "Selector.h"
#include "TextInput.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/AssetManager.h"
#include "../Game/Game.h"

class PlayerSelector : public Selector {
public:
	PlayerSelector();
	PlayerSelector(std::string univ);
	~PlayerSelector() = default;

	void handleEvents(Event& e);
	void draw() { drawScroll(); drawOverlay(); }
	void drawOverlay();

	void resize(Rect* rect = nullptr);

private:
	void newPlayer();
	void deletePlayer(int idx);

	std::string universe = "";

	std::vector<std::string> files;
	void loadFiles();

	Rect buttonPlay, buttonDelete;
	Rect buttonNew, inputName;
	TextInput input;
	SDL_Texture* drawItem(int idx);
};

#endif