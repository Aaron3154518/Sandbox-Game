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
}