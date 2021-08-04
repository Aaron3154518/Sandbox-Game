#ifndef TILE_OBJECTS_H
#define TILE_OBJECTS_H

#include "Tile.h"

#define NEW_TILE(ID, TYPE, CODE) \
	class TYPE : public Tile { \
		static Tile* TYPE ## __ = tileLoader.addTile(TileId::ID, new TYPE()); \
	public: \
		TYPE() : Tile(TileId::ID) {} \
		CODE \
	};

NEW_TILE(DRAGON_EGG, DragonEgg,
void onBreak() {
	//game_vars.spawn_entity(mobs.Dragon(),
		//[p + randint(10, 20) * BLOCK_W * c.random_sign() for p in game_vars.player_pos()])
}
)

NEW_TILE(PORTAL, Portal,
void summon();
void onPlace();
void tick(int x, int y, Timestep dt);
)

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