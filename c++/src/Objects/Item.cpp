#include "Item.h"

// Item Info
ItemInfo::ItemInfo(item::Id id, size_t _amnt) :
	itemId(id), amnt(_amnt), data(Item::getItem(id)->newItemData()) {}

void ItemInfo::read(IO& io) {
	io.read(itemId);
	io.read(amnt);
	data.read(io);
}

void ItemInfo::write(IO& io) const {
	io.write(itemId);
	io.write(amnt);
	data.write(io);
}

void ItemInfo::print() {
	std::cerr << "Item ID: " << itemId << ", Amnt: " << amnt
		<< "Data: " << (data.empty() ? "No" : "Yes") << std::endl;
}

bool ItemInfo::sameAs(const ItemInfo& other) const {
	return itemId == other.itemId && data == other.data && amnt > 0;
}

// Item
std::vector<ItemPtr>& Item::getItems() {
	static std::vector<ItemPtr> items(item::Id::numItems + 1);
	return items;
}

ItemPtr Item::getItem(item::Id id) {
	return getItems()[id];
}

item::Id Item::registerItem(ItemPtr t, item::Id id) {
	getItems()[id] = t;
	return id;
}

void Item::useAnim(double timeUsed, void* arm, bool left,
	std::pair<int, int> playerCenter, Rect& rect) {}

SharedTexture Item::getImage() const {
	AssetManager& assets = UI::assets();
	if (animIdx == -1) {
		return assets.getAsset(gameVals::items() + img);
	} else {
		// TODO: Animation
	}
	return makeSharedTexture(NULL);
}

void Item::tick() {}

std::string Item::getFullDescription() {
	return getDescription();
}

Texture Item::drawDescription() {
	return makeTexture();
}

std::map<Item::ItemData, bool> Item::getItemData(const DataKeys& keys) const {
	std::map<ItemData, bool> result;
	for (ItemData key : keys) { result[key] = data[key]; }
	return result;
}

void Item::setItemData(const DataKeys& keys, bool val) {
	for (ItemData key : keys) { data[key] = val; }
}