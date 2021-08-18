#ifndef UNIVERSE_SELECTOR_H
#define UNIVERSE_SELECTOR_H

#include <iostream>
#include <sys/stat.h>

#include <SDL.h>

#include "../Definitions.h"
#include "Selector.h"
#include "Game.h"
#include "../Utils/Rect.h"

class UniverseSelector : public Selector {
public:
	UniverseSelector();
	UniverseSelector(std::string _player);
	~UniverseSelector() = default;
private:
	bool newItem();
	bool deleteItem(int idx);
	void selectItem(int idx);
	void loadFiles();

	std::string player = "";

	static bool createUniverse(std::string dirName);
	static bool deleteUniverse(std::string dirName);
};

#endif