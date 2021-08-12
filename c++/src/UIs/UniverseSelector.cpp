#include "UniverseSelector.h"
#include "PlayerSelector.h"

UniverseSelector::UniverseSelector() : Selector(true) {}

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
	if (createUniverse(fileName)) {
		files.push_back(input.getInput());
		input.clearInput();
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
	if (player.empty()) {
		nextUIs.push_back(std::make_shared<UniverseSelector>(files[idx]));
	} else {
		nextUIs.push_back(std::make_shared<Game>(player, files[idx]));
	}
	running = false;
}

bool UniverseSelector::createUniverse(std::string dirName) {
	std::string fullDir = createFile(UNIVERSES, dirName, "/");
	if (!isDir(fullDir)) {
		// Create folder
		mkdir(fullDir.c_str());
		std::cerr << "Info File: " << createFile(fullDir, INFO_FILE, "") << std::endl;
		// Create info file
		std::fstream file(createFile(fullDir, INFO_FILE, "").c_str(),
			std::ios_base::out | std::ios_base::binary);
		CHECK_FILE_OPEN(file, return false;)
		file.close();
		CHECK_FILE_IO(file, return false;)
		return true;
	}
	return false;
}

bool UniverseSelector::deleteUniverse(std::string dirName) {
	std::string fullDir = createFile(UNIVERSES, dirName, "/");
	DIR* univDir = opendir(fullDir.c_str());
	struct dirent* en;
	if (univDir) {
		// Delete all .wld files
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
		// Delete info file
		std::remove(createFile(fullDir, INFO_FILE, "").c_str());
		closedir(univDir);
		// TODO: Delete backups
		if (rmdir(fullDir.c_str()) != 0) {
			std::cerr << "This universe foldler has been modified. "
				<< "Cannot delete" << std::endl;
			return false;
		}
		return true;
	}
	return false;
}