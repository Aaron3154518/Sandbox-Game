#ifndef GAME_H
#define GAME_H

#include <cmath>
#include <vector>
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

// Game class
class Game : public UI {
public:
    Game(const std::string& player, const std::string& universe);
    ~Game() = default;

    void tick(Event &e);
private:
    World world;

    void handleEvents(Event& e);
    void update();
    void render();
};

#endif /* GAME_h */
