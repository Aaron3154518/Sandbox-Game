#include "Tile.h"

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

//Tile
SDL_Texture* Tile::getImage(SDL_Point pos) const {
	AssetManager& assets = UI::assets();
	if (animIdx == -1) {
		return assets.getAsset(gameVals::items() + img);
	} else {
		return NULL;
	}
	return NULL;
}

void Tile::addDrop(item::Id item, int minAmnt, int maxAmnt) {
	if (minAmnt < 0) { minAmnt = 0; }
	drops.push_back(TileDrop());
	TileDrop& drop = drops.back();
	drop.item = item;
	drop.minAmnt = minAmnt;
	drop.maxAmnt = maxAmnt < 0 ? minAmnt : maxAmnt;
}

bool Tile::onBreak(SDL_Point loc) {
	return true;
}

bool Tile::onPlace(SDL_Point loc) {
	return true;
}

bool Tile::hit(SDL_Point loc, int power) {
	return true;
}