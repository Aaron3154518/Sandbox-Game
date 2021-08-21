#include "TileObjects.h"

namespace tile {
	// @param Type - the tile type to add
	// @param Id - the tile::Id variable which corresponds to Type
	// ADD_TILE(Type, Id)
	// - adds one instance of Type() to the vector of Tiles at Id
	// - assignes Id to Type::ID
	// - defines constructor (must end with ';' or '{ code }')

	ADD_TILE(Air, AIR) {
		mapColor = { 64,64,255 };
	}
	ADD_TILE(Dirt, DIRT) {
		img = "dirt.png";
		mapColor = { 64,64,255 };
		addDrop(item::Id::DIRT);
		setTileData(TileData::barrier, true);
	}
	ADD_TILE(Stone, STONE) {
		img = "stone.png";
		mapColor = { 64,64,255 };
		addDrop(item::Id::STONE);
		setTileData(TileData::barrier, true);
		hardness = 1;
	}
}