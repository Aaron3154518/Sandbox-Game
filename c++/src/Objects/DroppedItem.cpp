#include "DroppedItem.h"
#include "../GameObjects.h"

constexpr double X_SPEED = 15.;
constexpr double MAX_FALL_SPEED = 10.;

DroppedItem::DroppedItem(ItemInfo _info) : info(_info) {
	mRect = Rect::getMinRect(Item::getItem(info.itemId)->getImage().get(),
		gameVals::ITEM_W(), gameVals::ITEM_W());
	ratio.x = (double)mRect.w / gameVals::BLOCK_W();
	ratio.y = (double)mRect.h / gameVals::BLOCK_W();
}

void DroppedItem::move(Timestep& dt) {
	auto ms = dt.milliseconds();
	auto sec = dt.seconds();
	if (ms == 0) { return; }
	if (pickUpImmune()) { pickUpImmunity -= ms; }
	Point<double> d = {v.x * sec * gameVals::BLOCK_W(),
		v.y * sec * gameVals::BLOCK_W()};
	if (pulledIn) {
		pos = { pos.x + d.x, pos.y + d.y };
		Point<double> pxDim = { (double)mRect.w, (double)mRect.h };
		GameObjects::world().forceInWorld(pos, pxDim);
	} else {
		// Update vertical position and velocity
		v.x = copysignf(fmaxf(fabsf(v.x) - 1, 0), v.x);
		v.y += MAX_FALL_SPEED * sec / 2;
		v.y = fminf(v.y, MAX_FALL_SPEED / 2);
		GameObjects::world().checkCollisions(pos, ratio, d);
	}
	mRect.x = (int)pos.x; mRect.y = (int)pos.y;
}

void DroppedItem::drop(Point<double> p, DropDir dropDir) {
	pulledIn = false;
	mRect.setCenter((int)p.x, (int)p.y);
	pos = { (double)mRect.x, (double)mRect.y };
	v = { X_SPEED * dropDir, 0. };
	// TODO: are we sure it should ever be 0?
	pickUpImmunity = dropDir != DropDir::none ? 1500 : 0;
}

void DroppedItem::attract(Point<double> p) {
	// Calculate velocities
	Point<double> d{ p.x - mRect.cX(), p.y - mRect.cY() };
	v = { copysignf(4., d.x), copysignf(4., d.y) };
	pulledIn = true;
}
