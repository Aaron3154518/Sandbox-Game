#define SDL_MAIN_HANDLED

#include <iostream>

#include <SDL.h>

#include "UIs/PlayerSelector.h"
#include "Window.h"

int main(int argc, char* argv[]) {

    Window::Get().activeUIs.push_back(std::make_shared<PlayerSelector>());
    Window::Get().run();

    return 0;
}