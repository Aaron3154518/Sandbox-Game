#include "PlayerInventory.h"
#include "../UIs/Game.h"
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
		if (!inUse()) { info.itemId = item::Id::NONE; }
	}
}

// TODO: combine with right click
// TODO: fix first swing logic
bool UseItem::leftClick(ItemInfo& item) {
	if (!info.equals(item)) { firstSwing = true; }
	if (item[Item::ItemData::leftClick]
		&& (firstSwing || item[Item::ItemData::autoUse])) {
		SDL_Point pPos = Game::Player().getCPos();
		SDL_Point worldMouse = Game::World()
			.getWorldMousePos(mousePos(), pPos, false);
		usedLeft = worldMouse.x < pPos.x;
		firstSwing = false;
		// Use item
		item.getItem()->onLeftClick();
		// TODO: move to onLeftClick
		if (item[Item::ItemData::breaker]) {
			Game::Player().breakBlock(worldMouse);
		} 
		if (item[Item::ItemData::placer]) {
			bool success = Game::Player().placeBlock(worldMouse,
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
		SDL_Point pPos = Game::Player().getCPos();
		SDL_Point worldMouse = Game::World()
			.getWorldMousePos(mousePos(), pPos, false);
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

void UseItem::setUseTime(int ms) {
	useTime = ms;
	if (!inUse()) { info = ItemInfo::NO_ITEM(); }
}

void UseItem::clear() {
	info = ItemInfo::NO_ITEM();
	useTime = 0;
	firstSwing = false;
}

// Armor Inventory
ArmorInventory::ArmorInventory() : Inventory(DIM) {}

// PlayerInventory
PlayerInventory::PlayerInventory() : Inventory(DIM) {
	// Setup armor inventory
	armorInv.setPos(mRect.x2() + gameVals::INV_W() / 4, mRect.y);
	Rect armorRect = armorInv.getRect();

	craftToggle = Button(CRAFT_TOGGLE, 75);
	SharedTexture cTex = Window::Get().assets().getAsset(CRAFT_TOGGLE);
	Rect craftR = Rect::getMinRect(cTex.get(), BUTTON_W, BUTTON_W);
	craftR.setCX(armorRect.cX());
	craftR.y = armorRect.y2() + BUTTON_W / 4;
	craftToggle.setRect(craftR);

	if (open) { toggleOpen(); }
}

void PlayerInventory::draw() {
	rData.dest = mRect;
	// TODO: only draw part, draw data
	Window::Get().assets().drawTexture(rData);
	if (open) {
		SDL_Point p{ 0,0 };
		armorInv.draw(p);
		craftToggle.draw(p);
	}

	if (crafting.open) {
		crafting.draw();
	}

	if (drawDescription) {
		drawHoverItem(toInvPos(mousePos() - rData.dest.topLeft()));
	}

#ifdef PLAYER_DEBUG
	if (itemUsed.inUse()) {
		TextureData td;
		td.textureId = gameVals::images() + "pause.png";
		td.dest = Rect(0, 0, gameVals::INV_W(), gameVals::INV_W());
		Window::Get().assets().drawTexture(td);
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
	// Check to reset right transfer
	if (!didRightTransfer && rightTransfer != 0.) {
		rightTransfer = 0.;
	}
	didRightTransfer = false;

	itemUsed.tick(e.dt.milliseconds());
	if (!itemUsed.inUse()) {
		// Escape
		if (any8(e[SDLK_ESCAPE], Event::Button::RELEASED)) {
			toggleOpen();
			e.handleKey(SDLK_ESCAPE);
		}

		// Crafting UI
		if (!crafting.open || !crafting.handleEvents(e)) {
			// Hotbar
			if (e.checkScroll()) {
				if (e.scroll > 0 && hotbarItem < dim.x - 1) {
					selectHotbar(hotbarItem + 1);
				} else if (e.scroll < 0 && hotbarItem > 0) {
					selectHotbar(hotbarItem - 1);
				}
			}
			for (const auto& pair : gameVals::KEY_NUMS()) {
				if (any8(e[pair.first], Event::Button::PRESSED)) {
					selectHotbar((pair.second + 9) % 10);
					e.handleKey(pair.first);
				}
			}

			// TODO: Auto move items

			// Left/right click
			SDL_Point mouse = toInvPos(e.mouse - mRect.topLeft());
			SDL_Point armorMouse =
				toInvPos(e.mouse - armorInv.mRect.topLeft());
			// Check left click, try to click inventory,
			// try to click armor inventory
			if (any8(e[Event::Mouse::LEFT], Event::Button::M_CLICKED)
				&& !leftClickPos(mouse) && !armorInv.leftClickPos(armorMouse)) {
				// Try to click crafting toggle
				if (craftToggle.clicked(e.mouse)) {
					crafting.toggleOpen();
					// Click with item and update inventory
				} else if (itemUsed.leftClick(getCurrentItemRef())
					&& !heldItem.isItem()) {
					updatePos(SDL_Point{ hotbarItem, 0 });
				}
			// Check right held, try to click inventory
			} else if (any8(e[Event::Mouse::RIGHT], Event::Button::HELD)
				&& !rightClickPos(mouse)) {
				// Click armor inventory
				armorInv.rightClickPos(armorMouse);
			// Check right held, try to click inventory, try to click armor inventory
			} else if (any8(e[Event::Mouse::RIGHT], Event::Button::M_CLICKED)) {
				bool good = true;
				std::vector<Rect> rects = {
					mRect, armorInv.mRect, craftToggle.getRect() };
				// if (crafting.open) { rects.push_back(crafting.getRect()); }
				for (const Rect& r : rects) {
					if (SDL_PointInRect(&e.mouse, &r)) { good = false; break; }
				}
				if (good) {
					// Drop the item
					if (heldItem.isItem()) {
						SDL_Point pPos = Game::Player().getCPos();
						SDL_Point worldMouse = Game::World()
							.getWorldMousePos(mousePos(), pPos, false);
						// Drop Item
						Game::World().dropItem(DroppedItem(heldItem),
							worldMouse.x < pPos.x ? DroppedItem::DropDir::left
							: DroppedItem::DropDir::right);
						heldItem = ItemInfo::NO_ITEM();
						// Use the item
					} else { itemUsed.rightClick(getCurrentItemRef()); }
				}
			}
		}

		// Check hovering
		if (e.checkHover()) {
			if (SDL_PointInRect(&e.mouse, &mRect)) {
				drawDescription = true;
			} else if (SDL_PointInRect(&e.mouse, &armorInv.mRect)) {
				armorInv.drawDescription = true;
			}
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

ItemInfo PlayerInventory::leftClickItem(ItemInfo item, int otherMaxStack) {
	// If both items are nothing then there's nothing to grab
	if (item.isItem() || heldItem.isItem()) {
		// Transfer the item to held
		if (!heldItem.isItem()) {
			heldItem = item;
			// TODO: Ignore maxStack if player inventory
			int transferAmnt = std::min(item.amnt, maxStack);
			heldItem.amnt = transferAmnt;
			item.amnt -= transferAmnt;
		// Transfer the item to there
		} else {
			if (!item.isItem()) { item = heldItem; item.amnt = 0; }
			// Add to item
			if (heldItem.sameAs(item)) {
				// If item is empty, disregard item max stack
				int itemMaxStack = item.amnt == 0 ? otherMaxStack :
					item.maxStack(otherMaxStack);
				int transferAmnt = std::min(heldItem.amnt,
					std::max(itemMaxStack - item.amnt, 0));
				item.amnt += transferAmnt;
				heldItem.amnt -= transferAmnt;
			// Swap items
			} else if (heldItem.amnt < otherMaxStack && item.amnt < maxStack) {
				ItemInfo tmp = item;
				item = heldItem;
				heldItem = tmp;
			}
		}
		itemUsed.setUseTime(heldItem.isItem() ? heldItem.useTime() : 300);
	}
	return item;
}

ItemInfo PlayerInventory::rightClickItem(ItemInfo item, int otherMaxStack) {
	if (item.isItem() && (!heldItem.isItem() || heldItem.sameAs(item))) {
		// Time to wait until next item transfer
		double prev = rightTransfer;
		// Get tranfer amnt
		rightTransfer = rightTransfer == 0. ? 1. : rightTransfer * 1.01;
		int idealGrab = (int)rightTransfer - (int)prev;
		int maxGrab = std::max(heldItem.maxStack(maxStack) - heldItem.amnt, 0);
		int transferAmnt = std::min(item.amnt, std::min(idealGrab, maxGrab));
		if (!heldItem.isItem()) { heldItem = item; heldItem.amnt = 0; }
		heldItem.amnt += transferAmnt;
		item.amnt -= transferAmnt;
		didRightTransfer = true;
	}
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
		Game::World().dropItem(drop, DroppedItem::DropDir::left);
	}
}

void PlayerInventory::toggleOpen() {
	open = !open;
	int difference = gameVals::INV_W() * (dim.y - 1);
	mRect.h += open ? difference : -difference;
	rData.area = open ? RenderData::NO_RECT() : Rect(0, 0, mRect.w, mRect.h);
	if (!open) { crafting.setOpen(false); }
}

const ItemInfo& PlayerInventory::getCurrentItem() const {
	return heldItem.isItem() ? heldItem : items[0][hotbarItem];
}

ItemInfo& PlayerInventory::getCurrentItemRef() {
	return heldItem.isItem() ? heldItem : items[0][hotbarItem];
}

Rect PlayerInventory::getFullRect() const {
	Rect result = mRect;
	if (!open) { result.h += (DIM.y - 1) * gameVals::INV_W(); }
	return result;
}

void PlayerInventory::clear() {
	Inventory::clear();
	armorInv.clear();
	heldItem = ItemInfo::NO_ITEM();
	itemUsed.clear();
}

void PlayerInventory::reset() {
	hotbarItem = rightTransfer = 0;
	if (open) { toggleOpen(); }
	itemUsed.clear();
}

void PlayerInventory::read(IO& io) {
	reset();
	Inventory::read(io);
	armorInv.read(io);
	heldItem.read(io);
}

void PlayerInventory::write(IO& io) {
	Inventory::write(io);
	armorInv.write(io);
	heldItem.write(io);
}
