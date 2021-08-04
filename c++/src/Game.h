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

#include "Definitions.h"
#include "Utils/Utils.h"
#include "Utils/Rect.h"
#include "Utils/AssetManager.h"

// Game class
class Game {
private:
    Game() = default;
    ~Game() = default;

    bool mRunning = false;
    SDL_Window* mWindow;
public:
    Game(const Game&) = delete;
    void operator=(const Game&) = delete;

    static Game& get();

    void init();
    void clean();
    void tick();
    void updateTime();
    void handleEvents();
    void update();
    void render();

    void setDrawColor(const SDL_Color& c);
    void resetDrawColor();

    void setRenderTarget(SDL_Texture* tex);
    void resetRenderTarget();

    int maxW = 0, maxH = 0;
    int icon_w = 0, text_h = 0;
    Rect screen;
    SDL_Renderer* renderer;
    AssetManager assets;

    Uint32 gameTime = 0;
    Timestep ts;
    Event event;

    // Getters
    bool running() { return mRunning; }
    Rect getAbsRect(const Rect& r);
};

#endif /* GAME_h */
