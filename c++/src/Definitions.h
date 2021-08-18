#ifndef DEFINITIONS
#define DEFINITIONS
//#define DEBUG

#include <string>
#include <initializer_list>

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
	constexpr int FPS = 60;
	constexpr int FRAME_DELAY = 1000 / FPS;

	// Fonts
	constexpr auto SMALL_FONT = "smallfont";
	constexpr auto LARGE_FONT = "largefont";

	// Game constants
	constexpr int MIN_W = 800, MIN_H = 600, BLOCK_W = 32;
	constexpr int MAP_W = (int)std::min(MIN_W / 4, MIN_H / 3);
	constexpr int ITEM_W = (int)(BLOCK_W * 3 / 4);
	constexpr int INV_W = (int)std::min(MIN_W / 20, MIN_H / 20);
	constexpr int INV_IMG_W = (int)(INV_W * 4 / 5);
}

#endif /* DEFINITIONS */
