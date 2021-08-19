#ifndef GAME_H
#define GAME_H

#include <cmath>
#include <vector>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <sys/stat.h>

#include "dirent.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "../Definitions.h"
#include "UI.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/AssetManager.h"
#include "../World/World.h"
#include "../GameObjects.h"

// Game class
class Game : public UI {
public:
    Game(const std::string& _player, const std::string& _universe);
    ~Game() = default;

    void initUI();
    void tickUI(Event &e);
private:
    static GameObjects& game;

    std::string player, universe;
    SDL_Point playerPos = { 0,0 };

    void handleEvents(Event& e);
    void update();
    void render();
};

#endif /* GAME_h */
