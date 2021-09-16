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

	Rect mRect, craftersRect, recipeRect;
	Rect resultRect, ingredientRect;
	Rect optionsRect;
	bool open = false;

	// Craft button
	Button craftButton;

	// Recipes
	struct Crafter {
		std::set<Recipe> canCraft, cantCraft;

		// Recipe and ingredient scroll
		int rScroll = 0, iScroll = 0;
		int maxRScroll = 0, maxIScroll = 0;

		// Selected recipe
		Recipe rSelected;
	};
	std::map<tile::Id, Crafter> crafters;
	Spinner crafterSpinner;
	// Selected recipe
	bool rIsSelected = false;
	Recipe rSelected;

	static const int ALL;
	static const SDL_Point DIM;
	static const std::string CRAFT;
	static std::set<Recipe> HAND_CRAFTS;
};

#endif