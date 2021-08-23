#ifndef DEFINITIONS
#define DEFINITIONS
//#define DEBUG

#include <initializer_list>
#include <iostream>
#include <map>
#include <string>

#include <SDL.h>

namespace gameVals {
	std::string concat(std::initializer_list<std::string> strs);

	const std::string& res();
	const std::string& fonts();
	const std::string& images();
	const std::string& items();
	const std::string& projectiles();
	const std::string& entities();

	const std::string& saves();
	const std::string& players();
	const std::string& universes();

	const std::string& playerExt();
	const std::string& worldExt();
	const std::string& infoFile();

	// File getters
	const std::string& fontFile(); // Easy access to times new roman
	std::string fontFile(const std::string& fName);
	std::string playerFile(const std::string& pName);
	std::string univDir(const std::string& uName);
	std::string univInfoFile(const std::string& uName);
	std::string worldFile(const std::string& uName, const std::string& wName);

	// FPS info
	int FPS();
	int FRAME_DELAY();

	// Fonts
	const char* SMALL_FONT();
	const char* LARGE_FONT();
	const char* INV_FONT();

	// Game constants
	float FLOAT_ERR();
	double DOUBLE_ERR();
	int MIN_W();
	int MIN_H();
	int BLOCK_W();
	int MAP_W();
	int SPRITE_W();
	int ITEM_W();
	int INV_W();
	int INV_IMG_W();
	int INV_FONT_W();
	int INV_MARGIN();

	// Key codes -> number values
	const std::map<SDL_KeyCode, unsigned int>& KEY_NUMS();
}

#endif /* DEFINITIONS */
