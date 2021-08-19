#include "GameObjects.h"
#include "Player/Player.h"
#include "World/World.h"

namespace GameObjects {
	World& world() {
		static World WORLD;
		return WORLD;
	}

	Player& player() {
		static Player PLAYER;
		return PLAYER;
	}
}

/*GameObjects& GameObjects::Get() {
	static GameObjects gameInterface;
	return gameInterface;
}

World& GameObjects::GetWorld() {
	return Get().mWorld;
}

Player& GameObjects::GetPlayer() {
	return Get().mPlayer;
}*/