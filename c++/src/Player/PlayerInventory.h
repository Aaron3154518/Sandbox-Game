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
#include "../Utils/Rect.h"

// Forward declaration
class PlayerInventory;

class ArmorInventory : public Inventory {
public:
	ArmorInventory();
	~ArmorInventory() = default;

private:
	// Stats

	static const SDL_Point DIM;
};

class PlayerInventory : public Inventory {
public:
	PlayerInventory();
	~PlayerInventory() = default;

	// parentPos is assumed to be based on (0,0) as the topleft of the screen
	void draw(SDL_Point parentPos = SDL_Point{ 0,0 });
	void drawInventory();

	// Mouse is assumed to use the same topleft as mRect
	bool leftClick(SDL_Point mouse);
	bool rightClick(SDL_Point mouse);

	void autoMoveItem(SDL_Point loc);
	void onChangeHeld();

	void selectHotbar(unsigned int pos);
	void scrollHotbar(bool up);

	void useItem();
	void dropItem();

	void keyPressed(SDL_KeyCode key);

	bool isOpen() const { return open; }
	void toggleOpen();

	// Current held item (excluding hotbar)
	const ItemInfo& getHeldItem() const { return heldItem; }
	// Current selected item (held or in hotbar)
	const ItemInfo& getCurrentItem() const;

private:
	// Current selected item (held or in hotbar)
	ItemInfo& getCurrentItemRef();

	ArmorInventory armorInv;

	//Rect craftR;
	//TextureData craftToggle;
	Button craftToggle;

	// Currently held item
	ItemInfo heldItem;
	// Hot bar index
	unsigned int hotbarItem = 0;
	// Is the inventory open
	bool open;

	static const SDL_Point DIM;
	static const SDL_Color SELECT_COLOR, HOVER_COLOR;
	static const int BUTTON_W;
	static const std::string CRAFT_TOGGLE;
};

#endif