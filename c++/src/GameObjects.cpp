#include "GameObjects.h"

namespace GameObjects {
	WorldAccess& world() {
		static WorldAccess WORLD;
		return WORLD;
	}
	PlayerAccess& player() {
		static PlayerAccess PLAYER;
		return PLAYER;
	}
}