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
private:
	bool newItem();
	bool deleteItem(int idx);
	void selectItem(int idx);
	void loadFiles();

	std::string universe = "";
};

#endif