#include "UniverseSelector.h"
#include "PlayerSelector.h"

UniverseSelector::UniverseSelector() {
	input.setHint("Enter Universe Name");
	allowNewItems(true);
	title = "Select a Universe";
}

UniverseSelector::UniverseSelector(std::string _player) : UniverseSelector() {
	player = _player;
}

bool UniverseSelector::exit() {
	if (!player.empty()) {
		nextUIs.push_back(std::make_shared<PlayerSelector>());
	}
	return true;
}

void UniverseSelector::loadFiles() {
	files.clear();
	for (const auto& file : getDirContents(gameVals::universes())) {
		if (isDir(gameVals::univDir(file)) && validSaveFile(file)) {
			files.push_back(toDisplayName(file));
		}
	}
}

bool UniverseSelector::newItem() {
	std::string fileName = input.getInput();
	if (fileName.empty()) { return false; }
	if (std::find(files.begin(), files.end(), fileName) != files.end()) {
		return false;
	}
	if (createUniverse(toFileName(fileName))) {
		files.push_back(fileName);
		return true;
	}
	return false;
}

bool UniverseSelector::deleteItem(int idx) {
	if (idx < 0 || idx >= files.size()) { return false; }
	auto it = files.begin() + idx;
	if (deleteUniverse(toFileName(*it))) {
		files.erase(it);
		return true;
	}
	return false;
}

void UniverseSelector::selectItem(int idx) {
	if (idx < 0 || idx >= files.size()) { return; }
	std::string file = toFileName(files[idx]);
	if (player.empty()) {
		nextUIs.push_back(std::make_shared<PlayerSelector>(file));
	} else {
		nextUIs.push_back(std::make_shared<Game>(player, file));
	}
	running = false;
}

bool UniverseSelector::createUniverse(std::string dirName) {
	std::string fullDir = gameVals::univDir(dirName);
	if (!isDir(fullDir)) {
		// Create folder
		mkDir(fullDir.c_str());
		World::createNewUniverse(dirName);
		return true;
	}
	return false;
}

bool UniverseSelector::deleteUniverse(std::string dirName) {
	std::string fullDir = gameVals::univDir(dirName);
	DIR* univDir = opendir(fullDir.c_str());
	struct dirent* en;
	if (univDir) {
		// Delete all .wld files
		while ((en = readdir(univDir)) != NULL) {
			std::string name = en->d_name;
			std::string fullFile = fullDir + name;
			if (isFile(fullFile)) {
				size_t idx = name.find_last_of(".");
				if (idx != std::string::npos
					&& name.substr(idx) == gameVals::worldExt()) {
					std::remove(fullFile.c_str());
				}
			}
		}
		// Delete info file
		std::remove(gameVals::univInfoFile(dirName).c_str());
		closedir(univDir);
		// TODO: Delete backups
		if (rmDir(fullDir.c_str()) != 0) {
			std::cerr << "This universe foldler has been modified. "
				<< "Cannot delete" << std::endl;
			return false;
		}
		return true;
	}
	return false;
}