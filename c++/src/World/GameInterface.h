#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include "World.h"

class GameInterface {
public:
	static GameInterface& Get();
	static World& GetWorld();

	World& world() { return mWorld; }

private:
	GameInterface() = default;
	~GameInterface() = default;
	GameInterface(const GameInterface& other) = delete;
	GameInterface& operator =(const GameInterface& other) = delete;

	World mWorld;
};

#endif