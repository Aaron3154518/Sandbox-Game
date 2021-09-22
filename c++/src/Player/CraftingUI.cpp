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
	// Get all crafting stations
	Rect r = World::toBlockRect(Game::Player().getPlacementRange());
	auto blocks = Game::World().getCraftingBlocks(r);
	// Prune duplicates
	auto bIt = blocks.begin();
	int lastKey = -1;
	while (bIt != blocks.end()) {
		// Insert more complex/generalized logic here
		if (bIt->first == lastKey) {
			lastKey = bIt->first;
			bIt = blocks.erase(bIt);
		} else {
			lastKey = bIt->first;
			++bIt;
		}
	}
	// Compare with current blocks
	bool updateMe = crafters.size() != blocks.size();
	bIt = blocks.begin();
	auto cIt = crafters.begin();
	while (!updateMe && bIt != blocks.end()) {
		updateMe = cIt->orderId != bIt->first || cIt->pos != bIt->second.first;
		++cIt; ++bIt;
	}

	if (updateMe) {
		int prevPos = cSelected == -1 ? -1 : crafters[cSelected].orderId;
		cSelected = -1;
		crafters.clear();
		std::vector<Asset> assetVec;

		for (const auto& pair : blocks) {
			crafters.push_back(Crafter{ pair.second.second, pair.first,
				pair.second.first });
			assetVec.push_back(Asset{ true, "",
				Tile::getTile(pair.second.second)
				->getImage(pair.second.first) });
			if (pair.first == prevPos) {
				cSelected = crafters.size() - 1;
			}
		}

		crafterSpinner.set(assetVec);
		crafterSpinner.setSelected(cSelected, true);

		if (cSelected == -1) { rScroll = 0; }
	}

	updateRecipes();
}

void CraftingUI::updateRecipes() {
	// Get iterators to each crafting tile's recipes
	typedef std::multimap<int, RecipePtr>::const_iterator Iterator;
	std::vector<Iterator> iters, endIters;
	iters.resize(crafters.size());
	endIters.resize(crafters.size());
	for (int i = 0; i < crafters.size(); i++) {
		crafters[i].recipes.clear();
		CraftingTile* tile = dynamic_cast<CraftingTile*>(
			Tile::getTile(crafters[i].id).get());
		if (tile) {
			iters[i] = tile->getRecipes().begin();
			endIters[i] = tile->getRecipes().end();
		}
	}

	recipes.clear();
	bool running = true;
	while (running) {
		// Find the first recipe result
		int idx = -1, currVal = INT_MAX;
		for (int i = 0; i < crafters.size(); i++) {
			// Ignored uncraftable recipes
			//while(iters[i].first != iters[i].second && !canCraft(iters[i].first->second)) { ++iters[i].first; }
			if (iters[i] != endIters[i]
				&& iters[i]->first < currVal) {
				currVal = iters[i]->first;
				idx = i;
			}
		}

		if (idx >= 0) {
			// Add the recipe
			recipes.push_back(iters[idx]->second);
			// Log the index of the recipe in the Crafter object
			crafters[idx].recipes.push_back(iters[idx]->second);
			// Move forward in the recipe list
			++iters[idx];
		} else { running = false; }
	}

	// Update scroll
	if (cSelected == -1) {
		rMaxScroll = std::max(0, itemW * (int)std::ceil(
			(double)recipes.size() / R_DIM.x) - recipeRect.h);
	} else {
		rMaxScroll = std::max(0, itemW * (int)std::ceil(
			(double)crafters[cSelected].recipes.size() / R_DIM.x)
			- recipeRect.h);
	}
	if (rScroll > rMaxScroll) { rScroll = rMaxScroll; }
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
				int clickIdx = crafterSpinner.mouseOnItem();
				if (clickIdx >= 0 && clickIdx < crafters.size()) {
					crafterSpinner.setSelected(cSelected, false);
					cSelected = clickIdx == cSelected ? -1 : clickIdx;
					crafterSpinner.setSelected(cSelected, true);
					// Update scroll
					if (cSelected == -1) {
						rMaxScroll = std::max(0, itemW * (int)std::ceil(
							(double)recipes.size() / R_DIM.x) - recipeRect.h);
					} else {
						rMaxScroll = std::max(0, itemW * (int)std::ceil(
							(double)crafters[cSelected].recipes.size() / R_DIM.x)
							- recipeRect.h);
					}
					if (rScroll > rMaxScroll) { rScroll = rMaxScroll; }
				}
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
	drawRecipes();
}

void CraftingUI::drawRecipes() {
	AssetManager& assets = Window::Get().assets();
	// Draw recipies
	RectData({ { 0,255,128,128 }, SDL_BLENDMODE_BLEND }).
		set(recipeRect).render(assets);
	const std::vector<RecipePtr>& rVec = cSelected == -1 ? recipes :
		crafters[cSelected].recipes;
	// Get top left and bot right indexes
	int lb = R_DIM.x * std::floor(rScroll / itemW);
	int ub = R_DIM.x * std::ceil((double)(rScroll + recipeRect.h) / itemW);
	if (lb < 0) { lb = 0; }
	if (lb > rVec.size()) { lb = rVec.size(); }
	if (ub > rVec.size()) { ub = rVec.size(); }
	// Setup the item rect
	Rect r(0, 0, imgW, imgW);
	r.setCX(recipeRect.x + itemW / 2);
	r.setCY(recipeRect.y + itemW / 2 - (rScroll % itemW));
	// Setup render data
	RenderData rData;
	rData.boundary = recipeRect;
	while (lb < ub) {
		// Item image
		rData.asset.setTexture(
			Item::getItem(rVec[lb]->getResult().id)->getImage());
		rData.fitToAsset(assets, r.w, r.h);
		rData.dest.setCenter(r.cX(), r.cY());
		assets.drawTexture(rData);
		// Item text
		textData.text = std::to_string(rVec[lb]->getResult().amnt);
		rData.asset.setTexture(assets.renderText(textData));
		rData.fitToAsset(assets, 0, gameVals::INV_FONT_W());
		rData.dest.setBottomRight(r.bottomRight());
		assets.drawTexture(rData);

		// Move the rect
		lb++;
		if (lb % R_DIM.x == 0) {
			r.y += itemW;
			r.setCX(recipeRect.x + itemW / 2);
		} else { r.x += itemW; }
	}
}

void CraftingUI::setOpen(bool val) {
	open = val;
}

void CraftingUI::toggleOpen() {
	setOpen(!open);
}