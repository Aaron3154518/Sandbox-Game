#include "Tile.h"

std::vector<TilePtr>& Tile::getTiles() {
	static std::vector<TilePtr> tiles;
	return tiles;
}

TilePtr Tile::getTile(TileId id) {
	return getTiles()[id];
}

int Tile::registerTile(TilePtr t) {
	auto& tiles = getTiles();
	tiles.push_back(t);
	return tiles.size() - 1;
}