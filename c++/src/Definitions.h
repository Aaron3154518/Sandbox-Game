#ifndef DEFINITIONS
#define DEFINITIONS
//#define DEBUG

#include <string>
#include <initializer_list>

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
	/*constexpr int FPS() = 60;
	constexpr int FRAME_DELAY() = 1000 / FPS();

	// Fonts
	constexpr auto SMALL_FONT() = "smallfont";
	constexpr auto LARGE_FONT() = "largefont";
	constexpr auto INV_FONT() = "invfont";

	// Game constants
	constexpr float FLOAT_ERR() = 1e-5;
	constexpr double DOUBLE_ERR() = 1e-10;
	constexpr int MIN_W() = 800, MIN_H() = 600;
	constexpr int BLOCK_W = 32;
	constexpr int MAP_W() = std::min(MIN_W() / 4, MIN_H() / 3);
	constexpr int SPRITE_W() = MAP_W() / 15;
	constexpr int ITEM_W() = BLOCK_W * 3 / 4;
	constexpr int INV_W() = BLOCK_W;
	constexpr int INV_IMG_W() = INV_W() * 7 / 8;
	constexpr int INV_FONT()_W = INV_W() / 3;
	constexpr int INV_MARGIN() = (INV_W() - INV_IMG_W()) / 2;*/

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
}

#endif /* DEFINITIONS */
