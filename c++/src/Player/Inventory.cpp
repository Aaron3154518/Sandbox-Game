#include "Inventory.h"
#include "../GameObjects.h"

const SDL_Color Inventory::BKGRND{ 0, 255, 0, 150 };
const SDL_Color Inventory::SELECT_COLOR{ 175,175,0,255 };
const std::string Inventory::FONT_ID = Window::Get().assets().addFont(
	FontData{ gameVals::INV_FONT_W(), gameVals::INV_FONT_W() });

SDL_Point Inventory::toInvPos(SDL_Point pos) {
	SDL_Point result = (pos - gameVals::INV_MARGIN()) / gameVals::INV_W();
	if (pos.x < 0) { --result.x; }
	if (pos.y < 0) { --result.y; }
	return result;
}
SDL_Point Inventory::toPxPos(SDL_Point pos) {
	return (pos * gameVals::INV_W()) + gameVals::INV_MARGIN();
}
Rect Inventory::getInvRect(SDL_Point pos) {
	return Rect(0, 0, gameVals::INV_W(), gameVals::INV_W()) + toPxPos(pos);
}

// Inventory
Inventory::Inventory(SDL_Point _dim) : dim(_dim) {
	std::vector<ItemInfo> row(dim.x);
	items.resize(dim.y, row);

	td.fontId = FONT_ID;
	td.color = WHITE;
	td.yMode = td.xMode = TextData::PosType::botright;
	td.w = td.h = gameVals::INV_FONT_W();

	mRect = Rect(0, 0, dim.x * gameVals::INV_W() + 2 * gameVals::INV_MARGIN(),
		dim.y * gameVals::INV_W() + 2 * gameVals::INV_MARGIN());
	mTex.texture = Window::Get().assets().createTexture(mRect.w, mRect.h);
}

void Inventory::draw(SDL_Point topLeft) {
	SDL_Point mouse = mousePos();
	mTex.dest = mRect + topLeft;
	Window::Get().assets().drawTexture(mTex);
	if (drawDescription) {
		drawHoverItem(toInvPos(mouse - mTex.dest.topLeft()));
	}
}

void Inventory::drawInventory() {
	AssetManager& assets = Window::Get().assets();
	assets.setRenderTarget(mTex.texture.get());
	assets.rect(NULL, BKGRND, SDL_BLENDMODE_BLEND);
	assets.thickRect(Rect(0, 0, mRect.w, mRect.h),
		gameVals::INV_MARGIN(), AssetManager::BorderType::inside, BLACK);
	assets.resetRenderTarget();
	SDL_Point loc;
	for (loc.y = 0; loc.y < dim.y; loc.y++) {
		for (loc.x = 0; loc.x < dim.x; loc.x++) {
			updatePos(loc);
			assets.setRenderTarget(mTex.texture.get());
			assets.thickRect(getInvRect(loc), gameVals::INV_MARGIN(),
				AssetManager::BorderType::inside, BLACK);
			assets.resetRenderTarget();
		}
	}
}

void Inventory::handleEvents(Event& e, SDL_Point topLeft) {
	// TODO auto move item

	// Left/right click
	SDL_Point mouse = toInvPos(e.mouse - topLeft - mRect.topLeft());
	if (e.checkMouse(Event::Mouse::LEFT, Event::ButtonStatus::CLICKED)) {
		leftClickPos(mouse);
	} else if (e.checkMouse(Event::Mouse::RIGHT, Event::ButtonStatus::CLICKED)) {
		rightClickPos(mouse);
	}
	drawDescription = e.checkHover();
}

bool Inventory::leftClickPos(SDL_Point pos) {
	if (!validPos(pos)) { return false; }
	if (SDL_GetModState() & KMOD_SHIFT != 0) {
		autoMoveItem(pos);
	} else {
		ItemInfo& item = items[pos.y][pos.x];
		item = GameObjects::playerInv()
			.leftClickItem(item, item.maxStack(maxStack));
		updatePos(pos);
		// TODO: ActiveUI on inv update
	}
	return true;
}

bool Inventory::rightClickPos(SDL_Point pos) {
	if (validPos(pos)) {
		ItemInfo& item = items[pos.y][pos.x];
		item = GameObjects::playerInv()
			.rightClickItem(item, item.maxStack(maxStack));
		updatePos(pos);
		// TODO: ActiveUI on inv update
		return true;
	}
	return false;
}

std::list<SDL_Point> Inventory::getSpaceForItem(const ItemInfo& item) const {
	// Positions which are neither full nor empty
	std::list<SDL_Point> notFull, empty;
	// Make sure we can pick this item up
	if (!itemAllowed(item.itemId)) { return notFull; }
	// Get amnt to add and max stack size
	int amnt = item.amnt;
	int _maxStack = item.maxStack(maxStack);
	// Go through the inventory and find valid positions
	SDL_Point pos;
	for (pos.y = 0; pos.y < dim.y; pos.y++) {
		for (pos.x = 0; pos.x < dim.x; pos.x++) {
			const ItemInfo& invItem = items[pos.y][pos.x];
			if (!invItem.isItem()) { empty.push_back(pos); }
			else if (invItem.sameAs(item) && invItem.amnt < maxStack) {
				notFull.push_back(pos);
				amnt -= (_maxStack - invItem.amnt);
				if (amnt <= 0) { return notFull; }
			}
		}
	}
	for (SDL_Point& pos : empty) {
		notFull.push_back(pos);
		if ((amnt -= _maxStack) <= 0) { break; }
	}
	return notFull;
}

bool Inventory::isSpaceForItem(const ItemInfo& item) const {
	// Make sure we can pick this item up
	if (!itemAllowed(item.itemId)) { return false; }
	// Get amnt to add and max stack size
	int amnt = item.amnt;
	int _maxStack = item.maxStack(maxStack);
	// Go through the inventory and find a valid position
	SDL_Point pos;
	for (pos.y = 0; pos.y < dim.y; pos.y++) {
		for (pos.x = 0; pos.x < dim.x; pos.x++) {
			const ItemInfo& invItem = items[pos.y][pos.x];
			if (!invItem.isItem() ||
				(invItem.sameAs(item) && invItem.amnt < _maxStack)) {
				return true;
			}
		}
	}
}

bool Inventory::pickUpItem(ItemInfo& item) {
	auto space = getSpaceForItem(item);
	int _maxStack = item.maxStack(maxStack);
	for (SDL_Point& pos : space) {
		ItemInfo& invItem = items[pos.y][pos.x];
		if (!invItem.isItem()) {
			invItem = item;
			invItem.amnt = 0;
		}
		int transferAmnt = std::min(item.amnt, _maxStack - invItem.amnt);
		invItem.amnt += transferAmnt;
		item.amnt -= transferAmnt;
		updatePos(pos);
		if (item.amnt < 0) {
			std::cerr << "Inventory::pickUpItem(): Item amnt < 0" << std::endl;
		} else if (item.amnt == 0) { return true; }
	}
	return false;
}

void Inventory::autoMoveItem(SDL_Point loc) {
	std::cerr << "Auto move: " << loc << std::endl;
}

void Inventory::selectPos(SDL_Point pos) {
	if (validPos(pos)) {
		AssetManager& assets = Window::Get().assets();
		assets.setRenderTarget(mTex.texture.get());
		assets.thickRect(getInvRect(pos), 2 * gameVals::INV_MARGIN(),
			AssetManager::BorderType::middle, SELECT_COLOR);
		assets.resetRenderTarget();
	}
}

void Inventory::unselectPos(SDL_Point pos) {
	if (validPos(pos)) {
		AssetManager& assets = Window::Get().assets();
		assets.setRenderTarget(mTex.texture.get());
		assets.thickRect(getInvRect(pos), 2 * gameVals::INV_MARGIN(),
			AssetManager::BorderType::middle, BLACK);
		assets.resetRenderTarget();
	}
}

void Inventory::setPos(SDL_Point pos) {
	mRect.setTopLeft(pos);
}

void Inventory::setItemList(std::set<item::Id>& list, bool isWhiteList) {
	itemList = list;
	whiteList = isWhiteList;
}

const ItemInfo& Inventory::getItem(int x, int y) const {
	return getItem(SDL_Point{ x, y });
}
const ItemInfo& Inventory::getItem(SDL_Point loc) const {
	return validPos(loc) ? items[loc.y][loc.x] : ItemInfo::NO_ITEM();
}

void Inventory::setItem(int x, int y, const ItemInfo& item) {
	setItem(SDL_Point{ x,y }, item);
}
void Inventory::setItem(SDL_Point loc, const ItemInfo& item) {
	if (validPos(loc)) {
		items[loc.y][loc.x] = item;
		updatePos(loc);
	}
}

bool Inventory::empty() const {
	for (auto& row : items) {
		for (auto& item : row) {
			if (item.isItem()) { return true; }
		}
	}
	return false;
}

bool Inventory::itemAllowed(item::Id id) const {
	if (itemList.empty()) { return true; }
	auto it = itemList.find(id);
	return whiteList == (it != itemList.end());
}

bool Inventory::validPos(SDL_Point loc) const {
	return loc.x >= 0 && loc.y >= 0 && loc.x < dim.x && loc.y < dim.y;
}

void Inventory::read(IO& io) {
	SDL_Point readDim;
	io.read(readDim.x);
	io.read(readDim.y);
	ItemInfo tmp;
	for (int y = 0; y < readDim.y; y++) {
		for (int x = 0; x < readDim.x; x++) {
			if (x < dim.x && y < dim.y) { items[y][x].read(io); }
			else { tmp.read(io); }
		}
	}
	drawInventory();
}

void Inventory::write(IO& io) {
	io.write(dim.x);
	io.write(dim.y);
	for (auto& row : items) {
		for (auto& item : row) {
			item.write(io);
		}
	}
}

void Inventory::updatePos(SDL_Point loc) {
	if (!validPos(loc)) { return; }

	const ItemInfo& item = getItem(loc);
	Rect r(0, 0, gameVals::INV_IMG_W(), gameVals::INV_IMG_W());
	r.setCX(((double)loc.x + .5) * gameVals::INV_W());
	r.setCY(((double)loc.y + .5) * gameVals::INV_W());
	r += toPxPos(SDL_Point{ 0,0 });

	AssetManager& assets = Window::Get().assets();
	assets.setRenderTarget(mTex.texture.get());
	assets.rect(&r, BKGRND);
	if (item.isItem()) {
		TextureData itemTex;
		itemTex.texture = item.getImage();
		itemTex.dest = Rect::getMinRect(itemTex.texture.get(),
			gameVals::INV_IMG_W(), gameVals::INV_IMG_W());
		itemTex.dest.setCenter(r.cX(), r.cY());
		itemTex.boundary = Rect(0, 0, mRect.w, mRect.h);
		assets.drawTexture(itemTex);
		td.text = std::to_string(item.amnt);
		td.x = r.x2(); td.y = r.y2();
		assets.drawText(td);
	}
	assets.resetRenderTarget();
}

void Inventory::drawHoverItem(SDL_Point pos) {
	//std::cerr << "Hover Item: " << pos << std::endl;
}
