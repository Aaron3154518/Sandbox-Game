#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstdint>
#include <sstream>
#include <algorithm>

#include <SDL.h>

// TODO: fix
struct Animation;
struct Recipe;

std::string lowerCase(std::string str);

bool strToDouble(std::string str, double& result);

// Helper classes
class Timestep {
public:
    Timestep(uint32_t time = 0)
        : mTime(time) {}

    operator uint32_t() const { return mTime; }

    float seconds() const { return mTime / 1000.0f; }
    uint32_t milliseconds() const { return mTime; }
private:
    uint32_t mTime;
};

struct EventButton {
    bool pressed = false, clicked = false;
    SDL_Point clickPos = { 0,0 };
    uint32_t duration = 0;
};

struct Event {
    bool handled = false;
    Timestep dt;

    bool quit = false;

    bool resize = false;
    int newW = 0, newH = 0;

    SDL_Point mouse;
    double mouseDx = 0., mouseDy = 0.;

    EventButton left, middle, right;

    void updateMouse(int x, int y);
    void update(Timestep ts);
    void update(SDL_Event& e);

private:
    EventButton& getButton(int sdl_button_type) {
        switch (sdl_button_type) {
        case SDL_BUTTON_LEFT:
            return left;
            break;
        case SDL_BUTTON_RIGHT:
            return right;
            break;
        case SDL_BUTTON_MIDDLE:
            return middle;
            break;
        default:
            return left;
            break;
        }
    }
};

#endif