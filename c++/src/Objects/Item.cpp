#include "Item.h"

namespace object {
	const std::map<std::string, std::type_index> itemObjects = {
			"Item" : std::type_index(Item)
	};

	std::type_index getItemType(std::string name) {
		auto it = itemObjects.find(name);
		return it != itemObjects.end() ? it->second : std::type_index(Item);
	}

	Item* getItem(TileId id) {
		return itemLoader.getObject(id);
	}

	// ItemInfo
	void ItemInfo::copy(const ItemInfo& other) {
		itemId = other.itemId;
		amnt = other.amnt;
		data = other.data;
	}

	void ItemInfo::print() {
		std::cerr << "Item: " << itemId << ", Amnt: " << amnt
			<< ", Data: " << data << std::endl;
	}

	bool ItemInfo::sameAs(const ItemInfo& other) {
		return itemId == other.itemId && data == other.data &&
			amnt > 0;
	}

	bool ItemInfo::operator==(const ItemInfo& other) {
		return itemId == other.itemId && amnt == other.amnt &&
			data == other.data;
	}
}