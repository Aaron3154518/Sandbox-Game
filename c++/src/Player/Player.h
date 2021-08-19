#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

#include "../Definitions.h"
#include "../UIs/UI.h"
#include "../Utils/Rect.h"
#include "../Utils/Point.h"
#include "../Utils/AssetManager.h"
#include "../ID/Items.h"

class Player {
	friend class Game;
public:
	Player();
	~Player() = default;
	Player(const Player&) = delete;
	Player& operator =(const Player&) = delete;

	void hit(int damage, int centerX, int kbPower);

	// Getters/Setters
	std::string fullImgFile() const;
	SDL_Point getCPos() const { return mRect.center(); }
	Point<double> getPos() const { return pos; }
	void setPos(const Point<double>& newP);

private:
	void tick(Event& e);
	void move(Timestep dt);
	void draw();
	void drawUI();

	void respawn();

	/*
	Stats stats(STATS, DEF_PLAYER);
	Stats itemStats(TOOL_STATS);
	stats.addStats(itemStats);

	PlayerInventory inventory;
	
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
	std::string img = "player_pig.png";
	Rect mRect, spriteRect, armRect;
	Point<double> dim;

	// Ranges
	Rect collectionRange;
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
};

#endif