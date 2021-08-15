#include "PlayerSelector.h"
#include "UniverseSelector.h"

PlayerSelector::PlayerSelector() : Selector(true) {}

PlayerSelector::PlayerSelector(std::string univ) : PlayerSelector() {
	universe = univ;
}

void PlayerSelector::loadFiles() {
	files.clear();
	DIR* playerDir = opendir(PLAYERS);
	struct dirent* en;
	if (playerDir) {
		while ((en = readdir(playerDir)) != NULL) {
			std::string name = en->d_name;
			if (isFile(createFile(PLAYERS, name, ""))) {
				size_t idx = name.find_last_of(".");
				if (idx != std::string::npos
					&& validSaveFile(name.substr(0, idx))
					&& name.substr(idx) == PLAYER_EXT) {
					files.push_back(toDisplayName(name.substr(0, idx)));
				}
			}
		}
		closedir(playerDir);
	}
}

bool PlayerSelector::newItem() {
	std::string fileName = toFileName(input.getInput());
	if (fileName.empty()) { return false; }
	if (std::find(files.begin(), files.end(), fileName) != files.end()) {
		return false;
	}
	std::string fullFile = createFile(PLAYERS, fileName, PLAYER_EXT);
	if (!isFile(fullFile)) {
		std::ofstream file(fullFile);
		file << "Wassup World" << std::endl;
		file.close();
		files.push_back(input.getInput());
		input.clearInput();
		return true;
	}
	return false;
}

bool PlayerSelector::deleteItem(int idx) {
	if (idx < 0 || idx >= files.size()) { return false; }
	auto it = files.begin() + idx;
	std::string fullFile = createFile(PLAYERS, toFileName(*it), PLAYER_EXT);
	if (isFile(fullFile)) {
		std::remove(fullFile.c_str());
		files.erase(it);
		return true;
	}
	return false;
}

void PlayerSelector::selectItem(int idx) {
	if (idx < 0 || idx >= files.size()) { return; }
	if (universe.empty()) {
		nextUIs.push_back(std::make_shared<UniverseSelector>(files[idx]));
	} else {
		nextUIs.push_back(std::make_shared<Game>(files[idx], universe));
	}
	running = false;
}