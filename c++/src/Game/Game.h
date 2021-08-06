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
    Game(int id) {
        td.fontId = gameVals::LARGE_FONT;
        switch (id) {
        case 0:
            td.text = "Hello World";
            //addSubUI(new Game(1));
            subUIs.push_back(std::make_shared<Game>(1));
            subUIs.push_back(std::make_shared<Game>(2));
            nextUIs.push_back(std::make_shared<Game>(3));
            nextUIs.push_back(std::make_shared<Game>(4));
            nextUIs.push_back(std::make_shared<Game>(5));
            break;
        case 1:
            td.text = "More like Wassup World!";
            break;
        case 2:
            td.text = "Anyway, back to the boring message";
            break;
        case 3:
            td.text = "Hello Human";
            nextUIs.push_back(std::make_shared<Game>(6));
            break;
        case 4:
            td.text = "Now you will DIE!";
            break;
        case 5:
            td.text = "AAAARRGH!!!";
            break;
        case 6:
            td.text = "What!?";
            break;
        }
        td.color = PURPLE;
        td.xMode = CENTER;
        td.yMode = CENTER;
    }
    //~Game() = default;
    ~Game() { std::cerr << "Deinitialized: " << td.text << std::endl; }

    void tick(Event &e);
private:
    TextData td;

    void handleEvents(Event& e);
    void update();
    void render();
};

#endif /* GAME_h */
