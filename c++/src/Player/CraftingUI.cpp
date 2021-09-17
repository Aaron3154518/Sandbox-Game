#include "CraftingUI.h"
#include "../UIs/Game.h"

// Notes:
// Crafting
//	- R/LClick on item W/ item amnt = max craftable/1

const int CraftingUI::ALL = -1;
const SDL_Point CraftingUI::DIM{ 8, 5 };
const std::string CraftingUI::CRAFT = gameVals::images()
+ "crafting_toggle.png";

CraftingUI::CraftingUI() {
	const int HALF = gameVals::INV_W() / 2;
	const int BUTTON_W = gameVals::INV_W() * 7 / 12;

	AssetManager& assets = Window::Get().assets();

	// Set up rects
	Rect invR = Game::Player().getInventory().getFullRect();
	mRect = Rect(HALF, invR.y2(), DIM.x * gameVals::INV_W(),
		DIM.y * gameVals::INV_W());

	craftersRect = Rect(mRect.x, mRect.y, mRect.w, gameVals::INV_W());
	crafterSpinner.setRect(craftersRect);

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
}

void CraftingUI::updateCrafters() {
	std::map<tile::Id, int> updates;
	std::vector<Asset> assetVec;
	// Get all crafting stations
	Rect r = World::toBlockRect(Game::Player().getPlacementRange());
	for (auto& pair : Game::World().getCraftingBlocks(r)) {
		int& val = updates[pair.second];
		// No duplicates
		if (val == 0) {
			--val;
			assetVec.push_back(Asset{ true, "",
				Tile::getTile(pair.second)->getImage(pair.first) });
		}
	}
	// Compare with previously in range
	for (auto& pair : crafters) { updates[pair.first]++; }
	// Check for updates
	bool updateMe = false;
	for (auto& pair : updates) {
		if (pair.second != 0) {
			updateMe = true;
			if (pair.second == 1) {
				crafters.erase(crafters.find(pair.first));
			} else {
				crafters[pair.first];
			}
		}
	}
	if (updateMe) {
		crafterSpinner.set(assetVec);
	}
	// Update recipes
}

bool CraftingUI::handleEvents(Event& e) {
	updateCrafters();

	if (!crafterSpinner.handleEvents(e)) {
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
			} else { return false; }
			return true;
		}
	} else { return true; }
	return false;
}

void CraftingUI::draw() {
	AssetManager& assets = Window::Get().assets();
	RectData rectData({ { 200,200,0,128 }, SDL_BLENDMODE_BLEND });
	rectData.set(craftersRect).render(assets);
	rectData.color = { 0,255,128,128 };
	rectData.set(recipeRect).render(assets);
	rectData.color = { 200,0,200,128 };
	rectData.set(optionsRect).render(assets);
	rectData.color = { 255,0,0,128 };
	rectData.set(resultRect).render(assets);
	rectData.color = { 128,128,128,128 };
	rectData.set(ingredientRect).render(assets);

	craftButton.draw();
	crafterSpinner.draw();
}

void CraftingUI::setOpen(bool val) {
	open = val;
}

void CraftingUI::toggleOpen() {
	setOpen(!open);
}