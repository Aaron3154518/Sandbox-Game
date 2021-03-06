#ifndef WORLD_H
#define WORLD_H

#include <cmath>
#include <fstream>
#include <iomanip>
#include <set>
#include <vector>

#include <SDL.h>

#include "../Definitions.h"
#include "../ID/Tiles.h"
#include "../Objects/DroppedItem.h"
#include "../Objects/Tile.h"
#include "../Objects/TileObjects.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/FileIO.h"
#include "../Utils/Point.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"
#include "../Window.h"

struct Block {
	tile::Id id = tile::Id::AIR;
	uint8_t src = 0;
	// TODO: don't copy this
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
	SDL_Point getWorldMousePos(SDL_Point mouse,	bool blocks = false) const;
	Rect getScreenRect(SDL_Point center) const;

	// Functions involving the world blocks
	void getBlockSrc(int& x, int& y) const;
	SDL_Point getBlockSrc(SDL_Point loc) const;
	void checkCollisions(Point<int> size,
		Point<double>& pos, Point<double>& d) const;
	bool touchingBlocks(const Point<double>& pos, const Point<double>& dim,
		bool x, bool topLeft) const;
	bool anySolidBlocks(int x1, int x2, int y1, int y2) const;

	// Units are pixels
	void forceInWorld(Point<double>& p, Point<double> _dim) const;
	// Units are blocks
	void forceInWorld(SDL_Point& p, Point<double> _dim) const;
	bool isInWorld(SDL_Point loc) const;

	int surfaceH() const;
	int underground() const;
	SDL_Color skyColor() const;

	// Getters
	SDL_Point getDim() const { return dim; }
	SDL_Point getPixelDim() const {
		return { dim.x * gameVals::BLOCK_W(), dim.y * gameVals::BLOCK_W() };
	}
	int width() const { return dim.x * gameVals::BLOCK_W(); }
	int height() const { return dim.y * gameVals::BLOCK_W(); }

	const Block& getBlock(int x, int y) const;
	const Block& getBlock(SDL_Point loc) const;
	void setBlockData(int x, int y, ByteArray& data);
	void setBlockData(SDL_Point loc, ByteArray& data);

	std::multimap<int, std::pair<SDL_Point, tile::Id>>
		getCraftingBlocks(Rect area);

	void dropItem(const DroppedItem& drop, DroppedItem::DropDir dir);
	void dropItem(const DroppedItem& drop, DroppedItem::DropDir dir,
		Point<double> pos);

	void read(IO& io);
	void write(IO& io);
	bool saving() const { return nextSave <= 0; }

	enum WorldType : uint8_t {
		world = 0,
		idle
	};

	// Static functions
	static void createNewInfoFile(IO& io);
	static void createNewWorld(IO& io);
	static bool createNewUniverse(const std::string& univName);
	static const Block& airBlock();
	static Block createBlock(tile::Id tileId, uint8_t dx = 0, uint8_t dy = 0);
	static SDL_Point toBlockPos(SDL_Point p);
	static Rect toBlockRect(Rect r);
	// Constants
	static constexpr int SEC_PER_DAY = 60 * 24;
	static constexpr int MS_PER_DAY = SEC_PER_DAY * 1000;
	static constexpr int NOON = (int)(MS_PER_DAY / 2);
	static constexpr int DAY = (int)(MS_PER_DAY / 4);
	static constexpr int NIGHT = DAY * 3;

protected:
	void tick(Timestep& dt);

	void setFile(std::string fName);
	void newWorld();

	void setBlock(int x, int y, const Block& b);
	void setBlock(SDL_Point loc, const Block& b);

	void saveWorld();

	void drawDroppedItems(const Rect& worldRect);

private:
	void reset();

	// Loads world
	void loadWorld();
	double loadWorld(double progress);
	void loadInfo(IO& io);
	double loadBlocks(IO& io, double progress, int numRows);
	double loadMap(IO& io, double progress);

	// Saves world
	double saveWorld(double progress);
	void saveInfo(IO& io);
	double saveBlocks(IO& io, double progress, int numRows);
	double saveMap(IO& io, double progress);

	void updateBlock(int c, int r);
	void updateBlock(SDL_Point loc);
	void applyBlockChanges();

	// World type
	WorldType type = WorldType::world;
	// Can delete this world
	bool canDelete = false;
	// World dimensions and number of blocks
	SDL_Point dim{ 0,0 };
	int numBlocks = 0;
	// World spawn location
	SDL_Point spawn{ 0,0 };

	// World time
	uint32_t time = 0;

	// TODO: move to utils
	struct CmpPoint {
		bool operator()(const SDL_Point& lhs, const SDL_Point& rhs) const;
	};

	// Sparse array for special block types
	std::map<int, std::set<int>> craftingBlocks;
	// 2D array of blocks
	std::vector<std::vector<Block>> blocks;
	// Temporary data which stores world changes during saving
	std::map<SDL_Point, Block, CmpPoint> blockChanges;

	std::list<DroppedItem> droppedItems;

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