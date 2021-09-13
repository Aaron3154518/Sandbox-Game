#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

#include "PlayerInventory.h"
#include "../Definitions.h"
#include "../ID/Items.h"
#include "../ID/Tiles.h"
#include "../Objects/DroppedItem.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/FileIO.h"
#include "../Utils/Point.h"
#include "../Utils/Rect.h"
#include "../Window.h"

class Player {
public:
	Player();
	~Player() = default;
	Player(const Player&) = delete;
	Player& operator =(const Player&) = delete;

	// Returns true if the drop was completely picked up
	// and should be destroyed
	bool pickUp(DroppedItem& drop);

	void hit(int damage, int centerX, int kbPower);

	// Getters/Setters
	SDL_Point getCPos() const { return mRect.center(); }
	Point<double> getCPosf() const;
	Point<double> getPos() const { return pos; }
	void setPos(const Point<double>& newP);
	PlayerInventory& getInventory() { return inventory; }

	bool collidesPlayer(Rect blockRect) const;

	bool placeBlock(SDL_Point worldPos, tile::Id tileId);
	bool breakBlock(SDL_Point worldPos);

	void read(IO& io);
	void write(IO& io);
	void load();
	void save();

	static void createNewPlayer(IO& io);

protected:
	void handleEvents(Event& e);
	void draw();

	void setFile(std::string fName);

private:
	void reset();

	void move(Timestep dt);
	void drawUI();

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

	// Player image
	RenderData rData, cursorData;
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

	// File variables
	std::string fileName;
	FileRead fr;
	FileWrite fw;

	const static std::string IMG;
};

class PlayerAccess : public Player {
	friend class Game;
public:
	PlayerAccess() = default;
	~PlayerAccess() = default;
};

#endif