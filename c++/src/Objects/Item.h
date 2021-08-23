#ifndef ITEM_H
#define ITEM_H

#include <forward_list>
#include <iostream>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <utility>

#include <SDL.h>

// Including Tile.h is an include loop
#include "../Definitions.h"
#include "../ID/Items.h"
#include "../ID/Tiles.h"
#include "../UIs/UI.h"
#include "../Utils/AssetManager.h"
#include "../Utils/FileIO.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"

// TODO: fix
struct Animation;
struct Recipe;
struct Polygon;

class Item;
typedef std::shared_ptr<Item> ItemPtr;

class Item {
	friend class ItemInfo;
public:
	Item() = default;
	~Item() = default;

	enum ItemData {
		hasData = 0,	// Item stores data
		consumable,		// Is consumable (will decrease item amount)
		hasUi,			// Item brings up a ui when clicked
		autoUse,		// Will automatically start using again
		isWeapon,		// Can attack an enemy
		swing,			// Should swing when used
		placer,			// Places a block on use
		breaker,		// Breaks a block on use
		left,			// Has left click function
		right,			// Has right click function

		numData
	};

	virtual void useAnim(double timeUsed, void* arm, bool left,
		std::pair<int, int> playerCenter, Rect& rect);

	SharedTexture getImage() const;
	virtual SharedTexture getImage(ByteArray data) const { return getImage(); }
	virtual ByteArray newItemData() const { return ByteArray(); }

	virtual void onLeftClick() {}
	virtual void onRightClick() {}

	virtual void tick();

	virtual std::string getDescription() { return ""; }
	virtual std::string getFullDescription();

	Texture drawDescription();

	// Getters/Setters
	double getUseTime() const { return useTime; }
	typedef std::initializer_list<ItemData> DataKeys;
	bool getItemData(ItemData idx) const { return data[idx]; }
	std::map<ItemData, bool> getItemData(const DataKeys& keys) const;
	void setItemData(ItemData idx, bool val) { data[idx] = val; }
	void setItemData(const DataKeys& keys, bool val);
	tile::Id getBlockId() const { return blockId; }

	// Operator oveerloading
	bool operator[](ItemData idx) const { return data[idx]; }

	virtual item::Id id() { return Item::ID; }
	static item::Id Id() { return ID; }
	static ItemPtr getItem(item::Id id);

protected:
	static item::Id registerItem(ItemPtr t, item::Id id);

	// Item name
	std::string name = "";
	// Image
	std::string img = "";

	// Max stack
	size_t maxStack = 999;

	// Use time (seconds)
	double useTime = .3;
	// Animation index (if animation)
	int animIdx = -1;

	// Magic value of item for sacrificing
	int magicVal = 0;

	// Information booleans
	bool data[ItemData::numData] = { false };

	// TODO: placeable class
	tile::Id blockId = tile::Id::numTiles;

	// Attack box, if applicable
	//Polygon polygon;

private:
	const static item::Id ID = item::Id::numItems;
	static std::vector<ItemPtr>& getItems();
};

struct ItemInfo {
	ItemInfo() = default;
	ItemInfo(item::Id id, size_t amnt);
	~ItemInfo() = default;

	item::Id itemId = item::Id::numItems;
	size_t amnt = 0;
	ByteArray data;

	bool isItem() const { return amnt > 0 && itemId != item::Id::numItems; }
	int max_stack() const { return Item::getItem(itemId)->maxStack; }
	SharedTexture getImage() const { return Item::getItem(itemId)->getImage(data); }

	void read(IO& io);
	void write(IO& io) const;

	void print();

	bool sameAs(const ItemInfo& other) const;
};

#define NEW_ITEM(TYPE) \
private: \
friend class Item; \
TYPE(); \
const static item::Id ID; \
public: \
item::Id id() { return TYPE::ID; } \
static item::Id Id() { return ID; }

#define ADD_ITEM(TYPE, ITEM_ID) \
const item::Id TYPE::ID = \
	Item::registerItem(std::shared_ptr<TYPE>(new TYPE()), item::Id::ITEM_ID); \
TYPE::TYPE()

#endif