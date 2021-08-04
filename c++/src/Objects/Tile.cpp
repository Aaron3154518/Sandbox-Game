#include "Tile.h"
namespace object {
	const std::map<std::string, std::type_index> tileObjects = {
		"Tile" : std::type_index(Tile)
	};

	std::type_index getTileType(std::string name) {
		auto it = tileObjects.find(name);
		return it != tileObjects.end() ? it->second : std::type_index(Tile);
	}

	Tile* getTile(TileId id) {
		return tileLoader.getObject(id);
	}
}