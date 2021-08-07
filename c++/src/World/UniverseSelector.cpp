#include "UniverseSelector.h"
#include "PlayerSelector.h"

UniverseSelector::UniverseSelector() {
	setAllowNewItems(true);
}

UniverseSelector::UniverseSelector(std::string _player) : UniverseSelector() {
	player = _player;
}

void UniverseSelector::loadFiles() {
	files.clear();
	DIR* univDir = opendir(UNIVERSES);
	struct dirent* en;
	if (univDir) {
		while ((en = readdir(univDir)) != NULL) {
			std::string name = en->d_name;
			if (isDir(createFile(UNIVERSES, name, "")) && validSaveFile(name)) {
				files.push_back(toDisplayName(name));
			}
		}
		closedir(univDir);
	}
}

bool UniverseSelector::newItem() {
	std::string fileName = toFileName(input.getInput());
	if (fileName.empty()) { return false; }
	std::string fullFile = createFile(UNIVERSES, fileName, "");
	if (!isDir(fullFile)) {
		mkdir(fullFile.c_str());
		files.push_back(input.getInput());
		input.clearInput();
		return true;
	}
	return false;
}

bool UniverseSelector::deleteItem(int idx) {
	if (idx < 0 || idx >= files.size()) { return false; }
	auto it = files.begin() + idx;
	std::string fullDir = createFile(UNIVERSES, toFileName(*it), "");
	DIR* univDir = opendir(fullDir.c_str());
	struct dirent* en;
	if (univDir) {
		while ((en = readdir(univDir)) != NULL) {
			std::string name = en->d_name;
			std::string fullFile = createFile(fullDir, name, "");
			if (isFile(fullFile)) {
				size_t idx = name.find_last_of(".");
				if (idx != std::string::npos
					&& name.substr(idx) == WORLD_EXT) {
					std::remove(fullFile.c_str());
				}
			}
		}
		closedir(univDir);
		if (rmdir(fullDir.c_str()) != 0) {
			std::cerr << "This universe foldler has been modified."
				<< "Cannot delete" << std::endl;
		} else {
			files.erase(it);
		}
		return true;
	}
	return false;
}

void UniverseSelector::selectItem(int idx) {
	if (idx < 0 || idx >= files.size()) { return; }
	if (player.empty()) {
		nextUIs.push_back(std::make_shared<UniverseSelector>(files[idx]));
	} else {
		nextUIs.push_back(std::make_shared<Game>(player, files[idx]));
	}
	running = false;
}