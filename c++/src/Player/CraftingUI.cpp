#include "CraftingUI.h"
#include "../UIs/Game.h"

// Notes:
// Crafting
//	- R/LClick on item W/ item amnt = max craftable/1

const SDL_Point CraftingUI::DIM{ 8, 5 };
const std::string CraftingUI::CRAFT = gameVals::images()
+ "crafting_toggle.png";

std::set<Recipe> CraftingUI::HAND_CRAFTS = {
	{item::Id::WORK_TABLE, 1, {{item::Id::WOOD, 10}}}
};

CraftingUI::CraftingUI() {
	const int HALF = gameVals::INV_W() / 2;
	const int BUTTON_W = gameVals::INV_W() * 7 / 12;

	AssetManager& assets = Window::Get().assets();

	// Set up rects
	Rect invR = Game::Player().getInventory().getFullRect();
	mRect = Rect(HALF, invR.y2(), DIM.x * gameVals::INV_W(),
		DIM.y * gameVals::INV_W());

	craftersRect = Rect(mRect.x, mRect.y, mRect.w, gameVals::INV_W());

	recipeRect = Rect(mRect.x, craftersRect.y2(),
		mRect.w, mRect.h - gameVals::INV_W() * 2);

	optionsRect = Rect(0, 0, gameVals::INV_IMG_W(),
		recipeRect.h * gameVals::INV_IMG_W() / gameVals::INV_W());
	optionsRect.setCX(recipeRect.x2() + HALF);
	optionsRect.setCY(recipeRect.cY());

	resultRect = Rect(0, 0, gameVals::INV_IMG_W(), gameVals::INV_IMG_W());
	resultRect.setCX(mRect.x + HALF);
	resultRect.setCY(recipeRect.y2() + HALF);

	ingredientRect = Rect(mRect.x + gameVals::INV_W(), recipeRect.y2(),
		mRect.w - gameVals::INV_W() * 2, gameVals::INV_W());

	Rect craftRect = Rect(0, 0, BUTTON_W, BUTTON_W);
	craftRect.setCX(ingredientRect.x2() + HALF);
	craftRect.setCY(ingredientRect.cY());
	craftButton = Button(CRAFT, 75);
	craftButton.setRect(craftRect);

	// TODO: (remove this) Add all items to hand crafts + make hand_crafts const again
	for (int i = 0; i < item::Id::numItems; i++) {
		HAND_CRAFTS.insert(Recipe{ static_cast<item::Id>(i), 1 });
	}
}

void CraftingUI::handleEvents(Event& e) {
	if (any8(e[Event::Mouse::LEFT], Event::Button::M_CLICKED)) {
		if (craftButton.clicked(e.mouse)) {
			std::cerr << "Do Craft" << std::endl;
		} else if (SDL_PointInRect(&e.mouse, &craftersRect)) {
			std::cerr << "Crafting Stations" << std::endl;
		} else if (SDL_PointInRect(&e.mouse, &recipeRect)) {
			std::cerr << "Recipes" << std::endl;
		} else if (SDL_PointInRect(&e.mouse, &optionsRect)) {
			std::cerr << "Ingredient Options" << std::endl;
		} else if (SDL_PointInRect(&e.mouse, &resultRect)) {
			std::cerr << "Recipe Result" << std::endl;
		} else if (SDL_PointInRect(&e.mouse, &ingredientRect)) {
			std::cerr << "Recipe Ingredients" << std::endl;
		}
	}
}

void CraftingUI::draw() {
	AssetManager& assets = Window::Get().assets();
	assets.rect(&craftersRect, { 200,200,0,128 }, SDL_BLENDMODE_BLEND);
	assets.rect(&recipeRect, { 0,255,128,128 }, SDL_BLENDMODE_BLEND);
	assets.rect(&optionsRect, { 200,0,200,128 }, SDL_BLENDMODE_BLEND);
	assets.rect(&resultRect, { 255,0,0,128 }, SDL_BLENDMODE_BLEND);
	assets.rect(&ingredientRect, { 128,128,128,128 }, SDL_BLENDMODE_BLEND);
	craftButton.draw();
}

void CraftingUI::setOpen(bool val) {
	open = val;
}

void CraftingUI::toggleOpen() {
	setOpen(!open);
}