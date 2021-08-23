#ifndef INVENTORY_H
#define INVENTORY_H

#include <cmath>
#include <set>

#include <SDL.h>

#include "../Definitions.h"
#include "../ID/Items.h"
#include "../Objects/Item.h"
#include "../UIs/UI.h"
#include "../Utils/AssetManager.h"
#include "../Utils/FileIO.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"

class Inventory {
public:
	Inventory() : Inventory(SDL_Point{ 0,0 }) {}
	Inventory(SDL_Point _dim);
	~Inventory() = default;

	virtual void draw(SDL_Point parentPos);
	virtual void drawInventory();

	virtual bool leftClick(SDL_Point mouse) { return false; }
	virtual bool rightClick(SDL_Point mouse) { return true; }
	virtual bool pickUpItem(const ItemInfo& item) { return false; }

	// Getters/Setters
	void setPos(int x, int y) { setPos(SDL_Point{ x,y }); }
	void setPos(SDL_Point pos);
	Rect getRect() const { return mRect; }
	void setMaxStack(size_t val) { maxStack = val; }
	size_t getMaxStack() const { return maxStack; }
	void setItemList(std::set<item::Id>& list, bool isWhiteList);
	// Get/set item
	const ItemInfo& getItem(int x, int y) const;
	const ItemInfo& getItem(SDL_Point loc) const;
	void setItem(int x, int y, const ItemInfo& item);
	void setItem(SDL_Point loc, const ItemInfo& item);

	bool empty() const;
	bool itemAllowed(item::Id id) const;
	bool validPos(SDL_Point loc) const;

	void read(IO& io);
	void write(IO& io) const;

	static SDL_Point toInvPos(SDL_Point pos);
	static SDL_Point toPxPos(SDL_Point pos);

protected:
	void updateItem(SDL_Point loc);
	// pos is relative to the parent of Inventory
	// (i.e. pos - mRect.topleft() -> pos relative to topleft of mRect)
	void drawHoverItem(SDL_Point pos);

	size_t waitTime() const { fmaxf(750000 / (holdingR + 1), 10); }

	SDL_Point dim;
	// Personal maximum stack limit
	size_t maxStack = 999;

	// Items
	std::vector<std::vector<ItemInfo>> items;

	// List of items to accept/ignore
	std::set<item::Id> itemList;
	// True for whitelist, false for blacklist
	bool whiteList = true;

	Rect mRect;
	TextureData mTex;
	TextData td;
	// How long we've been holding right click (ms)
	size_t holdingR = 0;

	const static SDL_Color BKGRND;
	const static ItemInfo NO_ITEM;

private:
};

#endif