#ifndef CRAFTING_UI_H
#define CRAFTING_UI_H

#include <iostream>
#include <map>
#include <set>

#include <SDL.h>

#include "../Definitions.h"
#include "../ID/Items.h"
#include "../ID/Tiles.h"
#include "../Objects/Tile.h"
#include "../UIElements/Button.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/Rect.h"

class CraftingUI {
	friend class PlayerInventory;
public:
	CraftingUI();
	~CraftingUI() = default;

	void handleEvents(Event& e);
	void draw();

	void setOpen(bool val);
	void toggleOpen();

private:
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
	// Selected crafter and recipe
	tile::Id cSelected = tile::Id::AIR;

	const static SDL_Point DIM;
	const static std::string CRAFT;
	static std::set<Recipe> HAND_CRAFTS;
};

#endif