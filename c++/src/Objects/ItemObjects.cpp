#include "ItemObjects.h"

namespace item {
	// @param Type - the item type to add
	// @param Id - the item::Id variable which corresponds to Type
	// ADD_TILE(Type, Id)
	// - adds one instance of Type() to the vector of Items at Id
	// - assignes Id to Type::ID
	// - defines constructor (must end with ';' or '{ code }')

#define PLACEABLE setItemData(ItemData::consumable | ItemData::autoUse \
	| ItemData::swing | ItemData::placer | ItemData::leftClick, true)

	// Alphabetical Order
	// !A
	ADD_ITEM(Amethyst, AMETHYST) {
		name = "Amethyst";
		img = "amethyst.png";
	}
	// !B
	ADD_ITEM(BasicPickaxe, BASIC_PICKAXE) {
		name = "Basic Pickaxe";
		img = "basic_pickaxe.png";
	}
	ADD_ITEM(BasicSword, BASIC_SWORD) {
		name = "Basic Sword";
		img = "basic_sword.png";
	}
	ADD_ITEM(BonusStructure, BONUS_STRUCTURE) {
		name = "Bonus Structure";
		img = "bonus_structure.png";
	}
	ADD_ITEM(Boots, BOOTS) {
		name = "Boots";
		img = "boots.png";
	}
	// !C
	ADD_ITEM(Chest, CHEST) {
		name = "Chest";
		img = "chest.png";
	}
	ADD_ITEM(Chestplate, CHESTPLATE) {
		name = "Chestplate";
		img = "chestplate.png";
	}
	ADD_ITEM(Crusher, CRUSHER) {
		name = "Crusher";
		img = "crusher/crusher_0.png";
	}
	// !D
	ADD_ITEM(Dematerializer, DEMATERIALIZER) {
		name = "Dematerializer";
		img = "dematerializer.png";
	}
	ADD_ITEM(Desert, DESERT) {
		name = "Desert";
		img = "desert.png";
	}
	ADD_ITEM(DimensionHopper, DIMENSION_HOPPER) {
		name = "Dimension Hopper";
		img = "dimension_hopper.png";
	}
	ADD_ITEM(Dirt, DIRT) {
		name = "Dirt";
		img = "dirt.png";
		blockId = tile::Id::DIRT;
		PLACEABLE;
	}
	ADD_ITEM(DragonClaw, DRAGON_CLAW) {
		name = "Dragon Claw";
		img = "dragon_claw.png";
	}
	ADD_ITEM(DragonEgg, DRAGON_EGG) {
		name = "Dragon Egg";
		img = "dragon_egg.png";
	}
	// !E
	// !F
	ADD_ITEM(Forest, FOREST) {
		name = "Forest";
		img = "forest.png";
	}
	ADD_ITEM(Forge, FORGE) {
		name = "Forge";
		img = "forge/forge_0.png";
	}
	// !G
	ADD_ITEM(Geode, GEODE) {
		name = "Geode";
		img = "geode.png";
	}
	ADD_ITEM(GiantMagicBall, GIANT_MAGIC_BALL) {
		name = "Giant Magic Ball";
		img = "giant_magic_ball.png";
	}
	ADD_ITEM(Glass, GLASS) {
		name = "Glass";
		img = "glass.png";
	}
	ADD_ITEM(GoldBar, GOLD_BAR) {
		name = "Gold Bar";
		img = "gold_bar.png";
	}
	ADD_ITEM(GoldOre, GOLD_ORE) {
		name = "Gold Ore";
		img = "gold_ore.png";
	}
	// !H
	ADD_ITEM(Helmet, HELMET) {
		name = "Helmet";
		img = "helmet.png";
	}
	// !I
	ADD_ITEM(IronBar, IRON_BAR) {
		name = "Iron Bar";
		img = "iron_bar.png";
	}
	ADD_ITEM(IronOre, IRON_ORE) {
		name = "Iron Ore";
		img = "iron_ore.png";
	}
	// !J
	ADD_ITEM(Jade, JADE) {
		name = "Jade";
		img = "jade.png";
	}
	// !K
	// !L
	ADD_ITEM(LargeWorld, LARGE_WORLD) {
		name = "Large World";
		img = "large_world.png";
	}
	ADD_ITEM(Leaves, LEAVES) {
		name = "Leaves";
		img = "leaves.png";
	}
	ADD_ITEM(Leggings, LEGGINGS) {
		name = "Leggings";
		img = "leggings.png";
	}
	// !M
	ADD_ITEM(MagicBall, MAGIC_BALL) {
		name = "Magic Ball";
		img = "magic_ball.png";
	}
	ADD_ITEM(MagicWand, MAGIC_WAND) {
		name = "Magic Wand";
		img = "magic_wand.png";
	}
	ADD_ITEM(MedWorld, MED_WORLD) {
		name = "Medium World";
		img = "med_world.png";
	}
	ADD_ITEM(Mountain, MOUNTAIN) {
		name = "Mountain";
		img = "mountain.png";
	}
	// !N
	// !O
	ADD_ITEM(Obsidian, OBSIDIAN) {
		name = "Obsidian";
		img = "obsidian.png";
	}
	ADD_ITEM(Opal, OPAL) {
		name = "Opal";
		img = "opal.png";
	}
	// !P
	ADD_ITEM(Pearl, PEARL) {
		name = "Pearl";
		img = "pearl.png";
	}
	ADD_ITEM(Pedestal, PEDESTAL) {
		name = "Pedestal";
		img = "pedestal.png";
	}
	ADD_ITEM(Pyrite, PYRITE) {
		name = "Pyrite";
		img = "pyrite.png";
	}
	// !Q
	// !R
	ADD_ITEM(ReinforcedMagicBall, REINFORCED_MAGIC_BALL) {
		name = "Reinforced Magic Ball";
		img = "reinforced_magic_ball.png";
	}
	// !S
	ADD_ITEM(Sand, SAND) {
		name = "Sand";
		img = "sand.png";
	}
	ADD_ITEM(Sapphire, SAPPHIRE) {
		name = "Sapphire";
		img = "sapphire.png";
	}
	ADD_ITEM(ShinyMagicBall, SHINY_MAGIC_BALL) {
		name = "Shiny Magic Ball";
		img = "shiny_magic_ball.png";
	}
	ADD_ITEM(ShinyStone1, SHINY_STONE_1) {
		name = "Shiny Stone";
		img = "shiny_stone_1.png";
	}
	ADD_ITEM(ShinyStone2, SHINY_STONE_2) {
		name = "Shiny Stone";
		img = "shiny_stone_2.png";
	}
	ADD_ITEM(ShinyStone3, SHINY_STONE_3) {
		name = "Shiny Stone";
		img = "shiny_stone_3.png";
	}
	ADD_ITEM(SmallWorld, SMALL_WORLD) {
		name = "Small World";
		img = "small_world.png";
	}
	ADD_ITEM(Snow, SNOW) {
		name = "Snow";
		img = "snow.png";
	}
	ADD_ITEM(Snowball, SNOWBALL) {
		name = "Snowball";
		img = "snowball.png";
	}
	ADD_ITEM(Sphalerite, SPHALERITE) {
		name = "Sphalerite";
		img = "sphalerite.png";
	}
	ADD_ITEM(Stone, STONE) {
		name = "Stone";
		img = "stone.png";
		blockId = tile::Id::STONE;
		PLACEABLE;
	}
	// !T
	ADD_ITEM(TimeWarp, TIME_WARP) {
		name = "Time Warp";
		img = "time_warp.png";
	}
	// !U
	ADD_ITEM(UpgradeStation, UPGRADE_STATION) {
		name = "Upgrade Station";
		img = "upgrade_station.png";
	}
	// !V
	ADD_ITEM(Valley, VALLEY) {
		name = "Valley";
		img = "valley.png";
	}
	// !W
	ADD_ITEM(Waypoint, WAYPOINT) {
		name = "Waypoint";
		img = "waypoint.png";
	}
	ADD_ITEM(Wood, WOOD) {
		name = "Wood";
		img = "wood_item.png";
	}
	ADD_ITEM(WorkTable, WORK_TABLE) {
		name = "Work Table";
		img = "work_table.png";
		blockId = tile::Id::WORK_TABLE;
		PLACEABLE;
	}
	ADD_ITEM(WorldBuilder, WORLD_BUILDER) {
		name = "World Builder";
		img = "world_builder/world_builder_0.png";
	}
	// !X
	// !Y
	// !Z
}