#include "ItemObjects.h"

namespace item {
	// @param Type - the item type to add
	// @param Id - the item::Id variable which corresponds to Type
	// ADD_TILE(Type, Id)
	// - adds one instance of Type() to the vector of Items at Id
	// - assignes Id to Type::ID
	// - defines constructor (must end with ';' or '{ code }')

#define PLACEABLE setItemData({ItemData::consumable, ItemData::autoUse, ItemData::swing, ItemData::placer}, true)

	ADD_ITEM(Dirt, DIRT) {
		name = "Dirt";
		img = "dirt.png";
		blockId = tile::Id::DIRT;
		PLACEABLE;
	}
	ADD_ITEM(Stone, STONE) {
		name = "Stone";
		img = "stone.png";
		blockId = tile::Id::STONE;
		PLACEABLE;
	}
}