#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include "World.h"
#include "../Player/Player.h"

class GameInterface {
public:
	static GameInterface& Get();
	static World& GetWorld();
	static Player& GetPlayer();

	World& world() { return mWorld; }
	Player& player() { return mPlayer; }

private:
	GameInterface() = default;
	~GameInterface() = default;
	GameInterface(const GameInterface& other) = delete;
	GameInterface& operator =(const GameInterface& other) = delete;

	World mWorld;
	Player mPlayer;
};

#endif