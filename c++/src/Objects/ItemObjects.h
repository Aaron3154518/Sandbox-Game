#ifndef ITEM_OBJECTS_H
#define ITEM_OBJECTS_H

#include "Item.h"

#define NEW_TILE(ID, TYPE, CODE) \
	class TYPE : public Item { \
		static Item* TYPE ## __ = itemLoader,addTile(ItemId::ID, new TYPE()); \
	public: \
		TYPE() : Item(ItemId::ID) {} \
		CODE \
	};

#endif