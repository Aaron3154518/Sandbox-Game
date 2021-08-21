#ifndef ITEM_OBJECTS_H
#define ITEM_OBJECTS_H

#include "Item.h"
#include "../ID/Tiles.h"
#include "../ID/Items.h"

namespace item {
	// @param Type - the type whose class is being declared
	// NEW_ITEM(Type)
	// - makes Item a friend of Type
	// - declares a constructor Type()
	// - implements a special id for the type

	class Dirt : public Item { NEW_ITEM(Dirt) };
	class Stone : public Item { NEW_ITEM(Stone) };
}

#endif