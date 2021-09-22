#ifndef ITEM_OBJECTS_H
#define ITEM_OBJECTS_H

#include "Item.h"
#include "../ID/Items.h"
#include "../ID/Tiles.h"

namespace item {
	// @param Type - the type whose class is being declared
	// NEW_ITEM(Type)
	// - makes Item a friend of Type
	// - declares a constructor Type()
	// - implements a special id for the type

	// Alphabetical Order
	// !A
	class Amethyst : public Item { NEW_ITEM(Amethyst) };
	// !B
	class BasicPickaxe : public Item { NEW_ITEM(BasicPickaxe) };
	class BasicSword : public Item { NEW_ITEM(BasicSword) };
	class BonusStructure : public Item { NEW_ITEM(BonusStructure) };
	class Boots : public Item { NEW_ITEM(Boots) };
	// !C
	class Chest : public Item { NEW_ITEM(Chest) };
	class Chestplate : public Item { NEW_ITEM(Chestplate) };
	class Crusher : public Item { NEW_ITEM(Crusher) };
	// !D
	class Dematerializer : public Item { NEW_ITEM(Dematerializer) };
	class Desert : public Item { NEW_ITEM(Desert) };
	class DimensionHopper : public Item { NEW_ITEM(DimensionHopper) };
	class Dirt : public Item { NEW_ITEM(Dirt) };
	class DragonClaw : public Item { NEW_ITEM(DragonClaw) };
	class DragonEgg : public Item { NEW_ITEM(DragonEgg) };
	// !E
	// !F
	class Forest : public Item { NEW_ITEM(Forest) };
	class Forge : public Item { NEW_ITEM(Forge) };
	// !G
	class Geode : public Item { NEW_ITEM(Geode) };
	class GiantMagicBall : public Item { NEW_ITEM(GiantMagicBall) };
	class Glass : public Item { NEW_ITEM(Glass) };
	class GoldBar : public Item { NEW_ITEM(GoldBar) };
	class GoldOre : public Item { NEW_ITEM(GoldOre) };
	// !H
	class Helmet : public Item { NEW_ITEM(Helmet) };
	// !I
	class IronBar : public Item { NEW_ITEM(IronBar) };
	class IronOre : public Item { NEW_ITEM(IronOre) };
	// !J
	class Jade : public Item { NEW_ITEM(Jade) };
	// !K
	// !L
	class LargeWorld : public Item { NEW_ITEM(LargeWorld) };
	class Leaves : public Item { NEW_ITEM(Leaves) };
	class Leggings : public Item { NEW_ITEM(Leggings) };
	// !M
	class MagicBall : public Item { NEW_ITEM(MagicBall) };
	class MagicWand : public Item { NEW_ITEM(MagicWand) };
	class MedWorld : public Item { NEW_ITEM(MedWorld) };
	class Mountain : public Item { NEW_ITEM(Mountain) };
	// !N
	// !O
	class Obsidian : public Item { NEW_ITEM(Obsidian) };
	class Opal : public Item { NEW_ITEM(Opal) };
	// !P
	class Pearl : public Item { NEW_ITEM(Pearl) };
	class Pedestal : public Item { NEW_ITEM(Pedestal) };
	class Pyrite : public Item { NEW_ITEM(Pyrite) };
	// !Q
	// !R
	class ReinforcedMagicBall : public Item { NEW_ITEM(ReinforcedMagicBall) };
	// !S
	class Sand : public Item { NEW_ITEM(Sand) };
	class Sapphire : public Item { NEW_ITEM(Sapphire) };
	class ShinyMagicBall : public Item { NEW_ITEM(ShinyMagicBall) };
	class ShinyStone1 : public Item { NEW_ITEM(ShinyStone1) };
	class ShinyStone2 : public Item { NEW_ITEM(ShinyStone2) };
	class ShinyStone3 : public Item { NEW_ITEM(ShinyStone3) };
	class SmallWorld : public Item { NEW_ITEM(SmallWorld) };
	class Snow : public Item { NEW_ITEM(Snow) };
	class Snowball : public Item { NEW_ITEM(Snowball) };
	class Sphalerite : public Item { NEW_ITEM(Sphalerite) };
	class Stone : public Item { NEW_ITEM(Stone) };
	// !T
	class TimeWarp : public Item { NEW_ITEM(TimeWarp) };
	// !U
	class UpgradeStation : public Item { NEW_ITEM(UpgradeStation) };
	// !V
	class Valley : public Item { NEW_ITEM(Valley) };
	// !W
	class Waypoint : public Item { NEW_ITEM(Waypoint) };
	class Wood : public Item { NEW_ITEM(Wood) };
	class WorkTable : public Item { NEW_ITEM(WorkTable) };
	class WorldBuilder: public Item { NEW_ITEM(WorldBuilder) };
	// !X
	// !Y
	// !Z

}

#endif