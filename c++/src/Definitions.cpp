#include "Definitions.h"

namespace gameVals {
	std::string concat(std::initializer_list<std::string> strs) {
		size_t size = 0;
		for (const std::string& str : strs) { size += str.length(); }
		std::string result;
		result.reserve(size);
		for (const std::string& str : strs) { result += str; }
		return result;
	}

	// File constants
	const std::string& res() {
		const static std::string RES = "res/";
		return RES;
	}
	const std::string& fonts() {
		const static std::string FONTS = res() + "fonts/";
		return FONTS;
	}
	const std::string& images() {
		const static std::string IMAGES = res() + "images/";
		return IMAGES;
	}
	const std::string& items() {
		const static std::string ITEMS = res() + "items/";
		return ITEMS;
	}
	const std::string& projectiles() {
		const static std::string PROJECTILES = res() + "projectiles/";
		return PROJECTILES;
	}
	const std::string& entities() {
		const static std::string ENTITY_IMGS = res() + "entities/";
		return ENTITY_IMGS;
	}

	const std::string& saves() {
		const static std::string SAVES = "saves/";
		return SAVES;
	}
	const std::string& players() {
		const static std::string PLAYERS = saves() + "players/";
		return PLAYERS;
	}
	const std::string& universes() {
		const static std::string UNIVERSES = saves() + "universes/";
		return UNIVERSES;
	}

	const std::string& playerExt() {
		const static std::string PLAYER_EXT = ".plr";
		return PLAYER_EXT;
	}
	const std::string& worldExt() {
		const static std::string WORLD_EXT = ".wld";
		return WORLD_EXT;
	}
	const std::string& infoFile() {
		const static std::string INFO_FILE = "info.bin";
		return INFO_FILE;
	}

	// File getters
	const std::string& fontFile() {
		const static std::string TIMES_FONT = fontFile("times");
		return TIMES_FONT;
	}
	std::string fontFile(const std::string& fName) {
		return concat({ fonts(), fName, ".ttf" });
	}
	std::string playerFile(const std::string& pName) {
		return concat({ players(), pName, playerExt() });
	}
	std::string univDir(const std::string& uName) {
		return concat({ universes(), uName, "/" });
	}
	std::string univInfoFile(const std::string& uName) {
		return concat({ universes(), uName, "/", infoFile() });
	}
	std::string worldFile(const std::string& uName, const std::string& wName) {
		return concat({ universes(), uName, "/", wName, worldExt() });
	}

	// Constants
	// FPS info
	int FPS() { return 60; }
	int FRAME_DELAY() {
		const static int _FRAME_DELAY = 1000 / FPS();
		return _FRAME_DELAY;
	}

	// Game constants
	const std::string& DEF_IMG() {
		const static std::string _DEF_IMG = res() + "default.png";
		return _DEF_IMG;
	}
	float FLOAT_ERR() { return 1e-5; }
	double DOUBLE_ERR() { return 1e-10; }
	int MIN_W() { return 800; }
	int MIN_H() { return 600; }
	int BLOCK_W() { return 32; }
	int MAP_W() {
		const static int _MAP_W = std::min(MIN_W() / 4, MIN_H() / 3);
		return _MAP_W;
	}
	int SPRITE_W() {
		const static int _SPRITE_W = MAP_W() / 15;
		return _SPRITE_W;
	}
	int ITEM_W() {
		const static int _ITEM_W = BLOCK_W() * 3 / 4;
		return _ITEM_W;
	}
	int INV_W() {
		const static int _INV_W = BLOCK_W();
		return _INV_W;
	}
	int INV_IMG_W() {
		const static int _INV_IMG_W = INV_W() * 7 / 8;
		return _INV_IMG_W;
	}
	int INV_FONT_W() {
		const static int _INV_FONT_W = INV_W() * 11 / 24;
		return _INV_FONT_W;
	}
	int INV_MARGIN() {
		const static int _INV_MARGIN = (INV_W() - INV_IMG_W()) / 3;
		return _INV_MARGIN;
	}

	const std::map<SDL_KeyCode, unsigned int>& KEY_NUMS() {
		const static std::map<SDL_KeyCode, unsigned int> _KEY_NUMS =
		{ {SDLK_0, 0}, {SDLK_1, 1}, {SDLK_2, 2}, {SDLK_3, 3},
			{SDLK_4, 4}, { SDLK_5, 5}, { SDLK_6, 6},
			{SDLK_7, 7}, {SDLK_8, 8}, {SDLK_9, 9} };
		return _KEY_NUMS;
	}
}