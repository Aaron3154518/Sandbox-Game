#include "GameInterface.h"

GameInterface& GameInterface::Get() {
	static GameInterface gameInterface;
	return gameInterface;
}

World& GameInterface::GetWorld() {
	return Get().mWorld;
}