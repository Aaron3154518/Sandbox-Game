#ifndef TILE_H
#define TILE_H

#include <utility>
#include <list>
#include <vector>
#include <forward_list>
#include <memory>
#include <typeindex>
#include <typeinfo>

#include <SDL.h>

#include "../Definitions.h"
#include "../Utils/Utils.h"
#include "../ID/Tiles.h"
#include "../ID/Items.h"
#include "../UIs/UI.h"
#include "../Utils/AssetManager.h"

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

	SDL_Texture* getImage(SDL_Point pos) const;
	void setAnimation(Animation& anim);

	void addDrop(item::Id item, int minAmnt = 1, int maxAmnt = -1);
	//std::forward_list<ItemInfo> generateDrops() const;

	void onBreak();
	void onPlace();

	bool canPlace() const;
	bool activated() const { return false; }

	bool hit(int x, int y, int power);

	// Getters/Setters
	bool getTileData(TileData idx) const { return data[idx]; }
	void setTileData(TileData idx, bool val) { data[idx] = val; }
	SDL_Color getMapColor() const { return mapColor; }

	virtual int id() { return Tile::ID; }
	static int Id() { return ID; }
	static TilePtr getTile(tile::Id id);

protected:
	struct TileDrop {
		item::Id item;
		int minAmnt = 1;
		int maxAmnt = -1;
	};

	static tile::Id registerTile(TilePtr t, tile::Id id);

	// Dimensions
	SDL_Point dim{ 1, 1 };

	// Image
	std::string img = "";

	// HP and hardness of block
	int hp = 1, hardness = 0;
	// All of the locations where the block has been damaged
	std::map<SDL_Point, int> damage;

	// Tile data
	bool data[TileData::numData] = { false };

	// Minimap color
	SDL_Color mapColor{ 0,0,0 };

	// Drops, item : amnt
	std::list<TileDrop> drops;
	// Recipies (if crafting block)
	//std::forward_list<Recipe> recipies;

	// Animation index
	int animIdx = -1;

	// Light magnitude/radius
	int lightM = 0, lightR = 0;

private:
	const static tile::Id ID = tile::Id::numTiles;
	static std::vector<TilePtr>& getTiles();
};

#define NEW_TILE(TYPE) \
private: \
friend class Tile; \
TYPE(); \
const static tile::Id ID; \
public: \
int id() { return TYPE::ID; } \
static int Id() { return ID; }

#define ADD_TILE(TYPE, TILE_ID) \
const tile::Id TYPE::ID = \
	Tile::registerTile(std::shared_ptr<TYPE>(new TYPE()), tile::Id::TILE_ID); \
TYPE::TYPE()

#endif
