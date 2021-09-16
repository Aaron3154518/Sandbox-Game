#include "TileObjects.h"

namespace tile {
	// @param Type - the tile type to add
	// @param Id - the tile::Id variable which corresponds to Type
	// ADD_TILE(Type, Id)
	// - adds one instance of Type() to the vector of Tiles at Id
	// - assignes Id to Type::ID
	// - defines constructor (must end with ';' or '{ code }')

	// Normal blocks
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

	// Crafting Stations
	ADD_TILE(HandCrafting, HAND_CRAFTING) {
		img = "../entities/dragon/dragon_0.png";
		mapColor = { 64,64,255 };
		recipes = {
			{I::WORK_TABLE, 1, {{I::WOOD, 10}}}
		};
		for (int i = 0; i < item::Id::numItems; i++) {
			recipes.insert(Recipe{ static_cast<item::Id>(i), 1 });
		}
	}
	ADD_TILE(WorkTable, WORK_TABLE) {
		img = "work_table.png";
		mapColor = { 54,78,154 };
		addDrop(item::Id::WORK_TABLE);
		setTileData(TileData::onSurface, true);
		hardness = 1;
		recipes = {
			{I::SNOW, 1, {{I::SNOW_BALL, 4}}},
			{I::FOREST, 1, {{I::DIRT, 50}, {I::CAT, 1}}},
			{I::MOUNTAIN, 1, {{I::STONE, 10}, {I::SNOW, 15}}},
			{I::VALLEY, 1, {{I::STONE, 50}, {I::ZOMBIE, 1}}},
			{I::DESERT, 1, {{I::SAND, 50}, {I::GLASS, 15}}},
			{I::BASIC_SWORD, 1, {{I::WOOD, 10}, {I::STONE, 20}}},
			{I::CRUSHER, 1, {{I::STONE, 15}, {I::SHINY_STONE_1, 10}}},
			{I::CHEST, 1, {{I::WOOD, 15}, {I::STONE, 5}}},
			{I::WORLD_BUILDER, 1, {{I::STONE, 25}, {I::OBSIDIAN, 5}}},
			{I::SNOW_BALL, 1},
			{I::MAGIC_BALL, 1, {{I::GLASS, 10}}},
			{I::REINFORCED_MAGIC_BALL, 1, {{I::MAGIC_BALL, 2}, {I::IRON_BAR, 5}}},
			{I::SHINY_MAGIC_BALL, 1, {{I::REINFORCED_MAGIC_BALL, 2}, {I::GOLD_BAR, 10}}},
			{I::GIANT_MAGIC_BALL, 1, {{I::SHINY_MAGIC_BALL, 2}, {I::OBSIDIAN, 5}}},
			// TODO: Groups of items(e.g. any gem)
			{I::MAGIC_WAND, 1, {{I::WOOD, 5}, {I::OPAL, 2}}}
		};
	}

}