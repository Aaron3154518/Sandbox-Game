#ifndef INVENTORY_H
#define INVENTORY_H

#include <cmath>
#include <list>
#include <set>

#include <SDL.h>

#include "../Definitions.h"
#include "../ID/Items.h"
#include "../Objects/Item.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/FileIO.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"
#include "../Window.h"

class Inventory {
public:
	Inventory() : Inventory(SDL_Point{ 0,0 }) {}
	Inventory(SDL_Point _dim);
	~Inventory() = default;

	virtual void draw(SDL_Point topleft);
	virtual void drawInventory();

	virtual void handleEvents(Event& e, SDL_Point topLeft);
	virtual bool leftClickPos(SDL_Point pos);
	virtual bool rightClickPos(SDL_Point pos);
	bool isSpaceForItem(const ItemInfo& item) const;
	// Returns true if the entire item was picked up
	bool pickUpItem(ItemInfo& item);
	virtual void autoMoveItem(SDL_Point loc);

	void selectPos(SDL_Point pos);
	void unselectPos(SDL_Point pos);

	// Getters/Setters
	void setPos(int x, int y) { setPos(SDL_Point{ x,y }); }
	void setPos(SDL_Point pos);
	Rect getRect() const { return mRect; }
	void setMaxStack(int val) { maxStack = val; }
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

	virtual void clear();

	virtual void read(IO& io);
	virtual void write(IO& io);

	static SDL_Point toInvPos(SDL_Point pos);
	static SDL_Point toPxPos(SDL_Point pos);
	static Rect getInvRect(SDL_Point pos);

protected:
	std::list<SDL_Point> getSpaceForItem(const ItemInfo& item) const;

	void updatePos(SDL_Point loc);
	// pos is relative to the parent of Inventory
	// (i.e. pos - mRect.topleft() -> pos relative to topleft of mRect)
	void drawHoverItem(SDL_Point pos);

	// int holdingR = 0;
	//size_t waitTime() const { fmaxf(750000 / (holdingR + 1), 10); }

	SDL_Point dim;
	// Personal maximum stack limit
	int maxStack = 999;

	// Items
	std::vector<std::vector<ItemInfo>> items;

	// List of items to accept/ignore
	std::set<item::Id> itemList;
	// True for whitelist, false for blacklist
	bool whiteList = true;

	Rect mRect;
	RenderData rData;
	TextData textData;
	// Should we draw a description
	bool drawDescription = false;

	static const SDL_Color BKGRND, SELECT_COLOR;
	static const std::string FONT_ID;
};

#endif