#ifndef TILE_H
#define TILE_H

#include <utility>
#include <vector>
#include <forward_list>
#include <memory>
#include <typeindex>
#include <typeinfo>

#include <SDL.h>

#include "../Utils/Utils.h"
#include "../ID/Tiles.h"

constexpr auto TILE_FILE = "res/tiles.txt";

class Tile;
typedef std::shared_ptr<Tile> TilePtr;

enum TileData {
	has_data = 0,	// Tile stores data
	clickable,		// Tile is interactable
	has_ui,			// Tile brings up a ui when clicked
	on_surface,		// Must be placed on a surface
	crafting,		// Player can craft at this item
	updates,		// Block updates every tick
	light,			// Emits light
	barrier,		// Obstructs player movement
	spawner,		// Tile spawns enemies

	numData
};

class Tile {
public:
	Tile() = default;
	~Tile() = default;

	void tick(int x, int y, Timestep dt);

	SDL_Texture* getImg(std::pair<int, int> pos) const;
	void setAnimation(Animation& anim);

	void addDrop(int item, int minAmnt, int maxAmnt = -1);
	//std::forward_list<ItemInfo> generateDrops() const;

	void onBreak();
	void onPlace();

	bool canPlace() const;
	bool activated() const { return false; }

	bool hit(int x, int y, int power);

	//void load(DataObject data);

	bool getTileData(TileData idx) const { return data[idx]; }
	void setTileData(TileData idx, bool val) { data[idx] = val; }

	virtual int id() { return Tile::ID; }
	static int Id() { return ID; }
	static TilePtr getTile(TileId id);

protected:
	static int registerTile(TilePtr t);

private:
	const static int ID = -1;
	static std::vector<TilePtr>& getTiles();

	// Dimensions
	std::pair<int, int> dim{ 0, 0 };

	// Image
	std::string image = "";

	// HP and hardness of block
	int hp = 0, hardness = 0;
	// All of the locations where the block has been damaged
	std::map<std::pair<int, int>, int> damage;

	// Tile data
	bool data[TileData::numData] = { false };

	// Minimap color
	SDL_Color map_color{ 0,0,0 };

	// Drops, item : amnt
	std::map<int, int> drops;
	// Recipies (if crafting block)
	//std::forward_list<Recipe> recipies;

	// Animation index
	int animIdx;

	// Light magnitude/radius
	int light_m = 0, light_r = 0;
};

#define NEW_TILE(TYPE) \
private: \
friend class Tile; \
TYPE(); \
const static int ID; \
public: \
int id() { return TYPE::ID; } \
static int Id() { return ID; }

#define ADD_TILE(TYPE) \
const int TYPE::ID = Tile::registerTile(std::shared_ptr<TYPE>(new TYPE()));

#endif
