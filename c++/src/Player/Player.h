#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

#include "../Definitions.h"
#include "PlayerInventory.h"
#include "../UIs/UI.h"
#include "../Utils/Rect.h"
#include "../Utils/Point.h"
#include "../Utils/AssetManager.h"
#include "../ID/Tiles.h"
#include "../ID/Items.h"
#include "../Objects/DroppedItem.h"

class Player {
	friend class Game;
public:
	Player();
	~Player() = default;
	Player(const Player&) = delete;
	Player& operator =(const Player&) = delete;

	bool pickUp(DroppedItem& drop);

	void hit(int damage, int centerX, int kbPower);

	// Getters/Setters
	SDL_Point getCPos() const { return mRect.center(); }
	Point<double> getCPosf() const;
	Point<double> getPos() const { return pos; }
	void setPos(const Point<double>& newP);

	bool pointInPlayerBlock(SDL_Point blockPos) const;

protected:
	void tick(Event& e);
	void draw();

private:
	void move(Timestep dt);
	void drawUI();

	void leftClick(SDL_Point mouse);
	void rightClick(SDL_Point mouse);

	bool placeBlock(SDL_Point loc, tile::Id tileId);
	bool breakBlock(SDL_Point loc);

	void respawn();

	/*
	Stats stats(STATS, DEF_PLAYER);
	Stats itemStats(TOOL_STATS);
	stats.addStats(itemStats);
	
	ActiveUI activeUI;
	SDL_Point activeBlock{ 0,0 };
	// TODO: dragging ui

	CraftingUI craftingUI;

	Map map;
	*/

	// Item begin used
	item::Id itemUsed = item::Id::numItems;
	double useTime = 0;
	bool usedLeft = true, firstSwing = true;

	// Player image
	TextureData mTex;
	Rect mRect, spriteRect, armRect;
	Point<double> dim;

	// Inventory
	PlayerInventory inventory;

	// Ranges
	static const double PICKUP_DX, PICKUP_DY;
	Rect pickUpRange;
	static const double PLACE_DX, PLACE_DY;
	Rect placementRange;

	// Physics variables
	Point<double> pos, v, a;
	// Did we collide in x,y?
	enum CollideType : uint8_t { topLeft = 0, none, botRight };
	Point<CollideType> collided;
	// Distance fallen
	double fallDist = 0;

	// Attack immunity
	int immunity = 0;
	// Respawn counter
	int respawnCtr = 0;
	// Can we move?
	bool canMove = true;

	bool craftingOpen = false;
	bool mapOpen = false;

	std::string fileName;

	const static std::string IMG;
};

class PlayerAccess : public Player {
	friend class Game;
public:
	PlayerAccess() = default;
	~PlayerAccess() = default;
};

#endif