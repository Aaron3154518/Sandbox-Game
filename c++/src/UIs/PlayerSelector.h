#ifndef PLAYER_SELECTOR_H
#define PLAYER_SELECTOR_H

#include <fstream>
#include <sstream>
#include <vector>

#include <SDL.h>

#include "Game.h"
#include "Selector.h"
#include "../Definitions.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"

class PlayerSelector : public Selector {
public:
	PlayerSelector();
	PlayerSelector(std::string univ);
	~PlayerSelector() = default;
private:
	bool newItem();
	bool deleteItem(int idx);
	void selectItem(int idx);
	void loadFiles();

	std::string universe = "";
};

#endif