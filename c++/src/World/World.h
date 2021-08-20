#ifndef WORLD_H
#define WORLD_H

#include <iomanip>
#include <vector>
#include <fstream>
#include <cmath>

#include <SDL.h>

#include "../Definitions.h"
//#include "Chunk.h"
#include "../ID/Tiles.h"
#include "../Objects/Tile.h"
#include "../Objects/TileObjects.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/Point.h"
#include "../Utils/FileIO.h"
#include "../Utils/AssetManager.h"

struct Block {
	tile::Id id = tile::Id::AIR;
	uint8_t src = 0;
	bool spawner = false, crafter = false;
	ByteArray data;

	void setSrc(uint8_t dx, uint8_t dy) { src = (dx & 0x0F) | ((dy << 4) & 0xF0); }
	uint8_t dx() const { return src & 0x0F; }
	uint8_t dy() const { return (src >> 4) & 0x0F; }
};

class World {
public:
	World() = default;
	~World();

	void printInfo(bool printBlocks) const;

	// Visual functions
	SDL_Point getWorldMousePos(SDL_Point mouse, SDL_Point center,
		bool blocks = false) const;
	Rect getScreenRect(SDL_Point center) const;

	// Functions involving the world blocks
	void getBlockSrc(int& x, int& y) const;
	SDL_Point getBlockSrc(SDL_Point loc) const;
	bool checkCollisions(Point<double>& pos, const Point<double>& dim,
		Point<double>& d) const;
	bool touchingBlocks(const Point<double>& pos, const Point<double>& dim,
		bool x, bool topLeft) const;
	bool anySolidBlocks(int x1, int x2, int y1, int y2) const;

	bool isInWorld(SDL_Point loc) const;

	int surfaceH() const;
	int underground() const;
	SDL_Color skyColor() const;

	// Getters
	SDL_Point getDim() const { return dim; }
	SDL_Point getPixelDim() const {
		return { dim.x * gameVals::BLOCK_W, dim.y * gameVals::BLOCK_W };
	}
	int width() const { return dim.x * gameVals::BLOCK_W; }
	int height() const { return dim.y * gameVals::BLOCK_W; }

	const Block& getBlock(int x, int y) const;
	const Block& getBlock(SDL_Point loc) const;
	void setBlockData(int x, int y, ByteArray& data);
	void setBlockData(SDL_Point loc, ByteArray& data);

	bool saving() const { return nextSave <= 0; }

	enum WorldType : uint8_t {
		world = 0,
		idle
	};
	// Static functions
	static const Block& airBlock();
	static Block createBlock(tile::Id tileId, uint8_t dx = 0, uint8_t dy = 0);
	static void initializeBlock(Block& b);
	// Constants
	static constexpr int SEC_PER_DAY = 60 * 24;
	static constexpr int MS_PER_DAY = SEC_PER_DAY * 1000;
	static constexpr int NOON = (int)(MS_PER_DAY / 2);
	static constexpr int DAY = (int)(MS_PER_DAY / 4);
	static constexpr int NIGHT = DAY * 3;

	struct cmpPos {
		bool operator()(const SDL_Point& p1, const SDL_Point& p2) const {
			return (p1.y < p2.y) || (p1.y == p2.y && p1.x < p2.x);
		}
	};

protected:
	void tick(Timestep& dt);

	void setFile(std::string fName);
	void newWorld();

	void setBlock(int x, int y, const Block& b);
	void setBlock(SDL_Point loc, const Block& b);

	void saveWorld();

private:
	// Loads world
	void loadWorld();
	double loadWorld(double progress);
	void loadInfo();
	double loadBlocks(double progress, int numRows);
	double drawWorld(double progress);

	// Saves world
	double saveWorld(double progress);
	void saveInfo();
	double saveBlocks(double progress, int numRows);
	double saveMap(double progress);

	void applyBlockChanges();

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
	// Temporary data which stores world changes during saving
	std::map<SDL_Point, Block, cmpPos> blockChanges;

	// Auto save variables
	static constexpr int SAVE_DELAY = 10;
	double nextSave = SAVE_DELAY;
	double saveProgress = 0.;

	// File variables
	std::string fileName;
	FileRead fr;
	FileWrite fw;
};

class WorldAccess : public World {
	friend class Game;
public:
	WorldAccess() = default;
	~WorldAccess() = default;

	// Update world blocks
	bool placeBlock(SDL_Point loc, tile::Id tileId);
	bool breakBlock(SDL_Point loc);

private:
	void draw(SDL_Point center);
	void drawLight(const Rect& rect);
};

#endif