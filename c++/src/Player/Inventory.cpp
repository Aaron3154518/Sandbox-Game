#include "Inventory.h"
#include "../GameObjects.h"

const SDL_Color Inventory::BKGRND{ 0, 255, 0, 150 };
const SDL_Color Inventory::SELECT_COLOR{ 175,175,0,255 };
const ItemInfo Inventory::NO_ITEM;

SDL_Point Inventory::toInvPos(SDL_Point pos) {
	return (pos - gameVals::INV_MARGIN()) / gameVals::INV_W();
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

	td.fontId = gameVals::INV_FONT();
	td.color = WHITE;
	td.yMode = td.xMode = TextData::PosType::botright;
	td.w = td.h = gameVals::INV_FONT_W();

	mRect = Rect(0, 0, dim.x * gameVals::INV_W() + 2 * gameVals::INV_MARGIN(),
		dim.y * gameVals::INV_W() + 2 * gameVals::INV_MARGIN());
	mTex.texture = UI::assets().createTexture(mRect.w, mRect.h);
}

void Inventory::draw(SDL_Point parentPos) {
	SDL_Point mouse = UI::mouse();
	mTex.dest = mRect + parentPos;
	UI::assets().drawTexture(mTex);
	if (SDL_PointInRect(&mouse, &mTex.dest)) {
		drawHoverItem(toInvPos(mouse - mRect.topLeft()));
	}
}

void Inventory::drawInventory() {
	UI::setRenderTarget(mTex.texture.get());
	UI::assets().rect(NULL, BKGRND, SDL_BLENDMODE_BLEND);
	UI::assets().thickRect(Rect(0, 0, mRect.w, mRect.h),
		gameVals::INV_MARGIN(), AssetManager::BorderType::inside, BLACK);
	UI::resetRenderTarget();
	SDL_Point loc;
	for (loc.y = 0; loc.y < dim.y; loc.y++) {
		for (loc.x = 0; loc.x < dim.x; loc.x++) {
			updateItem(loc);
			UI::setRenderTarget(mTex.texture.get());
			UI::assets().thickRect(getInvRect(loc), gameVals::INV_MARGIN(),
				AssetManager::BorderType::inside, BLACK);
			UI::resetRenderTarget();
		}
	}
}

void Inventory::selectPos(SDL_Point pos) {
	if (validPos(pos)) {
		UI::setRenderTarget(mTex.texture.get());
		UI::assets().thickRect(getInvRect(pos), 2 * gameVals::INV_MARGIN(),
			AssetManager::BorderType::middle, SELECT_COLOR);
		UI::resetRenderTarget();
	}
}

void Inventory::unselectPos(SDL_Point pos) {
	if (validPos(pos)) {
		UI::setRenderTarget(mTex.texture.get());
		UI::assets().thickRect(getInvRect(pos), 2 * gameVals::INV_MARGIN(),
			AssetManager::BorderType::middle, BLACK);
		UI::resetRenderTarget();
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
			if (x < dim.x && y < dim.y) { items[y][x].read(io); } 			else { tmp.read(io); }
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
	r += toPxPos(SDL_Point{ 0,0 });

	AssetManager& assets = UI::assets();
	UI::setRenderTarget(mTex.texture.get());
	assets.rect(&r, BKGRND);
	if (item.isItem()) {
		TextureData itemTex;
		itemTex.texture = item.getImage();
		itemTex.dest = Rect::getMinRect(itemTex.texture.get(),
			gameVals::INV_IMG_W(), gameVals::INV_IMG_W());
		itemTex.dest.setCenter(r.cX(), r.cY());
		itemTex.boundary = mRect;
		assets.drawTexture(itemTex);
		td.text = std::to_string(item.amnt);
		td.x = r.x2(); td.y = r.y2();
		assets.drawText(td, r);
	}
	UI::resetRenderTarget();
}

void Inventory::drawHoverItem(SDL_Point pos) {
	//std::cerr << "Hover Item: " << pos << std::endl;
}
