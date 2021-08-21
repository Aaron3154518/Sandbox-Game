#ifndef DROPPED_ITEM_H
#define DROPPED_ITEM_H

#include <cmath>

#include <SDL.h>

#include "../Definitions.h"
#include "Item.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/Point.h"

class DroppedItem {
public:
	DroppedItem() : DroppedItem(ItemInfo()) {}
	DroppedItem(ItemInfo _info);
	~DroppedItem() = default;

	enum DropDir : int8_t { left = -1, none, right };

	void move(Timestep& dt);
	void drop(Point<double> p, DropDir dropDir);
	void attract(Point<double> p);

	const ItemInfo& getInfo() const { return info; }
	Rect getRect() const { return mRect; }
	bool pickUpImmune() const { return pickUpImmunity > 0; }
	bool beingPulled() const { return pulledIn; }
	void setPulled(bool val) { pulledIn = val; }

private:
	ItemInfo info;

	Rect mRect;
	// Used to check collisions with blocks
	Point<double> ratio;
	// Physics variables
	Point<double> pos, v;
	// Variables for picking up an item
	int pickUpImmunity = 0;
	bool pulledIn = false;
};

#endif