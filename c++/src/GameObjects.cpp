#include "GameObjects.h"

GameObjects& GameObjects::Get() {
	static GameObjects gameInterface;
	return gameInterface;
}

World& GameObjects::GetWorld() {
	return Get().mWorld;
}

Player& GameObjects::GetPlayer() {
	return Get().mPlayer;
}