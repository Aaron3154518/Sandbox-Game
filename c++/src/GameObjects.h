#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H
// TODO: Change back to include each file in header

// Forward Declarations
class World;
class Player;

namespace GameObjects {
	World& world();
	Player& player();
}

/*
class GameObjects {
public:
	static GameObjects& Get();
	static World& GetWorld();
	static Player& GetPlayer();

	World& world() { return mWorld; }
	Player& player() { return mPlayer; }

private:
	GameObjects() = default;
	~GameObjects() = default;
	GameObjects(const GameObjects& other) = delete;
	GameObjects& operator =(const GameObjects& other) = delete;

	World mWorld;
	Player mPlayer;
};*/

#endif