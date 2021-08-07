#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <fcntl.h>
#include <sys/stat.h>

#include <SDL.h>

#include "Rect.h"

// TODO: fix
struct Animation;
struct Recipe;

// String functions
std::string lowerCase(std::string str);

bool strToDouble(const std::string& str, double& result);

std::string replaceInString(const std::string& source,
    const std::string& search, const std::string& replace);

// File functions
bool isFile(const std::string& fileName);
bool isDir(const std::string& dirName);

bool validSaveFile(const std::string& fileName);

std::string createFile(const std::string& folder,
    const std::string& file, const std::string& ext);

std::string toFileName(const std::string& displayName);
std::string toDisplayName(const std::string& fileName);

// Point/vector functions
double magnitude(SDL_Point p);
double distance(SDL_Point p1, SDL_Point p2);

// Helper classes
class Timestep {
public:
    Timestep(uint32_t time = 0)
        : mTime(time) {}

    operator uint32_t() const { return mTime; }

    float seconds() const { return mTime / 1000.0f; }
    uint32_t milliseconds() const { return mTime; }

    void add(Timestep t) { mTime += t.mTime; }
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
    Sint32 scroll = 0;

    // Keys currently down
    std::map<SDL_Keycode, Timestep> down;
    // Set of just pressed/released keys
    std::set<SDL_Keycode> pressed, released;
    // Inputted text
    std::string inputText;

    EventButton left, middle, right;

    void updateMouse(int x, int y);
    void update(Timestep ts);
    void update(SDL_Event& e);

    bool keyDown(SDL_Keycode key, Timestep& ts) const;
    bool keyPressed(SDL_Keycode key) const;
    bool keyReleased(SDL_Keycode key) const;

    bool clicked(const EventButton& button) const;
private:
    static constexpr int MAX_CLICK_DIFF = 10;

    EventButton& getButton(int sdlButtonType) {
        switch (sdlButtonType) {
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