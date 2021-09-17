#ifndef CRAFTING_UI_H
#define CRAFTING_UI_H

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <SDL.h>

#include "../Definitions.h"
#include "../ID/Items.h"
#include "../ID/Tiles.h"
#include "../Objects/Tile.h"
#include "../UIElements/Button.h"
#include "../UIElements/Spinner.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/Rect.h"
#include "../Window.h"

class CraftingUI {
	friend class PlayerInventory;
public:
	CraftingUI();
	~CraftingUI() = default;

	bool handleEvents(Event& e);
	void draw();

	void setOpen(bool val);
	void toggleOpen();

private:
	void updateCrafters();
	void updateRecipes();

	void drawRecipes();

	int numRecipes();

	Rect mRect, craftersRect, recipeRect;
	Rect resultRect, ingredientRect;
	Rect optionsRect;
	bool open = false;

	TextData textData;

	// Craft button
	Button craftButton;

	// Recipes
	typedef std::multimap<int, std::pair<item::Id, int>> Recipes;
	std::map<tile::Id, Recipes> crafters;
	tile::Id cSelected = tile::Id::AIR;
	Spinner crafterSpinner;
	// Recipe scroll
	int rScroll = 0, rMaxScroll = 0;
	// Crafter of selected recipe
	tile::Id crSelected = tile::Id::AIR;
	// Selected recipe // TODO: order #
	Recipe rSelected;


	// Dimensions values
	int itemW = 0, imgW = 0;
	SDL_Point rDim = { 0,0 };

	static const int ALL;
	static const SDL_Point R_DIM;
	static const std::string FONT_ID;
	static const std::string CRAFT;
	static std::set<Recipe> HAND_CRAFTS;
};

#endif