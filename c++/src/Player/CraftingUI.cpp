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

const SDL_Color R_SELECT_COLOR{ 255,175,64,255 };

// Delay before showing a recipe's crafting station
constexpr int HOVER_DELAY = 500;

// CraftingUI
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

	ingredients.mRect = Rect(mRect.x + itemW, recipeRect.y2(),
		itemW * (R_DIM.x - 2), itemW);

	Rect craftRect = Rect(0, 0, BUTTON_W, BUTTON_W);
	craftRect.setCX(ingredients.mRect.x2() + HALF);
	craftRect.setCY(ingredients.mRect.cY());
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
		updateMe = cIt->id != bIt->second.second
			|| cIt->pos != bIt->second.first;
		++cIt; ++bIt;
	}

	if (updateMe) {
		int prevSelected = cSelected < 0 ? cSelected : crafters[cSelected].id;
		int newSelected = -1;
		crafters.clear();
		std::vector<Asset> assetVec;

		for (const auto& pair : blocks) {
			crafters.push_back(Crafter{ pair.second.second,
				pair.second.first });
			assetVec.push_back(Asset{ true, "",
				Tile::getTile(pair.second.second)
				->getImage(pair.second.first) });
			if (pair.second.second == prevSelected) {
				newSelected = crafters.size() - 1;
			}
		}

		crafterSpinner.set(assetVec);
		selectCrafter(newSelected);

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

	recipeIdxs.clear();
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
			// TODO: check can craft - ask player inventory and block
			// Add the recipe
			recipeIdxs.push_back(
				std::make_pair(idx, crafters[idx].recipes.size()));
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
		
		// Check mouse hover
		int tmp = rHover;
		if (SDL_PointInRect(&e.mouse, &recipeRect)) {
			int tmp = rHover;
			rHover = std::floor(
				(e.mouse.y - recipeRect.y + rScroll) / itemW) * R_DIM.x
				+ std::floor((e.mouse.x - recipeRect.x) / itemW);
			if (rHover >= getRecipeList().size()) { rHover = -1; }
		} else { rHover = -1; }
		if (rHover == -1 || tmp != rHover) {
			rHoverTime = 0;
		} else { rHoverTime += e.dt.milliseconds(); }

		// Check dragging selected
		if (rSelected.recipe) {
			ingredients.dragV(e);
			if (ingredients.isDragging()) {
				handled = true;
			}
		}

		// Left click
		if (!ingredients.isDragging()
			&& any8(e[Event::Mouse::LEFT], Event::Button::M_CLICKED)) {
			bool tmp = handled;
			handled = true;
			if (craftButton.clicked(e.mouse)) {
				std::cerr << "Do Craft" << std::endl;
			// Select crafter
			} else if (SDL_PointInRect(&e.mouse, &craftersRect)) {
				int clickIdx = crafterSpinner.mouseOnItem();
				if (clickIdx >= 0 && clickIdx < crafters.size()) {
					selectCrafter(clickIdx == cSelected ? -1 : clickIdx);
					rSelected.recipe.reset();
					ingredients.reset();
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
			// Select recipe
			} else if (SDL_PointInRect(&e.mouse, &recipeRect)) {
				auto& rVec = getRecipeList();
				if (rHover >= 0 && rHover < rVec.size()) {
					// Get the recipe from the current list
					rSelected.recipe = rVec[rHover];
					ingredients.reset();
					if (rSelected.recipe) {
						ingredients.maxScroll = std::max(0,
							itemW * (int)rSelected.recipe->getIngredients().size()
							- ingredients.mRect.w);
					}
					// Switch to the corresponding crafter
					if (cSelected == -1) {
						selectCrafter(recipeIdxs[rHover].first);
						rVec = getRecipeList();
						rSelected.cRIdx = recipeIdxs[rHover].second;
						// Attempt to center on the item
						rScroll = std::min(std::max(itemW *
							((int)std::floor(rSelected.cRIdx / R_DIM.x) - 1),
							0), rMaxScroll);
					} else {
						rSelected.cRIdx = rHover;
					}
					rSelected.cIdx = cSelected;
				}
			} else if (SDL_PointInRect(&e.mouse, &optionsRect)) {
				std::cerr << "Ingredient Options" << std::endl;
			} else if (SDL_PointInRect(&e.mouse, &resultRect)) {
				std::cerr << "Recipe Result" << std::endl;
			} else if (SDL_PointInRect(&e.mouse, &ingredients.mRect)) {
				if (rSelected.recipe) {
					std::cerr << "Show Ingredient Options" << std::endl;
				}
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
	rectData.set(ingredients.mRect).render(assets);

	craftButton.draw();
	crafterSpinner.draw();
	drawRecipes();
}

void CraftingUI::drawRecipes() {
	AssetManager& assets = Window::Get().assets();
	// Draw recipies
	RectData({ { 0,255,128,128 }, SDL_BLENDMODE_BLEND }).
		set(recipeRect).render(assets);
	const std::vector<RecipePtr>& rVec = getRecipeList();
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

	// Selected recipe
	if (rSelected.recipe && rSelected.cIdx == cSelected) {
		SDL_Point topLeft = r.topLeft();
		// Outline selected in recipe rect
		int idx = rSelected.cRIdx - lb;
		r.x += itemW * (idx % R_DIM.x);
		r.y += itemW * std::floor(idx / R_DIM.x);
		RectData({ R_SELECT_COLOR, SDL_BLENDMODE_NONE, recipeRect })
			.set(r, (itemW - imgW) / 2).render(assets);
		
		if (rSelected.recipe) {
			// Draw recipe result
			Inventory::drawItem(rSelected.recipe->getResult(),
				textData, resultRect);
			// Draw recipe ingredients
			int iLB = R_DIM.x * std::floor(ingredients.scroll / itemW);
			r.setCX(ingredients.mRect.x + itemW / 2
				- ((int)ingredients.scroll % itemW));
			r.setCY(ingredients.mRect.y + itemW / 2);
			auto iIter = rSelected.recipe->getIngredients().begin();
			auto iEnd = rSelected.recipe->getIngredients().end();
			std::advance(iIter, std::floor(ingredients.scroll / itemW));
			while (r.x < ingredients.mRect.x2() && iIter != iEnd) {
				Inventory::drawItem(iIter->second, textData,
					r, ingredients.mRect);
				r.x += itemW;
				iIter++;
			}
		}

		r.setTopLeft(topLeft);
	}

	// Draw the result items
	while (lb < ub) {
		Inventory::drawItem(rVec[lb]->getResult(), textData, r, recipeRect);
		// Move the rect
		lb++;
		if (lb % R_DIM.x == 0) {
			r.y += itemW;
			r.setCX(recipeRect.x + itemW / 2);
		} else { r.x += itemW; }
	}

	// Draw recipe hover
	if (rHover >= 0 && rHoverTime >= HOVER_DELAY
		&& rHover < recipeIdxs.size() && cSelected == -1) {
		const Crafter& crafter = crafters[recipeIdxs[rHover].first];

		SDL_Point mouse = mousePos();
		r.x = mouse.x; r.setY2(mouse.y);
		CircleData({ {100,100,255,128}, SDL_BLENDMODE_BLEND })
			.set(r.center(), itemW / 2).render(assets);
		CircleData({ {200,200,64,255}, SDL_BLENDMODE_BLEND })
			.set(r.center(), itemW / 2, -2).render(assets);
		RenderData imgData;
		imgData.asset.setTexture(
			Tile::getTile(crafter.id)->getImage(crafter.pos));
		imgData.fitToAsset(assets, r.w, r.h);
		imgData.dest.setCenter(r.cX(), r.cY());
		assets.drawTexture(imgData);
	}
}

std::vector<RecipePtr>& CraftingUI::getRecipeList() {
	return cSelected == -1 ? recipes : crafters[cSelected].recipes;
}

void CraftingUI::selectCrafter(int newCrafter) {
	if (newCrafter >= crafters.size()) { newCrafter = -1; }
	crafterSpinner.setSelected(cSelected, false);
	cSelected = newCrafter;
	crafterSpinner.setSelected(cSelected, true);
}

void CraftingUI::setOpen(bool val) {
	open = val;
}

void CraftingUI::toggleOpen() {
	setOpen(!open);
}
