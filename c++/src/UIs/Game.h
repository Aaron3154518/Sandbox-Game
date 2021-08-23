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
#include "../GameObjects.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"

// Game class
class Game : public UI {
public:
    Game(const std::string& _player, const std::string& _universe);
    ~Game() = default;

    void initUI();
    void tickUI(Event &e);
private:
    std::string player, universe;
    SDL_Point playerPos = { 0,0 };

    void handleEvents(Event& e);
    void update();
    void render();
};

#endif /* GAME_h */
