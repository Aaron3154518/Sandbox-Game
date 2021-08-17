#ifndef DEFINITIONS
#define DEFINITIONS
//#define DEBUG

#include <map>
#include <algorithm>

// Frequently used types
template <typename T>
using Map2D = std::map<int, std::map<int, T>>;

// Folders
// Resources
#define RES "res/"
#define FONTS RES "fonts/"
#define IMAGES RES "images/"
#define ITEM_IMGS RES "items/"
#define PROJ_IMGS RES "projectiles/"
#define PLAYER_IMGS RES "player/"
#define ENTITY_IMGS RES "enitites/"

#define TIMES_FONT FONTS "times.ttf"

// Saves
#define SAVES "saves/"
#define PLAYERS SAVES "players/"
#define UNIVERSES SAVES "universes/"

// File Extensions
constexpr auto WORLD_EXT = ".wld";
constexpr auto PLAYER_EXT = ".plr";
constexpr auto TMP_EXT = ".tmp";

// Special Files
constexpr auto INFO_FILE = "info.bin";

namespace gameVals {
	// FPS info
	constexpr int FPS = 60;
	constexpr int FRAME_DELAY = 1000 / FPS;

	// Fonts
	constexpr auto SMALL_FONT = "smallfont";
	constexpr auto LARGE_FONT = "largefont";

	// Game constants
	constexpr int MIN_W = 800, MIN_H = 600, BLOCK_W = 24;
	constexpr int MAP_W = (int)std::min(MIN_W / 4, MIN_H / 3);
	constexpr int ITEM_W = (int)(BLOCK_W * 3 / 4);
	constexpr int INV_W = (int)std::min(MIN_W / 20, MIN_H / 20);
	constexpr int INV_IMG_W = (int)(INV_W * 4 / 5);
}

#endif /* DEFINITIONS */
