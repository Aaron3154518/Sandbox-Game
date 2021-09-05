#include "CraftingUI.h"
#include "../UIs/Game.h"

// Notes:
// Crafting
//	- R/LClick on item W/ item amnt = max craftable/1

const SDL_Point DIM{ 10, 5 };

std::set<Recipe> CraftingUI::HAND_CRAFTS = {
	{item::Id::WORK_TABLE, 1, {{item::Id::WOOD, 10}}}
};

CraftingUI::CraftingUI() {
	AssetManager& assets = Window::Get().assets();

	// Set up rects
	Rect invR = Game::Player().getInventory().getFullRect();
	mRect = Rect(0, invR.y2(), DIM.x * gameVals::INV_W(),
		DIM.y * gameVals::INV_W());
	recipeRect = Rect(0, 0, mRect.w, mRect.h - gameVals::INV_W())
		+ mRect.topLeft();

	// Create font (once)
	static const std::string FONT_ID = assets.addFont(
		FontData{ mRect.w, gameVals::INV_W(), "Craft" });
	// Render craft button
	TextData textData;
	textData.fontId = FONT_ID;
	textData.text = "Craft";
	textData.color = BLACK;
	textData.bkgrnd = { 96,96,96,255 };
	textData.w = mRect.w; textData.h = gameVals::INV_W();
	textData.x = mRect.x2(); textData.y = recipeRect.y2() + textData.h / 2;
	textData.xMode = TextData::PosType::botright;
	textData.yMode = TextData::PosType::center;
	TextureData texData = assets.renderText(textData);
	craftButton = Button(texData.texture, 150);
	craftButton.setRect(texData.dest);

	ingredientRect = Rect(0, recipeRect.y2(),
		mRect.w, gameVals::INV_W());

	// TODO: (remove this) Add all items to hand crafts + make hand_crafts const again
	for (int i = 0; i < item::Id::numItems; i++) {
		HAND_CRAFTS.insert(Recipe{ static_cast<item::Id>(i), 1 });
	}
}

void CraftingUI::draw() {
	AssetManager& assets = Window::Get().assets();
	assets.rect(&recipeRect, { 0,255,128,128 }, SDL_BLENDMODE_BLEND);
	assets.rect(&ingredientRect, { 128,128,128,128 }, SDL_BLENDMODE_BLEND);
	craftButton.draw();
}

void CraftingUI::setOpen(bool val) {
	open = val;
}

void CraftingUI::toggleOpen() {
	setOpen(!open);
}