#include "PlayerSelector.h"
#include "UniverseSelector.h"

PlayerSelector::PlayerSelector() : Selector(true) {
	input.setHint("Enter Player Name");
}

PlayerSelector::PlayerSelector(std::string univ) : PlayerSelector() {
	universe = univ;
}

void PlayerSelector::loadFiles() {
	files.clear();
	for (const auto& file : getDirContents(gameVals::players().c_str())) {
		if (isFile(gameVals::players() + file)) {
			size_t idx = file.find_last_of(".");
			if (idx != std::string::npos && validSaveFile(file.substr(0, idx))
				&& file.substr(idx) == gameVals::playerExt()) {
				files.push_back(toDisplayName(file.substr(0, idx)));
			}
		}
	}
}

bool PlayerSelector::newItem() {
	std::string fileName = input.getInput();
	if (fileName.empty()) { return false; }
	if (std::find(files.begin(), files.end(), fileName) != files.end()) {
		return false;
	}
	std::string fullFile = gameVals::playerFile(toFileName(fileName));
	if (!isFile(fullFile)) {
		FileWrite fw;
		if (!fw.open(fullFile)) {
			std::cerr << "Could not open player file" << std::endl;
			return false;
		}
		Player::saveNewPlayer(fw);
		if (!fw.commit()) {
			std::cerr << "Failed to write to player file" << std::endl;
			return false;
		}
		files.push_back(fileName);
		return true;
	}
	return false;
}

bool PlayerSelector::deleteItem(int idx) {
	if (idx < 0 || idx >= files.size()) { return false; }
	auto it = files.begin() + idx;
	std::string fullFile = gameVals::playerFile(toFileName(*it));
	if (isFile(fullFile)) {
		std::remove(fullFile.c_str());
		files.erase(it);
		return true;
	}
	return false;
}

void PlayerSelector::selectItem(int idx) {
	if (idx < 0 || idx >= files.size()) { return; }
	std::string file = toFileName(files[idx]);
	if (universe.empty()) {
		subUIs.push_back(std::make_shared<UniverseSelector>(file));
	} else {
		nextUIs.push_back(std::make_shared<Game>(file, universe));
	}
	running = false;
}