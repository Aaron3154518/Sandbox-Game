#ifndef GAME_H
#define GAME_H

#include <cmath>
#include <fstream>
#include <stack>
#include <sys/stat.h>
#include <vector>
#include <unordered_map>

#include "dirent.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "UI.h"
#include "../Definitions.h"
#include "../Player/Player.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/Rect.h"
#include "../World/World.h"

// Game class
class Game : public UI {
public:
    static PlayerAccess& Player();
    static WorldAccess& World();

    Game(const std::string& _player, const std::string& _universe);
    ~Game() = default;

private:
    void initUI();
    void tickUI(Event& e);

    void handleEvents(Event& e);
    void render();

    struct GameData {
        PlayerAccess player;
        WorldAccess world;
        void init(const std::string& pFile, const std::string& uFile);
    };

    static GameData data;
    std::string player, universe;
};

#endif /* GAME_h */
