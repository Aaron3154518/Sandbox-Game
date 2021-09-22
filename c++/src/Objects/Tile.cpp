#include "Tile.h"
#include "../UIs/Game.h"

std::vector<TilePtr>& Tile::getTiles() {
	static std::vector<TilePtr> tiles(tile::Id::numTiles + 1);
	return tiles;
}

TilePtr Tile::getTile(tile::Id id) {
	return getTiles()[id];
}

tile::Id Tile::registerTile(TilePtr t, tile::Id id) {
	getTiles()[id] = t;
	return id;
}

bool Tile::isCrafter(tile::Id id) {
	return getTile(id)->getTileData(TileData::crafting);
}

//Tile
void Tile::tick(int x, int y, Timestep dt) {}

SharedTexture Tile::getImage(SDL_Point pos) const {
	AssetManager& assets = Window::Get().assets();
	if (animIdx == -1) {
		return assets.getAsset(gameVals::items() + img);
	} else {
		// TODO: Animation
	}
	return makeSharedTexture(NULL);
}

void Tile::setAnimation(Animation& anim) {}

void Tile::addDrop(item::Id item, int minAmnt, int maxAmnt) {
	if (minAmnt < 0) { minAmnt = 0; }
	drops.push_back(TileDrop());
	TileDrop& drop = drops.back();
	drop.item = item;
	drop.minAmnt = minAmnt;
	drop.maxAmnt = maxAmnt < 0 ? minAmnt : maxAmnt;
}

std::forward_list<ItemInfo> Tile::generateDrops() const {
	std::forward_list<ItemInfo> result;
	// TODO: random drops
	for (const TileDrop& drop : drops) {
		result.push_front(ItemInfo(
			drop.item, drop.maxAmnt == -1 ? drop.minAmnt : drop.maxAmnt));
	}
	return result;
}

bool Tile::onBreak(SDL_Point loc) {
	Point<double> pos{ (.5 + loc.x) * gameVals::BLOCK_W(),
		(.5 + loc.y) * gameVals::BLOCK_W() };
	for (ItemInfo& info : generateDrops()) {
		Game::World().dropItem(DroppedItem(info),
			DroppedItem::DropDir::none, pos);
	}
	return true;
}

bool Tile::onPlace(SDL_Point loc) {
	return true;
}

bool Tile::hit(SDL_Point loc, int power) {
	return true;
}

std::map<Tile::TileData, bool> Tile::getTileData(const DataKeys& keys) const {
	std::map<TileData, bool> result;
	for (TileData key : keys) { result[key] = data[key]; }
	return result;
}

void Tile::setTileData(const DataKeys& keys, bool val) {
	for (TileData key : keys) { data[key] = val; }
}

// Recipe
Recipe::Recipe(item::Id id, int amnt,
	std::initializer_list<ItemInfo> iList) :
	result(id, amnt) {
	for (const ItemInfo& i : iList) {
		ingredients.insert(std::make_pair(item::getItemOrder(i.itemId), i));
	}
}

Recipe::Recipe(const Recipe& other) : result(other.result) {
	for (auto& pair : other.ingredients) {
		ingredients.insert(std::make_pair(
			item::getItemOrder(pair.second.itemId), pair.second));
	}
}

bool Recipe::hasResult() const {
	return result.isItem();
}

const ItemInfo& Recipe::getResult() const {
	return result;
}

const std::multimap<int, ItemInfo>& Recipe::getIngredients() const {
	return ingredients;
}

// CraftingStation
CraftingTile::CraftingTile() {
	setTileData(TileData::crafting, true);
}

const std::multimap<int, RecipePtr>& CraftingTile::getRecipes() const {
	return recipes;
}

void CraftingTile::addRecipe(const Recipe& r) {
	recipes.insert(std::make_pair(item::getItemOrder(r.getResult().itemId),
		std::make_shared<Recipe>(r)));
}

void CraftingTile::addRecipes(std::initializer_list<Recipe> list) {
	for (const Recipe& r : list) { addRecipe(r); }
}
