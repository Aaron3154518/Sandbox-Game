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

/*struct EventButton {
    SDL_Point clickPos = { 0,0 };
    uint32_t duration = 0;
};

struct EventData {
    // Event data
    Timestep dt;

    bool quit = false;
    bool resize = false;
    int newW = 0, newH = 0;

    SDL_Point mouse;
    double mouseDx = 0., mouseDy = 0.;
    // < 0 = down, > 0 = up
    Sint32 scroll = 0;

    // Inputted text
    std::string inputText;

    EventButton left, middle, right;

    void update(Timestep ts);
    void update(SDL_Event& e);

    bool keyDown(SDL_Keycode key) const;
    bool keyDown(SDL_Keycode key, Timestep& ts) const;
    bool keyPressed(SDL_Keycode key) const;
    bool keyReleased(SDL_Keycode key) const;

    bool clicked(const EventButton& button) const;

    enum Mode : uint8_t {
        pressed = 0x01, released = 0x02,
        held = 0x04, handled = 0x08
    };

    enum Mouse : uint32_t {
        left = 0x000F, right = 0x00F0, middle = 0x0F00,
        all = left | right | middle
    };

    enum ModeOpts : uint8_t {
        any = 0x01, // Only one condition must be true
        handle = 0x02 // Handle event after getting status
    };

    bool checkMouse(Mouse mouse, uint8_t mode, uint8_t opts);
    bool handleMouse(Mouse mouse);

    // Returns current value
    bool operator[](uint8_t val) const;
    bool operator[](SDL_KeyCode key) const;
    bool keyboardHandled() const { return mKeyboardHandled; }

    // Updates value
    void handle(uint8_t val) { mMouseHandled |= (val & !Mouse::ANY); }
    void handle(SDL_KeyCode key) { mKeysHandled.insert(key); }
    void handleKeyboard() { mKeyboardHandled = true; }

    // Returns current and updates
    bool getAndHandle(uint8_t val);
    bool getAndHandle(SDL_KeyCode key);
    bool getAndHandleKeyboard();

    static uint32_t quadruple(const uint8_t& val);

private:
    static constexpr int MAX_CLICK_DIFF = 10;

    uint32_t mouseState = 0;
    // Handle entire keyboard
    bool mKeyboardHandled = false;
    // Keys currently down
    std::map<SDL_Keycode, Timestep> down;
    // Set of just pressed/released keys
    std::set<SDL_Keycode> pressed, released;

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
};*/


// TODO: EventStatus separate (can create multiple too)
struct Event {
    typedef uint8_t Status;
    typedef SDL_KeyCode Key;
    enum Mouse : uint8_t {
        LEFT = 0, RIGHT, MIDDLE
    };

    enum HandledStatus : Status {
        IGNORED = 0, // Handled Status is ignored
        ACTIVE, // Currently handling (taking events)
        HANDLED, // Has been handled
        NOT_HANDLED // Not handled and not currently handling
    };
    enum ButtonStatus : Status {
        PRESSED = 0x01,
        RELEASED = 0x02,
        HELD = 0x04,

        CLICKED = 0x08 // Just for mouse buttons
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
    bool checkMouse(Mouse mouse, Status status);
    bool checkKey(Key key, Status status);
    void handleKey(Key key);
    bool checkAndHandleKey(Key key, Status status);

    void nextUI(Rect r, bool textInput);

    // Privatizeificateify
    void update(Timestep ts);

private:
    static constexpr Status KEY_HANDLED = 0x80;
    static constexpr int MAX_CLICK_DIFF = 10;

    static bool canUse(HandledStatus status);

    void update(SDL_Event& e);
    static Mouse toMouse(int sdlButtonType);

    HandledStatus mouseStatus = HandledStatus::IGNORED;
    std::map<Mouse, MouseButton> mouseButtons;
    HandledStatus keyboardStatus = HandledStatus::IGNORED;
    std::map<Key, KeyButton> keyButtons;
};

#endif
