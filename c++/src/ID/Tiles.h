#ifndef ID_TILES_H
#define ID_TILES_H

namespace tile {
	// Order never changes!
	enum Id : uint16_t {
		AIR = 0,
		DIRT,
		STONE,
		CAT,
		ZOMBIE,
		DIMENSION_HOPPER,
		CHEST,
		SNOW,
		WORLD_BUILDER,
		WORK_TABLE,
		DOOM_BUNNY,
		WOOD,
		LEAVES,
		HELICOPTER,
		BIRDIE,
		BOULDER1,
		BOULDER2,
		BOULDER3,
		CRUSHER,
		SHINY_STONE_1,
		SHINY_STONE_2,
		SHINY_STONE_3,
		DRAGON_EGG,
		FORGE,
		UPGRADE_STATION,
		SAND,
		GLASS,
		GEODE,
		PORTAL,
		PEDESTAL,


		numTiles
	};

	// TODO: operator bool(const Id& id) { return id != Id::numTiles; }
	//bool toBool(const Id& id) { return id != Id::numTiles; }
}

#endif