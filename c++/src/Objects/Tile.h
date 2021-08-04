#ifndef TILE_H
#define TILE_H

#define TILE_FILE "res/tiles.txt"

#include <utility>
#include <forward_list>
#include <map>
#include <typeindex>
#include <typeinfo>

#include <SDL.h>

#include "../Utils/Utils.h"
#include "ObjectIds.h"
#include "ObjectParser.h"

namespace object {
	const std::map<std::string, std::type_index> tileObjects;
	std::type_index getTileType(std::string name);
	Tile* getTile(TileId id);

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

	static Loader<Tile, TileId> tileLoader(TileId::numTiles,
		LoaderInfo{ TILE_FILE, "Tile", tileNames, tileObjects });

	class Tile {
	public:
		Tile() : Tile(TileId::AIR) {}
		Tile(TileId _id) : id(_id) {}
		~Tile() = default;

		void tick(int x, int y, Timestep dt);

		SDL_Texture* getImg(std::pair<int, int> pos) const;
		void setAnimation(Animation& anim);

		void addDrop(int item, int minAmnt, int maxAmnt = -1);
		std::forward_list<ItemInfo> generateDrops() const;

		void onBreak();
		void onPlace();

		bool canPlace() const;
		bool activated() const { return false; }

		bool hit(int x, int y, int power);

		void load(DataObject data);

		bool getTileData(TileData idx) const { return data[idx]; }
		void setTileData(TileData idx, bool val) { data[idx] = val; }

	private:
		// Tile index
		TileId id = TileId::AIR;
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
		// Light display
		SDL_Texture* light_d;
	};

	static Loader<Tile, TileId> tileLoader(TileId::numTiles,
		LoaderInfo{ TILE_FILE, "Tile", tileNames, tileObjects });
}

#endif
