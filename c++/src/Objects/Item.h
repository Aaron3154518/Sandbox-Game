#ifndef ITEM_H
#define ITEM_H

#define ITEM_FILE "res/items.txt"

#include <iostream>
#include <utility>
#include <forward_list>
#include <map>
#include <typeindex>
#include <typeinfo>

#include <SDL.h>

#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "ObjectIds.h"
#include "ObjectParser.h"

namespace object {
	const std::map<std::string, std::type_index> ItemObjects;
	std::type_index getItemType(std::string name);
	Item* getItem(ItemId id);

	// TODO: fix
	struct ItemInfo;
	struct Animation;
	struct Recipe;

	struct ItemInfo {
		ItemId itemId = ItemId::AIR, amnt = 0;
		std::string data = "";

		bool isItem() { return amnt > 0; }
		int max_stack() { return getItem(itemId).maxStack; }

		int num_bytes() { return 4; } // TODO: data
		std::string write();

		void copy(const ItemInfo& other);
		ItemInfo copy() { return ItemInfo{ itemId, amnt, data }; }

		void print();
		void newItem();

		bool sameAs(const ItemInfo& other);
		bool operator==(const ItemInfo& other);
	};

	ItemInfo loadInfo(std::string data);

	enum ItemData {
		has_data = 0,	// Item stores data
		consumable,		// Is consumable (will decrease item amount)
		has_ui,			// Tile brings up a ui when clicked
		auto_use,		// Will automatically start using again
		is_weapon,		// Can attack an enemy
		swing,			// Should swing when used
		placer,			// Places a block on use
		breaker,		// Breaks a block on use
		left,			// Has left click function
		right,			// Has right click function

		numData
	};

	static Loader<Item, ItemId> ItemLoader(ItemId::numItems,
		LoaderInfo{ Item_FILE, "Item", ItemNames, ItemObjects });

	class Item {
		friend class ItemInfo;
	public:
		Item() : Item(ItemId::AIR) {}
		Item(ItemId _id) : id(_id) {}
		~Item() = default;

		void useAnim(double timeUsed, void* arm, bool left,
			std::pair<int, int> playerCenter, Rect &rect)

		std::string newItem();

		void onLeftClick() {}
		void onRightClick() {}

		void tick();

		std::string getDescription() { return ""; }
		std::string getFullDescription();

		SDL_Texture* drawDescription();

	private:
		// Item index
		ItemId id = ItemId::AIR;
		// Item name
		std::string name = "";
		// Image
		std::string image = "";

		// Max stack
		int maxStack = 999;

		// Use time (seconds)
		double useTime = .3;
		// Animation index (if animation)
		int animIdx = -1;

		// Magic value of item for sacrificing
		int magicVal = 0;

		// Information booleans
		bool data[ItemData::numData] = { false };

		// TODO: placeable class
		TileId blockId = TileId::AIR;

		// Attack box, if applicable
		Polygon polygon;
	};

	static Loader<Item, ItemId> itemLoader(ItemId::numItems,
		LoaderInfo{ ITEM_FILE, "Item", itemNames, itemObjects });
}
#endif