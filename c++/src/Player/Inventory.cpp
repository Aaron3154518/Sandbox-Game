#include "Inventory.h"
#include "../GameObjects.h"

const SDL_Color Inventory::BKGRND{ 0, 255, 0, 64 };
const ItemInfo Inventory::NO_ITEM;

SDL_Point Inventory::toInvPos(SDL_Point pos) {
	pos -= SDL_Point{ gameVals::INV_MARGIN(), gameVals::INV_MARGIN() };
	pos /= gameVals::INV_W();
	return pos;
}

// Inventory
Inventory::Inventory(SDL_Point _dim) : dim(_dim) {
	std::vector<ItemInfo> row(dim.x);
	items.resize(dim.y, row);

	td.fontId = gameVals::INV_FONT();
	td.color = BLACK;
	td.yMode = td.xMode = TextData::PosType::botright;
	td.w = td.h = gameVals::INV_W();

	mRect = Rect(0, 0, dim.x * gameVals::INV_W() + 2 * gameVals::INV_MARGIN(),
		dim.y * gameVals::INV_W() + 2 * gameVals::INV_MARGIN());
	mTex = UI::assets().createTexture(mRect.w, mRect.h);
	drawInventory();
}

void Inventory::draw(SDL_Point parentPos) {
	SDL_Point mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);
	Rect r = mRect + parentPos;
	UI::assets().drawTexture(mTex.get(), r);
	if (SDL_PointInRect(&mouse, &r)) {
		mouse -= mRect.topLeft();
		drawHoverItem(toInvPos(mouse));
	}
}

void Inventory::drawInventory() {
	UI::setRenderTarget(mTex.get());
	UI::assets().rect(NULL, BLACK);
	UI::resetRenderTarget();
	SDL_Point loc;
	for (loc.y = 0; loc.y < dim.y; loc.y++) {
		for (loc.x = 0; loc.x < dim.x; loc.x++) {
			updateItem(loc);
		}
	}
}

void Inventory::setItemList(std::set<item::Id>& list, bool isWhiteList) {
	itemList = list;
	whiteList = isWhiteList;
}

const ItemInfo& Inventory::getItem(int x, int y) const {
	return getItem(SDL_Point{ x, y });
}
const ItemInfo& Inventory::getItem(SDL_Point loc) const {
	return validPos(loc) ? items[loc.y][loc.x] : NO_ITEM;
}

void Inventory::setItem(int x, int y, const ItemInfo& item) {
	setItem(SDL_Point{ x,y }, item);
}
void Inventory::setItem(SDL_Point loc, const ItemInfo& item) {
	if (validPos(loc)) {
		items[loc.y][loc.x] = item;
		updateItem(loc);
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
	return loc.x >= 0 && loc.y >= 0 && loc.x < dim.x&& loc.y < dim.y;
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
}

void Inventory::write(IO& io) const {
	io.write(dim.x);
	io.write(dim.y);
	for (auto& row : items) {
		for (auto& item : row) {
			item.write(io);
		}
	}
}

void Inventory::updateItem(SDL_Point loc) {
	if (!validPos(loc)) { return; }

	const ItemInfo& item = getItem(loc);
	Rect r(0, 0, gameVals::INV_IMG_W(), gameVals::INV_IMG_W());
	r.setCX(((double)loc.x + .5) * gameVals::INV_W());
	r.setCY(((double)loc.y + .5) * gameVals::INV_W());
	r.x += gameVals::INV_MARGIN(); r.y += gameVals::INV_MARGIN();

	AssetManager& assets = UI::assets();
	UI::setRenderTarget(mTex.get());
	assets.rect(&r, BKGRND);
	if (item.isItem()) {
		SharedTexture itemTex = item.getImage();
		Rect texRect = Rect::getMinRect(itemTex.get(),
			gameVals::INV_IMG_W(), gameVals::INV_IMG_W());
		texRect.setCenter(r.cX(), r.cY());
		assets.drawTexture(itemTex.get(), texRect, &mRect);
		td.text = std::to_string(item.amnt);
		td.x = r.x2(); td.y = r.y2();
		assets.drawText(td, &mRect);
	}
	UI::resetRenderTarget();
}

void Inventory::drawHoverItem(SDL_Point pos) {
	//std::cerr << "Hover Item: " << pos << std::endl;
}
