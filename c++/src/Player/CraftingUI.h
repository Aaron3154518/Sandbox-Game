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
#include "../Utils/Rect.h"

class CraftingUI {
	friend class PlayerInventory;
public:
	CraftingUI();
	~CraftingUI() = default;

	void draw();

	void setOpen(bool val);
	void toggleOpen();

private:
	Rect mRect, recipeRect, ingredientRect;
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

	static std::set<Recipe> HAND_CRAFTS;
};

#endif