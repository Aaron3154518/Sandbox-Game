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
#include "../Utils/Utils.h"
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

	std::vector<RecipePtr>& getRecipeList();
	void selectCrafter(int newCrafter);

	Rect mRect, craftersRect, recipeRect;
	Rect resultRect, ingredientRect;
	Rect optionsRect;
	bool open = false;

	TextData textData;

	// Craft button
	Button craftButton;

	// Recipes
	std::vector<RecipePtr> recipes;
	// For each recipe, pair<crafter index, recipe index>
	std::vector<std::pair<int, int>> recipeIdxs;
	struct Crafter {
		tile::Id id = tile::Id::AIR;
		SDL_Point pos = { 0,0 };
		std::vector<RecipePtr> recipes;
		int rSelected = -1;
	};
	std::vector<Crafter> crafters;
	int cSelected = -1, rHover = -1;
	Spinner crafterSpinner;
	struct SelectedRecipe {
		int cIdx = -1, cRIdx = -1;
		tile::Id cId = tile::Id::AIR;
		SDL_Point pos = { 0,0 };
		RecipePtr recipe;
	};
	SelectedRecipe rSelected;
	// Recipe scroll
	int rScroll = 0, rMaxScroll = 0;
	// Time hovering over recipe
	int rHoverTime = 0;


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