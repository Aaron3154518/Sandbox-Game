#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <fstream>
#include <cmath>

#include <SDL.h>

#include "../Definitions.h"
#include "../GameObjects.h"
//#include "Chunk.h"
#include "../ID/Tiles.h"
#include "../Objects/Tile.h"
#include "../Objects/TileObjects.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/Point.h"
#include "../Utils/FileIO.h"
#include "../Utils/AssetManager.h"

class World {
	friend class Game;
public:
	struct Block {
		tile::Id id = tile::Id::AIR;
		uint8_t src = 0;
		bool spawner = false, crafter = false;

		void setSrc(uint8_t dx, uint8_t dy) { src = (dx & 0x0F) | ((dy << 4) & 0xF0); }
		uint8_t dx() const { return src & 0x0F; }
		uint8_t dy() const { return (src >> 4) & 0x0F; }
	};
	const static Block nullBlock;

	World() = default;
	~World();
	World(const World&) = delete;
	World operator =(const World&) = delete;

	void printInfo(bool printBlocks) const;

	// Update world blocks
	bool placeBlock(SDL_Point loc, tile::Id tileId);
	bool breakBlock(SDL_Point loc);
	void addBlock(SDL_Point loc, tile::Id tileId);
	void removeBlock(SDL_Point loc, tile::Id tileId);

	// Functions involving the world blocks
	void getBlockSrc(int& x, int& y) const;
	SDL_Point getBlockSrc(SDL_Point loc) const;
	bool checkCollisions(Point<double>& pos, const Point<double>& dim,
		Point<double>& d) const;
	bool touchingBlocks(const Point<double>& pos, const Point<double>& dim,
		bool x, bool topLeft) const;
	bool anySolidBlocks(int x1, int x2, int y1, int y2) const;

	// Visual functions
	SDL_Point getWorldMousePos(SDL_Point mouse, SDL_Point center,
		bool blocks = false) const;
	Rect getScreenRect(SDL_Point center) const;
	int width() const { return dim.x * gameVals::BLOCK_W; }
	int height() const { return dim.y * gameVals::BLOCK_W; }

	// Getters
	SDL_Point getDim() const { return dim; }
	SDL_Point getPixelDim() const {
		return { dim.x * gameVals::BLOCK_W, dim.y * gameVals::BLOCK_W };
	}
	const Block& getBlock(int x, int y) const;
	const Block& getBlock(SDL_Point loc) const { return getBlock(loc.x, loc.y); }
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
	void setFile(std::string fName);
	void newWorld();

	void tick(Timestep& dt);
	void draw(SDL_Point center);
	void drawLight(const Rect& rect);

	// Loads world
	void loadWorld();
	double loadWorld(double progress);
	void loadInfo();
	double loadBlocks(double progress, int numRows);
	double drawWorld(double progress);

	// Saves world
	void saveWorld();
	double saveWorld(double progress);
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