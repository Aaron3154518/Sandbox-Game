#include "PlayerInventory.h"
#include "../GameObjects.h"

const SDL_Point ArmorInventory::DIM{ 1,4 };
const SDL_Point PlayerInventory::DIM{ 10,5 };

const SDL_Color PlayerInventory::SELECT_COLOR{ 128,128,0,255 };
const SDL_Color PlayerInventory::HOVER_COLOR{ 200,200,200,120 };
const int PlayerInventory::BUTTON_W = gameVals::INV_W() / 2;
const std::string PlayerInventory::CRAFT_TOGGLE =
gameVals::images() + "crafting_toggle.png";

// Armor Inventory
ArmorInventory::ArmorInventory() : Inventory(DIM) {}

// PlayerInventory
PlayerInventory::PlayerInventory() : Inventory(DIM) {
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

void PlayerInventory::draw(SDL_Point parentPos) {
	mTex.dest = mRect + parentPos;
	// TODO: only draw part, draw data
	UI::assets().drawTexture(mTex);
	SDL_Point mouse = UI::mouse();
	if (open) {
		armorInv.draw(parentPos);
		craftToggle.draw(parentPos);
		//craftToggle.dest = craftR + parentPos;
		//craftToggle.textureId = SDL_PointInRect(&mouse, &craftToggle.dest) ?
		//	CRAFT_TOGGLE_HOVER : CRAFT_TOGGLE;
		//UI::assets().drawTexture(craftToggle);
	}
	if (SDL_PointInRect(&mouse, &mTex.dest)) {
		drawHoverItem(toInvPos(mouse - mTex.dest.topLeft()));
	}
}

void PlayerInventory::drawInventory() {
	bool doToggle = !open;
	if (doToggle) { toggleOpen(); }
	Inventory::drawInventory();
	if (doToggle) { toggleOpen(); }
	armorInv.drawInventory();
}

bool PlayerInventory::leftClick(SDL_Point mouse) {
	if (SDL_PointInRect(&mouse, &mRect)) {
		if (open) { Inventory::leftClick(mouse); }
		else { selectHotbar((mouse.x - mRect.x) / gameVals::INV_W()); }
		return true;
	} else if (open) {
		if (craftToggle.clicked(mouse)) {
			std::cerr << "Toggle Crafting" << std::endl;
			// player.craftingOpen = !player.craftingOpen;
			// player.useTime = .3;
			return true;
		} else { return armorInv.leftClick(mouse); }
	}
	return false;
}

bool PlayerInventory::rightClick(SDL_Point mouse) {
	if (open) {
		if (SDL_PointInRect(&mouse, &mRect)) {
			// Try to move item to armor inventory
			auto item =
				getItem((mouse - mRect.topLeft()) / gameVals::INV_W());
			if (item.isItem() && armorInv.pickUpItem(item)) { return true; }
			return Inventory::rightClick(mouse);
		} else { return armorInv.rightClick(mouse); }
	}
	return false;
}

void PlayerInventory::autoMoveItem(SDL_Point loc) {}

void PlayerInventory::onChangeHeld() {}

void PlayerInventory::selectHotbar(unsigned int pos) {
	if (pos != hotbarItem && pos < dim.x) {
		UI::setRenderTarget(mTex.texture.get());
		Rect r(hotbarItem * gameVals::INV_W(), 0,
			gameVals::INV_W(), gameVals::INV_W());
		r += toPxPos(SDL_Point{ 0,0 });
		UI::assets().thickRect(r, gameVals::INV_MARGIN(),
			AssetManager::BorderType::inside, BLACK);
		hotbarItem = pos;
		r = Rect(hotbarItem * gameVals::INV_W(), 0,
			gameVals::INV_W(), gameVals::INV_W());
		UI::assets().thickRect(r, gameVals::INV_MARGIN(),
			AssetManager::BorderType::inside, SELECT_COLOR);
		UI::resetRenderTarget();
	}
}

void PlayerInventory::scrollHotbar(bool up) {
	if (up && hotbarItem > 0) { selectHotbar(--hotbarItem); } else if (!up && hotbarItem < dim.x - 1) { selectHotbar(++hotbarItem); }
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

void PlayerInventory::keyPressed(SDL_KeyCode key) {
	if (key == SDLK_ESCAPE) { toggleOpen(); } else {
		auto it = gameVals::KEY_NUMS().find(key);
		if (it != gameVals::KEY_NUMS().end()) {
			selectHotbar(it->second);
		}
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