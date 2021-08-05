#define SDL_MAIN_HANDLED

#include <iostream>

#include <SDL.h>

#include "Game/Game.h"
#include "Utils/Utils.h"
#include "Utils/UI.h"

int main(int argc, char* argv[]) {

    UI::init();

    Game().run();

    UI::clean();

    return 0;
}