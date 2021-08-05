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
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/AssetManager.h"
#include "../Utils/UI.h"

// Game class
class Game : public UI {
public:
    //Game() = default;
    Game() {
        td.fontId = gameVals::LARGE_FONT;
        td.text = "Wassup World";
        td.color = PURPLE;
        td.xMode = CENTER;
        td.yMode = CENTER;
    }
    ~Game() = default;

    void tick(Event &e);
private:
    TextData td;
    bool first = false;
    void handleEvents(Event& e);
    void update();
    void render();
};

#endif /* GAME_h */
