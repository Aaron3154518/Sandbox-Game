#include "Item.h"

// Item Info
const ItemInfo& ItemInfo::NO_ITEM() {
	const static ItemInfo _NO_ITEM;
	return _NO_ITEM;
}

ItemInfo::ItemInfo(item::Id id, size_t _amnt) :
	itemId(id), amnt(_amnt), data(Item::getItem(id)->newItemData()) {}

void ItemInfo::update() {
	if (!isItem()) {
		amnt = 0;
		itemId = item::Id::numItems;
		data.clear();
	}
}

void ItemInfo::read(IO& io) {
	io.read(itemId);
	io.read(amnt);
	data.read(io);
	update();
}

void ItemInfo::write(IO& io) {
	update();
	io.write(itemId);
	io.write(amnt);
	data.write(io);
}

void ItemInfo::print() {
	std::cerr << "Item ID: " << itemId << ", Amnt: " << amnt
		<< "Data: " << (data.empty() ? "No" : "Yes") << std::endl;
}

bool ItemInfo::isItem() const {
	return amnt > 0 && itemId != item::Id::numItems;
}
ItemPtr ItemInfo::getItem() const {
	return Item::getItem(isItem() ? itemId : item::Id::numItems);
}
int ItemInfo::maxStack() const {
	return getItem()->maxStack;
}
int ItemInfo::maxStack(int mMaxStack) const {
	return std::min(maxStack(), mMaxStack);
}
int ItemInfo::useTime() const {
	return getItem()->useTime;
}
SharedTexture ItemInfo::getImage() const {
	return getItem()->getImage(data);
}
bool ItemInfo::operator[](Item::ItemData idx) const {
	return (*getItem())[idx];
}

bool ItemInfo::sameAs(const ItemInfo& other) const {
	return itemId == other.itemId && data == other.data;
}

bool ItemInfo::equals(const ItemInfo& other) const {
	return isItem() == other.isItem() && (!isItem() || *this == other);
}

bool ItemInfo::operator==(const ItemInfo& other) const {
	return itemId == other.itemId && amnt == other.amnt
		&& data == other.data;
}

// Item
std::vector<ItemPtr>& Item::getItems() {
	static std::vector<ItemPtr> items;
	if (items.empty()) {
		ItemPtr emptyHand = std::make_shared<Item>();
		emptyHand->setItemData(ItemData::autoUse | ItemData::breaker
			| ItemData::leftClick, true);
		items.resize(item::Id::numItems + 1, emptyHand);
	}
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
	AssetManager& assets = Window::Get().assets();
	if (animIdx == -1) {
		return assets.getAsset(gameVals::items() + img);
	} else {
		// TODO: Animation
	}
	return makeSharedTexture(NULL);
}

int Item::tick(int useTime, int dt) {
	return useTime - dt;
}

std::string Item::getFullDescription() {
	return getDescription();
}

Texture Item::drawDescription() {
	return makeTexture();
}

bool Item::getItemData(uint32_t bits) const {
	return (data & bits) == bits;
}
void Item::setItemData(uint32_t bits, bool val) {
	if (val) { data |= bits; } else { data &= !bits; }
}