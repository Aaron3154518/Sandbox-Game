#include "CraftingUI.h"
#include "../UIs/Game.h"

// Notes:
// Crafting
//	- R/LClick on item W/ item amnt = max craftable/1

const int CraftingUI::ALL = -1;
const SDL_Point CraftingUI::R_DIM{ 7, 3 };
const std::string CraftingUI::FONT_ID = Window::Get().assets().addFont(
	FontData{ gameVals::INV_FONT_W(), gameVals::INV_FONT_W() });
const std::string CraftingUI::CRAFT = gameVals::images()
+ "crafting_toggle.png";

CraftingUI::CraftingUI() :
	itemW(gameVals::INV_W()), imgW(gameVals::INV_IMG_W()) {

	textData.fontId = FONT_ID;
	textData.color = WHITE;

	const int HALF = itemW / 2;
	const int BUTTON_W = itemW * 7 / 12;

	AssetManager& assets = Window::Get().assets();

	// Set up rects
	Rect invR = Game::Player().getInventory().getFullRect();
	mRect = Rect(HALF, invR.y2(),
		itemW * (R_DIM.x + 1) , itemW * (R_DIM.y + 2));

	craftersRect = Rect(mRect.x, mRect.y, itemW * R_DIM.x, itemW);
	crafterSpinner.setRect(craftersRect);

	recipeRect = Rect(mRect.x, craftersRect.y2(),
		itemW * R_DIM.x, itemW * R_DIM.y);

	optionsRect = Rect(0, 0, imgW, imgW * R_DIM.y);
	optionsRect.setCX(recipeRect.x2() + HALF);
	optionsRect.setCY(recipeRect.cY());

	resultRect = Rect(0, 0, imgW, imgW);
	resultRect.setCX(mRect.x + HALF);
	resultRect.setCY(recipeRect.y2() + HALF);

	ingredientRect = Rect(mRect.x + itemW, recipeRect.y2(),
		itemW * (R_DIM.x - 2), itemW);

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

	rMaxScroll = std::max(0, itemW *
		(int)std::ceil((double)numRecipes() / R_DIM.x) - recipeRect.h);
	if (cSelected != tile::Id::AIR
		&& crafters.find(cSelected) == crafters.end()) {
		cSelected = tile::Id::AIR;
		rScroll = 0;
	}

	updateRecipes();
}

void CraftingUI::updateRecipes() {
	for (auto& pair : crafters) {
		pair.second.clear();
		CraftingTile* tile = dynamic_cast<CraftingTile*>(
			Tile::getTile(pair.first).get());
		if (tile) {
			for (const Recipe& r : tile->getRecipes()) {
				pair.second.insert(std::make_pair(r.resultItem,
					std::make_pair(r.resultItem, r.resultAmnt)));
			}
		}
	}
}

int CraftingUI::numRecipes() {
	int numItems = 0;
	//auto cIt = crafters.find(cSelected);
	auto cIt = crafters.begin();
	if (cIt != crafters.end()) {
		numItems = cIt->second.size();
	} else {
		for (auto& pair : crafters) {
			numItems += pair.second.size();
		}
	}
	return numItems;
}

bool CraftingUI::handleEvents(Event& e) {
	updateCrafters();

	bool handled = false;
	if (!crafterSpinner.handleEvents(e)) {
		// Scroll
		if (e.checkScroll()) {
			if (SDL_PointInRect(&e.mouse, &recipeRect)) {
				rScroll = std::min(std::max(rScroll + e.scroll * itemW / 3, 0),
					rMaxScroll);
				handled = true;
			}
		}

		// Left click
		if (any8(e[Event::Mouse::LEFT], Event::Button::M_CLICKED)) {
			bool tmp = handled;
			handled = true;
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
			} else { handled = tmp; }
		}
	} else { handled = true; }
	return handled;
}

void CraftingUI::draw() {
	AssetManager& assets = Window::Get().assets();
	RectData rectData({ { 200,200,0,128 }, SDL_BLENDMODE_BLEND });
	rectData.set(craftersRect).render(assets);
	rectData.color = { 200,0,200,128 };
	rectData.set(optionsRect).render(assets);
	rectData.color = { 255,0,0,128 };
	rectData.set(resultRect).render(assets);
	rectData.color = { 128,128,128,128 };
	rectData.set(ingredientRect).render(assets);

	craftButton.draw();
	crafterSpinner.draw();

	// Draw recipies
	rectData.color = { 0,255,128,128 };
	rectData.set(recipeRect).render(assets);
	// Get number of items
	int numItems = numRecipes();
	// Get top left and bot right indexes
	int lb = R_DIM.x * (int)(rScroll / itemW);
	int ub = R_DIM.x * std::ceil((double)(rScroll + recipeRect.h) / itemW);
	if (lb < 0) { lb = 0; }
	if (ub > numItems) { ub = numItems; }
	// Draw recipes
	//auto cIt = crafters.find(cSelected);
	auto cIt = crafters.begin();
	if (cIt != crafters.end()) {
		// Move iterator to starting value
		int i = 0;
		auto rIt = cIt->second.begin();
		while (i < lb) { ++rIt; ++i; }
		// Set up the item rect
		Rect r(0, 0, imgW, imgW);
		r.setCX(recipeRect.x + itemW / 2);
		r.setCY(recipeRect.y + itemW / 2 - (rScroll % itemW));
		// Draw each item
		RenderData rData;
		rData.boundary = recipeRect;
		while (i++ < ub) {
			// Item image
			rData.asset.setTexture(
				Item::getItem(rIt->second.first)->getImage());
			rData.fitToAsset(assets, r.w, r.h);
			rData.dest.setCenter(r.cX(), r.cY());
			assets.drawTexture(rData);
			// Item text
			textData.text = std::to_string(rIt->second.second);
			rData.asset.setTexture(assets.renderText(textData));
			rData.fitToAsset(assets, 0, gameVals::INV_FONT_W());
			rData.dest.setBottomRight(r.bottomRight());
			assets.drawTexture(rData);

			// Move the rect
			if (i % R_DIM.x == 0) {
				r.y += itemW;
				r.setCX(recipeRect.x + itemW / 2);
			} else { r.x += itemW; }
			++rIt;
		}
	}
}

void CraftingUI::setOpen(bool val) {
	open = val;
}

void CraftingUI::toggleOpen() {
	setOpen(!open);
}