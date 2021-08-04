#include "Utils.h"

std::string lowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return str;
}

bool strToDouble(std::string str, double& result) {
    std::istringstream ss(str);
    ss >> result;
    return !ss.fail() && ss.eof() ? true : false;
}

// Event
void Event::update(Timestep ts) {
    for (EventButton* eb : { &left, &middle, &right }) {
        if (eb->pressed) { eb->duration += ts; }
        eb->clicked = false;
    }
}
void Event::update(SDL_Event& e) {
    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
    {
        EventButton& eb = getButton(e.button.button);
        eb.pressed = true;
        eb.duration = 0;
        eb.clickPos = mouse;
        eb.clickPosGlobal = globalMouse;
    }
    break;
    case SDL_MOUSEBUTTONUP:
    {
        EventButton& eb = getButton(e.button.button);
        eb.pressed = false;
        eb.clicked = true;
    }
    break;
    case SDL_MOUSEMOTION:
        mouseDx = e.motion.xrel;
        mouseDy = e.motion.yrel;
        break;
    }
}