#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <map>
#include <set>

#include <SDL.h>

#include "Rect.h"
#include "Utils.h"

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

// TODO: EventStatus separate (can create multiple too)
// TODO: Getters
struct Event {
    typedef uint8_t Status;
    typedef SDL_KeyCode Key;
    enum Mouse : uint8_t {
        LEFT = 0, RIGHT, MIDDLE
    };

    enum Access : Status {
        IGNORED = 0, // Handled status is ignored
        ACTIVE, // Currently handling (taking events)
        INACTIVE, // Not handled and not currently handling
        HANDLED // Has been handled
    };
    enum Button : Status {
        PRESSED = 0x01,
        RELEASED = 0x02,
        HELD = 0x04,

        M_CLICKED = 0x08, // For mouse buttons
        K_HANDLED = 0x08  // For key buttons
    };

    struct MouseButton {
        Status status = 0;
        SDL_Point clickPos{ 0,0 };
        uint32_t duration = 0;
    };
    struct KeyButton {
        Status status = 0;
        uint32_t duration = 0;
    };

    // Event data
    Timestep dt;

    bool quit = false;
    bool resize = false;
    int newW = 0, newH = 0;

    SDL_Point mouse;
    double mouseDx = 0., mouseDy = 0.;
    // < 0 = down, > 0 = up
    int scroll = 0;

    // Inputted text
    std::string inputText;

    bool checkScroll() const;
    bool checkHover() const;
    uint8_t operator[](const Mouse& mouse);
    uint8_t operator[](const Key& key);
    void handleKey(Key key);

    const MouseButton& getMouse(const Mouse& mouse);
    const KeyButton& getKey(const Key& key);

    void nextUI(Rect r, bool textInput);

    // Privatizeificateify
    void update(Timestep ts);

private:
    static constexpr int MAX_CLICK_DIFF = 10;

    static bool canUse(Access status);

    void update(SDL_Event& e);
    static Mouse toMouse(int sdlButtonType);

    Access mouseStatus = Access::IGNORED;
    std::map<Mouse, MouseButton> mouseButtons;
    Access keyboardStatus = Access::IGNORED;
    std::map<Key, KeyButton> keyButtons;
};

#endif
