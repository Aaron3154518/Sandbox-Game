#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <fstream>

#include <SDL.h>

#include "../Definitions.h"
//#include "Chunk.h"
#include "../Objects/ObjectIDs.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"

class World {
public:
	World() = default;
	~World();

	setFile(const std::string& fName);

	void tick(Timestep& dt);

	// Resets this to a new world
	void newWorld(const SDL_Point& newDim);

	// Loads world
	double loadWorld(double progress);
	void loadInfo(bool closeFile);
	double loadBlocks(double progress);
	double drawWorld(double progress);

	// Saves world
	double saveWorld(double progress);
	void saveInfo();
	double saveBlocks(double progress, int numRows);
	double saveMap(double progress);

	// Update world blocks
	void placeBlock(int x, int y, object::Tile::TileId block);
	void destroyBlock(int x, int y);
	// Update world block information
	void addBlock(int x, int y, object::Tile::TileId block);
	void removeBlock(int x, int y, object::Tile::TileId block);

	// Visual functions
	Rect getScreenRect(const SDL_Point& playerPos);
	void drawLight(const Rect& rect);

	// Getters
	int surfaceH() const;
	int underground() const;
	SDL_Color skyColor() const;

	enum WorldType : uint8_t {
		world = 0,
		idle
	};
	// Constants
	static const int SEC_PER_DAY, MS_PER_DAY, NOON, DAY, NIGHT;
private:
	// map
	// light

	// World type
	WorldType type = WorldType::world;
	// Can delete this world
	bool canDelete = false;
	// World time
	uint32_t time = 0;
	// World spawn location
	SDL_Point spawn{ 0,0 };
	// World dimensions and number of blocks
	SDL_Point dim{ 0,0 };
	int numBlocks = 0;
	// 2D array of blocks
	std::vector<std::vector<object::Tile::TileId>> blocks;
	// Special blocks
	// block data
	Map2D<object::Tile::TileId> spawners, crafters;
	// File variables
	std::string fileName = "";
	std::fstream file;
	// Auto save variables
	double nextSave = 30.;
	double saveProgress = 0.;
	// Chunks
//	ChunkManager manager;
};

#endif