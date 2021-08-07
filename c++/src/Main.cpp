#define SDL_MAIN_HANDLED

#include <iostream>

#include <SDL.h>

#include "Game/Game.h"
#include "World/PlayerSelector.h"
#include "Utils/Utils.h"
#include "Utils/UI.h"

int main(int argc, char* argv[]) {

    UI::init();

    UI::activeUIs.push_back(std::make_shared<PlayerSelector>());
    UI::run();

    UI::clean();

    return 0;
}