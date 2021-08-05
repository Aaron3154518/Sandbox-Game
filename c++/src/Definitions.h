#ifndef DEFINITIONS
#define DEFINITIONS
//#define DEBUG

#include <algorithm>

// For generating file paths
#define STR(str) #str
// Resources
#define RES(str) STR(res/str)
#define FONT(str) RES(fonts/str)
#define IMG(str) RES(images/str)
#define ITEM_IMG(str) RES(items/str)
#define PROJ_IMG(str) RES(projectiles/str)
#define PLAYER_IMG(str) RES(player/str)
#define ENTITY_IMG(str) RES(entities/str)
// Saves
#define SAVE(str) STR(saves/str)
#define PLAYER(str) SAVE(players/str)
#define UNIVERSE(str) SAVE(universes/str)

namespace gameVals {
	// FPS info
	constexpr int FPS = 60;
	constexpr int FRAME_DELAY = 1000 / FPS;

	// Fonts
	constexpr auto SMALL_FONT = "smallfont";
	constexpr auto LARGE_FONT = "largefont";

	// Game constants
	constexpr int MIN_W = 800, MIN_H = 600, BLOCK_W = 16;
	constexpr int MAP_W = (int)std::min(MIN_W / 4, MIN_H / 3);
	constexpr int ITEM_W = (int)(BLOCK_W * 3 / 4);
	constexpr int INV_W = (int)std::min(MIN_W / 20, MIN_H / 20);
	constexpr int INV_IMG_W = (int)(INV_W * 4 / 5);
}

#endif /* DEFINITIONS */
