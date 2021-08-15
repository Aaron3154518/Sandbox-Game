#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <fstream>

#include <SDL.h>

#include "../Definitions.h"
//#include "Chunk.h"
#include "../ID/Tiles.h"
#include "../Objects/Tile.h"
#include "../Objects/TileObjects.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/FileIO.h"

class World {
public:
	World() = default;
	~World();

	void setFile(std::string fName);
	void newWorld();
	void printInfo(bool printBlocks) const;

	void tick(Timestep& dt);

	// Loads world
	void loadWorld();
	double loadWorld(double progress);

	// Saves world
	void saveWorld();
	double saveWorld(double progress);

	// Update world blocks
	void placeBlock(int x, int y, TileId block);
	void destroyBlock(int x, int y);
	// Update world block information
	void addBlock(int x, int y, TileId block);
	void removeBlock(int x, int y, TileId block);

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

	struct Block {
		TileId id = TileId::AIR;
		uint8_t src = 0;
		bool spawner = false, crafter = false;

		void setSrc(uint8_t dx, uint8_t dy) { src = (dx & 0x0F) | ((dy << 4) & 0xF0); }
		uint8_t xSrc() const { return src & 0x0F; }
		uint8_t ySrc() const { return (src >> 4) & 0x0F; }
	};
private:
	void loadInfo();
	double loadBlocks(double progress, int numRows);
	double drawWorld(double progress);

	void saveInfo();
	double saveBlocks(double progress, int numRows);
	double saveMap(double progress);

	// World type
	WorldType type = WorldType::world;
	// Can delete this world
	bool canDelete = false;
	// World dimensions and number of blocks
	SDL_Point dim{ 0,0 };
	int numBlocks = 0;
	// World spawn location
	SDL_Point spawn = { 0,0 };

	// World time
	uint32_t time = 0;
	// 2D array of blocks
	std::vector<std::vector<Block>> blocks;
	// Map <x, y> -> data
	std::map<SDL_Point, ByteArray> blockData;

	// map
	// light

	// File variables
	std::string fileName;
	FileRead fr;
	FileWrite fw;

	// Temporary data which stores world changes during saving
	std::list<std::pair<SDL_Point, Block>> blockChanges;
	std::map<SDL_Point, ByteArray> dataChanges;
	// Auto save variables
	double nextSave = 30.;
	double saveProgress = 0.;
	// Chunks
	//	ChunkManager manager;
};

#endif