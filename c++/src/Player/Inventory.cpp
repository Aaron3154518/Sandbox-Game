#include "Inventory.h"
#include "../UIs/Game.h"

// Item max stack is not used for inventory-inventory transfers
// EXCEPT when adding to an existing item

const SDL_Color Inventory::BKGRND{ 0, 255, 0, 150 };
const SDL_Color Inventory::SELECT_COLOR{ 175,175,0,255 };
const std::string Inventory::FONT_ID = Window::Get().assets().addFont(
	FontData{ gameVals::INV_FONT_W(), gameVals::INV_FONT_W() });

SDL_Point Inventory::toInvPos(SDL_Point pos) {
	return (pos - gameVals::INV_MARGIN()) / gameVals::INV_W();
}
SDL_Point Inventory::toPxPos(SDL_Point pos) {
	return (pos * gameVals::INV_W()) + gameVals::INV_MARGIN();
}
Rect Inventory::getInvRect(SDL_Point pos) {
	return Rect(0, 0, gameVals::INV_W(), gameVals::INV_W()) + toPxPos(pos);
}

void Inventory::drawItem(const ItemInfo& info, TextData textData,
	Rect r, Rect boundary) {
	AssetManager& assets = Window::Get().assets();
	RenderData rData;
	rData.boundary = boundary.invalid() ? r : boundary;
	// Item image
	rData.asset.setTexture(info.getImage());
	rData.fitToAsset(assets, r.w, r.h);
	rData.dest.setCenter(r.cX(), r.cY());
	assets.drawTexture(rData);
	// Item text
	textData.text = std::to_string(info.amnt);
	rData.asset.setTexture(assets.renderText(textData));
	rData.fitToAsset(assets, 0,
		r.h * gameVals::INV_FONT_W() / gameVals::INV_IMG_W());
	rData.dest.setBottomRight(r.bottomRight());
	assets.drawTexture(rData);
}

// Inventory
Inventory::Inventory(SDL_Point _dim) : dim(_dim) {
	std::vector<ItemInfo> row(dim.x);
	items.resize(dim.y, row);

	textData.fontId = FONT_ID;
	textData.color = WHITE;

	mRect = Rect(0, 0, dim.x * gameVals::INV_W() + 2 * gameVals::INV_MARGIN(),
		dim.y * gameVals::INV_W() + 2 * gameVals::INV_MARGIN());
	rData.asset.setTexture(
		Window::Get().assets().createTexture(mRect.w, mRect.h));
}

void Inventory::draw(SDL_Point topLeft) {
	SDL_Point mouse = mousePos();
	rData.dest = mRect + topLeft;
	Window::Get().assets().drawTexture(rData);
	if (drawDescription) {
		drawHoverItem(toInvPos(mouse - rData.dest.topLeft()));
	}
}

void Inventory::drawInventory() {
	AssetManager& assets = Window::Get().assets();
	assets.setRenderTarget(rData.asset.texture.get());
	RectData({ BKGRND, SDL_BLENDMODE_BLEND }).set().render(assets);
	RectData({ BLACK }).set(Rect(0, 0, mRect.w, mRect.h),
		-gameVals::INV_MARGIN()).render(assets);
	assets.resetRenderTarget();
	SDL_Point loc;
	for (loc.y = 0; loc.y < dim.y; loc.y++) {
		for (loc.x = 0; loc.x < dim.x; loc.x++) {
			// updatePos resets render target
			updatePos(loc);
			assets.setRenderTarget(rData.asset.texture.get());
			RectData({ BLACK }).set(getInvRect(loc),
				-gameVals::INV_MARGIN()).render(assets);
			assets.resetRenderTarget();
		}
	}
}

void Inventory::handleEvents(Event& e, SDL_Point topLeft) {
	// TODO auto move item

	// Left/right click
	SDL_Point mouse = toInvPos(e.mouse - topLeft - mRect.topLeft());
	if (any8(e[Event::Mouse::LEFT], Event::Button::M_CLICKED)) {
		leftClickPos(mouse);
	} else if (any8(e[Event::Mouse::RIGHT], Event::Button::HELD)) {
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
		item = Game::Player().getInventory()
			.leftClickItem(item, maxStack);
		updatePos(pos);
		// TODO: ActiveUI on inv update
	}
	return true;
}

bool Inventory::rightClickPos(SDL_Point pos) {
	if (validPos(pos)) {
		ItemInfo& item = items[pos.y][pos.x];
		item = Game::Player().getInventory()
			.rightClickItem(item, maxStack);
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
	if (!itemAllowed(item.itemId) || !item.isItem()) { return notFull; }
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
	if (!itemAllowed(item.itemId) || !item.isItem()) { return false; }
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
	return false;
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
		assets.setRenderTarget(rData.asset.texture.get());
		RectData({ SELECT_COLOR }).set(getInvRect(pos),
			2 * gameVals::INV_MARGIN(), true).render(assets);
		assets.resetRenderTarget();
	}
}

void Inventory::unselectPos(SDL_Point pos) {
	if (validPos(pos)) {
		AssetManager& assets = Window::Get().assets();
		assets.setRenderTarget(rData.asset.texture.get());
		RectData({}).set(getInvRect(pos), 2 * gameVals::INV_MARGIN(), true).
			render(assets);
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

void Inventory::clear() {
	for (auto& row : items) {
		for (auto& item : row) {
			item = ItemInfo::NO_ITEM();
		}
	}
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
	assets.setRenderTarget(rData.asset.texture.get());
	RectData({ BKGRND }).set(r).render(assets);
	if (item.isItem()) { drawItem(item, textData, r); }
	assets.resetRenderTarget();
}

void Inventory::drawHoverItem(SDL_Point pos) {
	//std::cerr << "Hover Item: " << pos << std::endl;
}