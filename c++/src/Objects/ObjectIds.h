#ifndef OBJECT_IDS_H
#define OBJECT_IDS_H

namespace object {
	namespace Tile {
		enum TileId : uint16_t {
			// Normal Tiles
			AIR = 0, DIRT, STONE, SNOW, WOOD, LEAVES, SAND, GLASS,
			BOULDER1, BOULDER2, BOULDER3, SHINY_STONE_1, SHINY_STONE_2, SHINY_STONE_3,
			GEODE,
			DRAGON_EGG,

			// Usables
			DIMENSION_HOPPER, CHEST, WORLD_BUILDER, CRUSHER,
			UPGRADE_STATION, PORTAL, PEDESTAL,

			// Crafters
			WORK_TABLE, FORGE,

			// Spawners
			CAT, ZOMBIE, DOOM_BUNNY, HELICOPTER, BIRDIE,

			numTiles
		};
	}

	const static std::string tileNames[Tile::TileId::numTiles] = {
		// Normal Tiles
		"Air", "Dirt", "Stone", "Snow", "Wood", "Leaves", "Sand", "Glass",
		"Boulder1", "Boulder2", "Boulder3", "ShinyStone1", "ShinyStone2", "ShinyStone3",
		"Geoade",
		"DragonEgg",

		// Usables
		"DimensionHopper", "Chest", "WorldBuilder", "Crusher",
		"UpgradeStation", "Portal", "Pedestal",

		// Crafters
		"WorkTable", "Forge"

		// Spawners
		"Cat", "Zombie", "DoomBunny", "Helicopter", "Birdie",

		""
	};

	namespace Item {
		enum ItemId : uint16_t {
			// Normal Items
			AIR = 0, DIRT, STONE, SNOW, WOOD, LEAVES, SAND, GLASS,
			BOULDER1, BOULDER2, BOULDER3, SHINY_STONE_1, SHINY_STONE_2, SHINY_STONE_3,
			GEODE,
			DRAGON_EGG,

			// Usables
			DIMENSION_HOPPER, CHEST, WORLD_BUILDER, CRUSHER,
			UPGRADE_STATION, PORTAL, PEDESTAL,

			// Crafters
			WORK_TABLE, FORGE,

			// Spawners
			CAT, ZOMBIE, DOOM_BUNNY, HELICOPTER, BIRDIE,

			numItems
		};
	}

	const static std::string ItemNames[Item::ItemId::numItems] = {
		// Normal Items
		"Air", "Dirt", "Stone", "Snow", "Wood", "Leaves", "Sand", "Glass",
		"Boulder1", "Boulder2", "Boulder3", "ShinyStone1", "ShinyStone2", "ShinyStone3",
		"Geoade",
		"DragonEgg",

		// Usables
		"DimensionHopper", "Chest", "WorldBuilder", "Crusher",
		"UpgradeStation", "Portal", "Pedestal",

		// Crafters
		"WorkTable", "Forge"

		// Spawners
		"Cat", "Zombie", "DoomBunny", "Helicopter", "Birdie",

		""
	};
}

#endif