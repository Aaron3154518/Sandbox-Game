#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include "Player/PlayerInventory.h"
#include "Player/Player.h"
#include "World/World.h"

namespace GameObjects {
	WorldAccess& world();
	PlayerAccess& player();
	PlayerInventory& playerInv();
}

#endif