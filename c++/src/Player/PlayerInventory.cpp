#include "PlayerInventory.h"
#include "../GameObjects.h"
#include "Player.h"

const SDL_Point ArmorInventory::DIM{ 1,4 };
const SDL_Point PlayerInventory::DIM{ 10,5 };

const int PlayerInventory::BUTTON_W = gameVals::INV_W() / 2;
const std::string PlayerInventory::CRAFT_TOGGLE =
gameVals::images() + "crafting_toggle.png";

// TODO: pass reference to current item which can be updated
// TODO: also can check for item changes
void UseItem::tick(int ms) {
	if (!inUse()) { firstSwing = true; }
	else {
		if (info.isItem()) { useTime = info.getItem()->tick(useTime, ms); } 
		else { useTime -= ms; }
		if (!inUse()) { info.itemId = item::Id::numItems; }
	}
}

// TODO: combine with right click
// TODO: fix first swing logic
bool UseItem::leftClick(ItemInfo& item) {
	if (!info.equals(item)) { firstSwing = true; }
	if (item[Item::ItemData::leftClick]
		&& (firstSwing || item[Item::ItemData::autoUse])) {
		SDL_Point pPos = GameObjects::player().getCPos();
		SDL_Point worldMouse = GameObjects::world()
			.getWorldMousePos(UI::mouse(), pPos, false);
		usedLeft = worldMouse.x < pPos.x;
		firstSwing = false;
		// Use item
		item.getItem()->onLeftClick();
		// TODO: move to onLeftClick
		if (item[Item::ItemData::breaker]) {
			GameObjects::player().breakBlock(worldMouse);
		} 
		if (item[Item::ItemData::placer]) {
			bool success = GameObjects::player().placeBlock(worldMouse,
				item.getItem()->getBlockId());
			if (success && item[Item::ItemData::consumable]) { --item.amnt; }
		} else if (item[Item::ItemData::consumable]) { --item.amnt; }
		// TODO: support variable usetime over time / on which click
		// TODO: return use time from onClick()
		useTime = item.useTime();
		info = item;
		return true;
	}
	return false;
}

bool UseItem::rightClick(ItemInfo& item) {
	if (!info.equals(item)) { firstSwing = true; }
	if (item.isItem() && item[Item::ItemData::rightClick]
		&& (firstSwing || item[Item::ItemData::autoUse])) {
		SDL_Point pPos = GameObjects::player().getCPos();
		SDL_Point worldMouse = GameObjects::world()
			.getWorldMousePos(UI::mouse(), pPos, false);
		usedLeft = worldMouse.x < pPos.x;
		firstSwing = false;
		// Use item
		item.getItem()->onRightClick();
		// TODO: support variable usetime over time
		useTime = item.useTime();
		info = item;
		return true;
	}
	return false;
}

// Armor Inventory
ArmorInventory::ArmorInventory() : Inventory(DIM) {}

// PlayerInventory
PlayerInventory::PlayerInventory() : Inventory(DIM) {
	setMaxStack(10);
	items[dim.y - 1][dim.x - 1] = ItemInfo(item::Id::DIRT, 10);
	// Setup armor inventory
	armorInv.setPos(mRect.x2() + gameVals::INV_W() / 4, mRect.y);
	Rect armorRect = armorInv.getRect();

	craftToggle = Button(CRAFT_TOGGLE, 75);
	SharedTexture cTex = UI::assets().getAsset(CRAFT_TOGGLE);
	Rect craftR = Rect::getMinRect(cTex.get(), BUTTON_W, BUTTON_W);
	craftR.setCX(armorRect.cX());
	craftR.y = armorRect.y2() + BUTTON_W / 4;
	craftToggle.setRect(craftR);

	open = true;
	toggleOpen();
}

void PlayerInventory::draw() {
	mTex.dest = mRect;
	// TODO: only draw part, draw data
	UI::assets().drawTexture(mTex);
	SDL_Point mouse = UI::mouse();
	if (open) {
		SDL_Point p{ 0,0 };
		armorInv.draw(p);
		craftToggle.draw(p);
		//craftToggle.dest = craftR + parentPos;
		//craftToggle.textureId = SDL_PointInRect(&mouse, &craftToggle.dest) ?
		//	CRAFT_TOGGLE_HOVER : CRAFT_TOGGLE;
		//UI::assets().drawTexture(craftToggle);
	}
	if (drawDescription) {
		drawHoverItem(toInvPos(mouse - mTex.dest.topLeft()));
	}

#ifdef PLAYER_DEBUG
	if (itemUsed.inUse()) {
		TextureData td;
		td.textureId = gameVals::images() + "pause.png";
		td.dest = Rect(0, 0, gameVals::INV_W(), gameVals::INV_W());
		UI::assets().drawTexture(td);
	}
#endif
}

void PlayerInventory::drawInventory() {
	bool doToggle = !open;
	if (doToggle) { toggleOpen(); }
	Inventory::drawInventory();
	selectPos(SDL_Point{ hotbarItem,0 });
	if (doToggle) { toggleOpen(); }
	armorInv.drawInventory();
}

void PlayerInventory::handleEvents(Event& e) {
	itemUsed.tick(e.dt.milliseconds());
	if (!itemUsed.inUse()) {
		// Escape
		if (e.checkAndHandleKey(SDLK_ESCAPE, Event::ButtonStatus::RELEASED)) {
			toggleOpen();
		}

		// Hotbar
		if (e.checkScroll()) {
			if (e.scroll > 0 && hotbarItem < dim.x - 1) {
				selectHotbar(hotbarItem + 1);
			} else if (e.scroll < 0 && hotbarItem > 0) {
				selectHotbar(hotbarItem - 1);
			}
		}
		for (const auto& pair : gameVals::KEY_NUMS()) {
			if (e.checkAndHandleKey(pair.first, Event::ButtonStatus::PRESSED)) {
				selectHotbar((pair.second + 9) % 10);
			}
		}

		// Auto move items

		// Left/right click
		SDL_Point mouse = toInvPos(e.mouse - mRect.topLeft());
		SDL_Point armorMouse =
			toInvPos(e.mouse - armorInv.mRect.topLeft());
		// Check left click, try to click inventory, try to click armor inventory
		if (e.checkMouse(Event::Mouse::LEFT, Event::ButtonStatus::CLICKED)
			&& !leftClickPos(mouse) && !armorInv.leftClickPos(armorMouse)) {
			// Try to click crafting toggle
			if (craftToggle.clicked(e.mouse)) {
				std::cerr << "Open Crafting" << std::endl;
			// Click with item and update inventory
			} else if (itemUsed.leftClick(getCurrentItemRef())
				&& !heldItem.isItem()) {
				updatePos(SDL_Point{ hotbarItem, 0 });
			}
		// Check right click, try to click inventory, try to click armor inventory
		} else if (e.checkMouse(Event::Mouse::RIGHT, Event::ButtonStatus::CLICKED)
			&& !rightClickPos(mouse) && !armorInv.rightClickPos(armorMouse)) {
			// Drop the item
			if (heldItem.isItem()) {
				SDL_Point pPos = GameObjects::player().getCPos();
				SDL_Point worldMouse = GameObjects::world()
					.getWorldMousePos(UI::mouse(), pPos, false);
				// Drop Item
				GameObjects::world().dropItem(DroppedItem(heldItem),
					worldMouse.x < pPos.x ? DroppedItem::DropDir::left
					: DroppedItem::DropDir::right);
				heldItem = ItemInfo::NO_ITEM();
			// Use the item
			} else { itemUsed.rightClick(getCurrentItemRef()); }
		}

		// Check hovering
		if (e.checkHover()) {
			if (SDL_PointInRect(&e.mouse, &mRect)) {
				drawDescription = true;
			} else if (SDL_PointInRect(&e.mouse, &armorInv.mRect)) {
				armorInv.drawDescription = true;
			}
		}

		// Check holding right click
		if (!e.checkMouse(Event::Mouse::RIGHT, Event::ButtonStatus::HELD)) {
			amntTransferred = 0.;
		}
	}
}

bool PlayerInventory::leftClickPos(SDL_Point pos) {
	if (validPos(pos)) {
		if (open) { Inventory::leftClickPos(pos); }
		else { selectHotbar(pos.x); }
		return true;
	}
	return false;
}

bool PlayerInventory::rightClickPos(SDL_Point pos) {
	if (open && validPos(pos)) {
		// Try to move item to armor inventory
		auto item = getItem(pos);
		if (SDL_GetModState() & KMOD_SHIFT != 0) {
			if (item.isItem() && armorInv.pickUpItem(item)) {
				updatePos(pos);
				return true;
			}
		} else { return Inventory::rightClickPos(pos); }
	}
	return false;
}

ItemInfo PlayerInventory::leftClickItem(ItemInfo item, int itemMaxStack) {
	// If both items are nothing then there's nothing to grab
	if (item.isItem() || heldItem.isItem()) {
		// Transfer the item to here
		if (!heldItem.isItem()) {
			heldItem = item;
			int transferAmnt = std::min(item.amnt, maxStack);
			heldItem.amnt = transferAmnt;
			item.amnt -= transferAmnt;
		// Transfer the item to there
		} else {
			if (!item.isItem()) { item = heldItem; item.amnt = 0; }
			if (heldItem.sameAs(item)) {
				int transferAmnt = std::min(heldItem.amnt, itemMaxStack - item.amnt);
				item.amnt += transferAmnt;
				heldItem.amnt -= transferAmnt;
			}
		}
		itemUsed.setUseTime(heldItem.isItem() ? heldItem.useTime() : 300);
	}
	return item;
}

ItemInfo PlayerInventory::rightClickItem(ItemInfo item, int itemMaxStack) {
	if (item.isItem() && (!heldItem.isItem() || item.sameAs(heldItem))) {
		// Time to wait until next item transfer
		double prev = amntTransferred;
		// Get tranfer amnt
		if (amntTransferred == 0.) { amntTransferred = 1.; }
		else { amntTransferred *= 1.1; }
		int idealGrab = (int)(amntTransferred - prev);
		int maxGrab = heldItem.maxStack(maxStack) - heldItem.amnt;
		int transferAmnt = std::min(item.amnt, std::min(idealGrab, maxGrab));
		heldItem.amnt += transferAmnt;
		item.amnt -= transferAmnt;
	} else { amntTransferred = 0.; }
	return item;
}

void PlayerInventory::autoMoveItem(SDL_Point loc) {}

void PlayerInventory::onChangeHeld() {}

void PlayerInventory::selectHotbar(int pos) {
	if (pos != hotbarItem && pos < dim.x) {
		unselectPos(SDL_Point{ hotbarItem,0 });
		hotbarItem = pos;
		selectPos(SDL_Point{ hotbarItem,0 });
	}
}

void PlayerInventory::useItem() {
	ItemInfo& item = getCurrentItemRef();
	if (item.isItem()) {
		--item.amnt;
		if (!item.isItem()) { onChangeHeld(); }
	}
}

void PlayerInventory::dropItem() {
	if (heldItem.isItem()) {
		DroppedItem drop(heldItem);
		heldItem.amnt = 0;
		// TODO: left or right?
		GameObjects::world().dropItem(drop, DroppedItem::DropDir::left);
	}
}

void PlayerInventory::toggleOpen() {
	open = !open;
	int difference = gameVals::INV_W() * (dim.y - 1);
	mRect.h += open ? difference : -difference;
	mTex.area = open ? TextureData::NO_RECT() : Rect(0, 0, mRect.w, mRect.h);
}

const ItemInfo& PlayerInventory::getCurrentItem() const {
	return heldItem.isItem() ? heldItem : items[0][hotbarItem];
}

ItemInfo& PlayerInventory::getCurrentItemRef() {
	return heldItem.isItem() ? heldItem : items[0][hotbarItem];
}