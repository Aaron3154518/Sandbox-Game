#ifndef PLAYER_INVENTORY_H
#define PLAYER_INVENTORY_H

#include <cmath>
#include <iostream>
#include <map>

#include <SDL.h>

#include "Inventory.h"
#include "../Definitions.h"
#include "../Objects/DroppedItem.h"
#include "../Objects/Item.h"
#include "../UIElements/Button.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"

class ArmorInventory : public Inventory {
	friend class PlayerInventory;
public:
	ArmorInventory();
	~ArmorInventory() = default;

private:
	// Stats

	static const SDL_Point DIM;
};

struct UseItem {
	friend class PlayerInventory;
public:
	ItemInfo item() const { return info; }
	bool isItem() const { return info.isItem(); }
	bool inUse() const { return useTime > 0; }
	bool left() const { return usedLeft; }

private:
	void tick(int ms);
	bool leftClick(ItemInfo& item);
	bool rightClick(ItemInfo& item);
	void setUseTime(int ms) { useTime = ms; if (!inUse()) { info.itemId = item::Id::numItems; } }
	void decUseTime(int ms) { setUseTime(useTime - ms); }

	ItemInfo info;
	int useTime = 0;
	bool usedLeft, firstSwing = false;
};

class PlayerInventory : public Inventory {
public:
	PlayerInventory();
	~PlayerInventory() = default;

	void draw(); //Player
	void drawInventory(); //Player

	void handleEvents(Event& e);
	bool leftClickPos(SDL_Point pos);
	bool rightClickPos(SDL_Point pos);

	ItemInfo leftClickItem(ItemInfo item, int itemMaxStack);
	ItemInfo rightClickItem(ItemInfo item, int itemMaxStack);
	void autoMoveItem(SDL_Point loc);
	void onChangeHeld(); //Private

	void selectHotbar(int pos); //Private

	void useItem(); //Player
	void dropItem(); //Player

	bool isOpen() const { return open; }
	void toggleOpen(); //Player

	// Current held item (excluding hotbar)
	const ItemInfo& getHeldItem() const { return heldItem; }
	// Current selected item (held or in hotbar)
	const ItemInfo& getCurrentItem() const;

	void read(IO& io);
	void write(IO& io);

private:
	// Current selected item (held or in hotbar)
	ItemInfo& getCurrentItemRef();

	ArmorInventory armorInv;

	//Rect craftR;
	//TextureData craftToggle;
	Button craftToggle;

	// Currently held item
	ItemInfo heldItem;
	int hotbarItem = 0;
	// Is the inventory open
	bool open;
	// Item transfer for right click
	double amntTransferred = 0.;

	// Item begin used
	UseItem itemUsed;

	static const SDL_Point DIM;
	static const int BUTTON_W;
	static const std::string CRAFT_TOGGLE;
};

#endif