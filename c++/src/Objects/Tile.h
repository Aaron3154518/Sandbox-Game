#ifndef TILE_H
#define TILE_H

#include <utility>
#include <list>
#include <vector>
#include <forward_list>
#include <map>
#include <memory>
#include <typeindex>
#include <typeinfo>

#include <SDL.h>

#include "DroppedItem.h"
#include "Item.h"
#include "../Definitions.h"
#include "../ID/Items.h"
#include "../ID/Tiles.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/Utils.h"
#include "../Utils/Point.h"
#include "../Window.h"

class Tile;
typedef std::shared_ptr<Tile> TilePtr;

class Tile {
public:
	Tile() = default;
	~Tile() = default;

	// TODO: bitwise
	enum TileData {
		hasData = 0,	// Tile stores data
		clickable,		// Tile is interactable
		hasUi,			// Tile brings up a ui when clicked
		onSurface,		// Must be placed on a surface
		crafting,		// Player can craft at this item
		updates,		// Block updates every tick
		light,			// Emits light
		barrier,		// Obstructs player movement
		spawner,		// Tile spawns enemies

		numData
	};

	virtual void tick(int x, int y, Timestep dt);

	virtual SharedTexture getImage(SDL_Point pos) const;
	virtual void setAnimation(Animation& anim);

	virtual void addDrop(item::Id item, int minAmnt = 1, int maxAmnt = -1);
	std::forward_list<ItemInfo> generateDrops() const;

	virtual bool onBreak(SDL_Point loc);
	virtual bool onPlace(SDL_Point loc);

	virtual bool canPlace(SDL_Point loc) const { return true; }
	virtual bool activated() const { return false; }

	virtual bool hit(SDL_Point loc, int power);

	// Getters/Setters
	typedef std::initializer_list<TileData> DataKeys;
	bool getTileData(TileData idx) const { return data[idx]; }
	std::map<TileData, bool> getTileData(const DataKeys& keys) const;
	void setTileData(TileData idx, bool val) { data[idx] = val; }
	void setTileData(const DataKeys& keys, bool val);
	SDL_Color getMapColor() const { return mapColor; }
	Point<uint8_t> getDim() const { return dim; }

	virtual tile::Id id() { return Tile::ID; }
	static tile::Id Id() { return ID; }
	static TilePtr getTile(tile::Id id);
	static bool isCrafter(tile::Id id);

protected:
	struct TileDrop {
		item::Id item;
		int minAmnt = 1;
		int maxAmnt = -1;
	};

	static tile::Id registerTile(TilePtr t, tile::Id id);

	// Dimensions
	Point<uint8_t> dim{ 1, 1 };

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

struct Recipe {
	Recipe(item::Id id, int amnt,
		std::initializer_list<ItemInfo> iList = {});
	Recipe(const Recipe& other);

	bool hasResult() const;

	const ItemInfo& getResult() const;
	const std::multimap<int, ItemInfo>& getIngredients() const;

private:
	ItemInfo result;
	std::multimap<int, ItemInfo> ingredients;
};

typedef std::shared_ptr<const Recipe> RecipePtr;

class CraftingTile : public Tile {
public:
	CraftingTile();
	~CraftingTile() = default;

	const std::multimap<int, RecipePtr>& getRecipes() const;

protected:
	// Makes recipes easier to hardcode
	using I = item::Id;

	void addRecipe(const Recipe& r);
	void addRecipes(std::initializer_list<Recipe> list);

private:
	std::multimap<int, RecipePtr> recipes;
};

#define NEW_TILE(TYPE) \
private: \
friend class Tile; \
TYPE(); \
const static tile::Id ID; \
public: \
tile::Id id() { return TYPE::ID; } \
static tile::Id Id() { return ID; }

#define ADD_TILE(TYPE, TILE_ID) \
const tile::Id TYPE::ID = \
	Tile::registerTile(std::shared_ptr<TYPE>(new TYPE()), tile::Id::TILE_ID); \
TYPE::TYPE()

#endif
