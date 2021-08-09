#ifndef TILE_OBJECTS_H
#define TILE_OBJECTS_H

#include "Tile.h"

class DragonEgg : public Tile {
	NEW_TILE(DragonEgg)
public:
	void onBreak();
};

class Portal : public Tile {
	NEW_TILE(Portal)
public:
	void summon();
	void onPlace();
	void tick(int x, int y, Timestep dt);
};

/*NEW_TILE(DIMENSION_HOPPER, DimensionHopper,
	void activate(std::pair<int, int> pos);
class UI : public ActiveUI {
public:
	UI(std::pair<int, int>) : ActiveUI(nullptr, nullptr, pos) {}
	void processEvents(events, mouse, keys);
	void on_resize();

private:
	WorldSelector selector;
};
)*/
#endif