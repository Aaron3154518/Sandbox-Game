#include "Items.h"

namespace item {
	int getItemOrder(Id id) {
		int i = 0;
		switch (id) {
			// case Id::ID: i++;
			// Materials
			case Id::DRAGON_CLAW: i++;
				// Bars
			case Id::GOLD_BAR: i++;
			case Id::IRON_BAR: i++;
			// Blocks
				// Special Blocks
			case Id::DRAGON_EGG: i++;
				// Gems
			case Id::OPAL: i++;
			case Id::PEARL: i++;
			case Id::JADE: i++;
			case Id::SAPPHIRE: i++;
			case Id::AMETHYST: i++;
			case Id::GEODE: i++;
				// Ores
			case Id::OBSIDIAN: i++;
			case Id::SPHALERITE: i++;
			case Id::PYRITE: i++;
			case Id::GOLD_ORE: i++;
			case Id::IRON_ORE: i++;
			case Id::SHINY_STONE_3: i++;
			case Id::SHINY_STONE_2: i++;
			case Id::SHINY_STONE_1: i++;
				// Common Blocks
			case Id::GLASS: i++;
			case Id::SAND: i++;
			case Id::SNOW: i++;
			case Id::LEAVES: i++;
			case Id::WOOD: i++;
			case Id::STONE: i++;
			case Id::DIRT: i++;
			// Magic
			case Id::GIANT_MAGIC_BALL: i++;
			case Id::SHINY_MAGIC_BALL: i++;
			case Id::REINFORCED_MAGIC_BALL: i++;
			case Id::MAGIC_BALL: i++;
			case Id::PEDESTAL: i++;
			// Worlds Stuff
			case Id::BONUS_STRUCTURE: i++;
				// World Size
			case Id::LARGE_WORLD: i++;
			case Id::MED_WORLD: i++;
			case Id::SMALL_WORLD: i++;
				// Biomes
			case Id::DESERT: i++;
			case Id::VALLEY: i++;
			case Id::MOUNTAIN: i++;
			case Id::FOREST: i++;
			case Id::WORLD_BUILDER: i++;
			case Id::DIMENSION_HOPPER: i++;
			// Chest
			case Id::CHEST: i++;
			// Crafting Stations
			case Id::UPGRADE_STATION: i++;
			case Id::CRUSHER: i++;
			case Id::FORGE: i++;
			case Id::WORK_TABLE: i++;
			// Useables
			case Id::MAGIC_WAND: i++;
			case Id::WAYPOINT: i++;
			case Id::DEMATERIALIZER: i++;
			case Id::TIME_WARP: i++;
			// Armor
			case Id::BOOTS: i++;
			case Id::LEGGINGS: i++;
			case Id::CHESTPLATE: i++;
			case Id::HELMET: i++;
			// Tools
			case Id::SNOW_BALL: i++;
			case Id::BASIC_PICKAXE: i++;
			case Id::BASIC_SWORD: i++;
				break;
			default: return INT_MAX;
		};
		return i;
	}
}