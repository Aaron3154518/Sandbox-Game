#include "Tiles.h"

namespace tile {
	// TODO: don't use ++ (slow)?
	int getTileOrder(Id id) {
		int i = 0;
		switch (id) {
			// case Id::ID: i++;
			// Ambient
			case Id::BOULDER3: i++;
			case Id::BOULDER2: i++;
			case Id::BOULDER1: i++;
			// Blocks
			case Id::DRAGON_EGG: i++;
			case Id::SHINY_STONE_3: i++;
			case Id::SHINY_STONE_2: i++;
			case Id::SHINY_STONE_1: i++;
			case Id::GEODE: i++;
			case Id::GLASS: i++;
			case Id::SAND: i++;
			case Id::SNOW: i++;
			case Id::STONE: i++;
			case Id::DIRT: i++;
			case Id::WOOD: i++;
			case Id::LEAVES: i++;
			// Usable Tiles
			case Id::PORTAL: i++;
			case Id::PEDESTAL: i++;
			case Id::DIMENSION_HOPPER: i++;
			case Id::WORLD_BUILDER: i++;
			case Id::CHEST: i++;
			// Crafting Tiles
			case Id::UPGRADE_STATION: i++;
			case Id::FORGE: i++;
			case Id::CRUSHER: i++;
			case Id::WORK_TABLE: i++;
			case Id::HAND_CRAFTING: i++;
				break;
			default: return INT_MAX;
		};
		return i;
	}
}