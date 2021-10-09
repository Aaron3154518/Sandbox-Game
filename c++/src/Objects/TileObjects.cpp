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
		addRecipe(Recipe(I::WORK_TABLE, 1, { ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10),ItemInfo(I::WOOD, 10) }));
		for (int i = 0; i < item::Id::numItems; i++) {
			addRecipe(Recipe(static_cast<I>(i), 1));
		}
	}
	ADD_TILE(WorkTable, WORK_TABLE) {
		img = "work_table.png";
		mapColor = { 54,78,154 };
		addDrop(item::Id::WORK_TABLE);
		setTileData(TileData::onSurface, true);
		hardness = 1;
		addRecipes({
			Recipe(I::SNOW, 1, {ItemInfo(I::SNOWBALL, 4)}),
			Recipe(I::FOREST, 1, {ItemInfo(I::DIRT, 50), ItemInfo(I::CAT, 1)}),
			Recipe(I::MOUNTAIN, 1, {ItemInfo(I::STONE, 10), ItemInfo(I::SNOW, 15)}),
			Recipe(I::VALLEY, 1, {ItemInfo(I::STONE, 50), ItemInfo(I::ZOMBIE, 1)}),
			Recipe(I::DESERT, 1, {ItemInfo(I::SAND, 50), ItemInfo(I::GLASS, 15)}),
			Recipe(I::BASIC_SWORD, 1, {ItemInfo(I::WOOD, 10), ItemInfo(I::STONE, 20)}),
			Recipe(I::CRUSHER, 1, {ItemInfo(I::STONE, 15), ItemInfo(I::SHINY_STONE_1, 10)}),
			Recipe(I::CHEST, 1, {ItemInfo(I::WOOD, 15), ItemInfo(I::STONE, 5)}),
			Recipe(I::WORLD_BUILDER, 1, {ItemInfo(I::STONE, 25), ItemInfo(I::OBSIDIAN, 5)}),
			Recipe(I::SNOWBALL, 1),
			Recipe(I::MAGIC_BALL, 1, {ItemInfo(I::GLASS, 10)}),
			Recipe(I::REINFORCED_MAGIC_BALL, 1, {ItemInfo(I::MAGIC_BALL, 2), ItemInfo(I::IRON_BAR, 5)}),
			Recipe(I::SHINY_MAGIC_BALL, 1, {ItemInfo(I::REINFORCED_MAGIC_BALL, 2), ItemInfo(I::GOLD_BAR, 10)}),
			Recipe(I::GIANT_MAGIC_BALL, 1, {ItemInfo(I::SHINY_MAGIC_BALL, 2), ItemInfo(I::OBSIDIAN, 5)}),
			// TODO: Groups of items(e.g. any gem)
			Recipe(I::MAGIC_WAND, 1, {ItemInfo(I::WOOD, 5), ItemInfo(I::OPAL, 2)})
			});
	}
}