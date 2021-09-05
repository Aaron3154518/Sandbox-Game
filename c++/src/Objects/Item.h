#ifndef ITEM_H
#define ITEM_H

#include <cmath>
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
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/FileIO.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"
#include "../Window.h"

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

	enum ItemData : uint32_t {
		hasData = 0x1,		// Item stores data
		consumable = 0x2,	// Is consumable (will decrease item amount)
		hasUi = 0x4,		// Item brings up a ui when clicked
		autoUse = 0x8,		// Will automatically start using again
		isWeapon = 0x10,	// Can attack an enemy
		swing = 0x20,		// Should swing when used
		placer = 0x40,		// Places a block on use
		breaker = 0x80,		// Breaks a block on use
		leftClick = 0x100,	// Has left click function
		rightClick = 0x200,	// Has right click function
	};

	virtual void useAnim(double timeUsed, void* arm, bool left,
		std::pair<int, int> playerCenter, Rect& rect);

	SharedTexture getImage() const;
	virtual SharedTexture getImage(ByteArray data) const { return getImage(); }
	virtual ByteArray newItemData() const { return ByteArray(); }

	virtual void onLeftClick() {}
	virtual void onRightClick() {}

	virtual int tick(int useTime, int dt);

	virtual std::string getDescription() { return ""; }
	virtual std::string getFullDescription();

	Texture drawDescription();

	// Getters/Setters
	double getUseTime() const { return useTime; }
	bool getItemData(uint32_t bits) const;
	void setItemData(uint32_t bits, bool val);
	tile::Id getBlockId() const { return blockId; }

	// Operator oveerloading
	bool operator[](uint32_t bits) const { return getItemData(bits); }

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
	int maxStack = 999;

	// Use time (seconds)
	double useTime = .3;
	// Animation index (if animation)
	int animIdx = -1;

	// Magic value of item for sacrificing
	int magicVal = 0;

	// Information bits
	uint32_t data = 0;

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
	int amnt = 0;
	ByteArray data;

	bool isItem() const;
	ItemPtr getItem() const;
	int maxStack() const;
	int maxStack(int mMaxStack) const;
	int useTime() const;
	SharedTexture getImage() const;
	bool operator[](Item::ItemData idx) const;

	void read(IO& io);
	void write(IO& io);

	void print();

	bool sameAs(const ItemInfo& other) const;
	bool equals(const ItemInfo& other) const;
	bool operator==(const ItemInfo& other) const;

	static const ItemInfo& NO_ITEM();
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