#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include "Player/Player.h"
#include "Player/PlayerInventory.h"
#include "World/World.h"

namespace GameObjects {
	WorldAccess& world();
	PlayerAccess& player();
	PlayerInventory& playerInv();
}

#endif