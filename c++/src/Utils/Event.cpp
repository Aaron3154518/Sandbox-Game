#include "Event.h"

//#define DEBUG_EVENT

/*
uint32_t EventData::quadruple(const uint8_t& val) {
    uint32_t num = val;
    return num | (num << 8) | (num << 16) | (num << 24);
}

bool EventStatus::operator[](uint8_t val) const {
    // All flags must be true
    if (val & Mouse::ANY == 0) {
        return mMouseHandled & val != val;
    // Any flag must be true
    } else {
        return mMouseHandled & val == 0;
    }
    return false;
}
bool EventStatus::operator[](SDL_KeyCode key) const {
    return !mKeyboardHandled && mKeysHandled.find(key) == mKeysHandled.end();
}

bool EventStatus::getAndHandle(uint8_t val) {
    bool result = (*this)[e];
    handle(val);
    return result;
}
bool EventStatus::getAndHandle(SDL_KeyCode key) {
    bool result = (*this)[key];
    handle(key);
    return result;
}
bool EventStatus::getAndHandleKeyboard() {
    bool result = mKeyboardHandled;
    handleKeyboard();
    return result;
}
// Event
bool EventData::checkMouse(uint32_t mouse, uint8_t mode, uint8_t opts) {
    uint32_t data = quadruple(mode) & mouse;
    bool result = opts & ModeOpts::any ? data & mouseState != data
        : data & mouseState == 0);
    if (opts & ModeOpts::handle) { handleMouse(mouse); }
    return result;
}

const EventData::handleMouse(Mouse mouse) {
    mouseState |= quadruple(Mode::handled) & mouse;
}

bool EventData::checkKey(SDL_KeyCode key, uint8_t mode, uint8_t opts) {
    uint32_t data = quadruple(mode) & mouse;
    bool result = opts & ModeOpts::any ? data & mouseState != data
        : data & mouseState == 0);
        if (opts & ModeOpts::handle) { handleMouse(mouse); }
        return result;
}

const EventData::handleMouse(Mouse mouse) {
    mouseState |= quadruple(Mode::handled) & mouse;
}

bool EventData::keyDown(SDL_Keycode key) const {
    return down.find(key) != down.end();
}

bool EventData::keyDown(SDL_Keycode key, Timestep& ts) const {
    auto it = down.find(key);
    if (it != down.end()) { ts = it->second; return true; }
    return false;
}

bool EventData::keyPressed(SDL_Keycode key) const {
    return pressed.find(key) != pressed.end();
}

bool EventData::keyReleased(SDL_Keycode key) const {
    return released.find(key) != released.end();
}

bool EventData::clicked(const EventButton& button) const {
    return button.clicked &&
        distance(button.clickPos, mouse) < MAX_CLICK_DIFF;
}
*/


void Event::nextUI(Rect r, bool textInput) {
    if (mouseStatus == HandledStatus::ACTIVE) {
        mouseStatus = HandledStatus::HANDLED;
    } else if (mouseStatus != HandledStatus::HANDLED) {
        mouseStatus = SDL_PointInRect(&mouse, &r) ? HandledStatus::ACTIVE
            : HandledStatus::NOT_HANDLED;
    }
    if (keyboardStatus == HandledStatus::ACTIVE) {
        keyboardStatus = HandledStatus::HANDLED;
    } else if (keyboardStatus != HandledStatus::HANDLED) {
        keyboardStatus = textInput ? HandledStatus::ACTIVE
            : HandledStatus::NOT_HANDLED;
    }
}

bool Event::checkScroll() const {
    return canUse(mouseStatus);
}

bool Event::checkHover() const {
    return canUse(mouseStatus);
}

bool Event::checkMouse(Mouse mouse, Status status) {
    return canUse(mouseStatus)
        && (mouseButtons[mouse].status & status) == status;
}

bool Event::checkKey(Key key, Status status) {
    if (keyboardStatus != HandledStatus::HANDLED) {
        Status checkStatus = status;
        if (keyboardStatus != HandledStatus::ACTIVE) {
            // Also need to check if it's been handled
            checkStatus |= KEY_HANDLED;
        }
        return (keyButtons[key].status & checkStatus) == status;
    }
    return false;
}

void Event::handleKey(Key key) {
    keyButtons[key].status |= KEY_HANDLED;
}

bool Event::checkAndHandleKey(Key key, Status status) {
    if (checkKey(key, status)) {
        handleKey(key);
        return true;
    }
    return false;
}

bool Event::canUse(HandledStatus status) {
    return status == HandledStatus::ACTIVE || status == HandledStatus::IGNORED;
}

void Event::update(Timestep ts) {
    dt = ts;
    uint32_t ms = dt.milliseconds();
    // Update buttons
    for (auto& pair : mouseButtons) {
        MouseButton& b = pair.second;
        if (b.status & ButtonStatus::HELD != 0) { b.duration += ms; }
        // Reset pressed/released
        b.status &= ButtonStatus::HELD;
    }
    // Update keys
    for (auto& pair : keyButtons) {
        KeyButton& b = pair.second;
        if (b.status & ButtonStatus::HELD != 0) { b.duration += ms; }
        // Reset pressed/released and handled
        b.status &= ButtonStatus::HELD;
    }
    // Reset text editing
    inputText = "";
    // Update mouse
    SDL_GetMouseState(&mouse.x, &mouse.y);
    // Reset mouse movement
    mouseDx = mouseDy = 0;
    scroll = 0;
    // Reset event flags
    quit = resize = false;
    // Parse events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                std::cerr << "Quit" << std::endl;
                quit = true;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    resize = true;
                    newW = e.window.data1;
                    newH = e.window.data2;
                }
                break;
            default:
                update(e);
                break;
        }
    }
    // Reset handled status
    mouseStatus = keyboardStatus = HandledStatus::IGNORED;
}

void Event::update(SDL_Event& e) {
    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
        {
            MouseButton& b = mouseButtons[toMouse(e.button.button)];
            b.status = ButtonStatus::PRESSED | ButtonStatus::HELD;
            b.duration = 0;
            b.clickPos = mouse;
        }
        break;
        case SDL_MOUSEBUTTONUP:
        {
            MouseButton& b = mouseButtons[toMouse(e.button.button)];
            b.status = (distance(b.clickPos, mouse) < MAX_CLICK_DIFF ?
                ButtonStatus::CLICKED : 0) | ButtonStatus::RELEASED;
        }
        break;
        case SDL_MOUSEMOTION:
        {
            mouseDx = e.motion.xrel;
            mouseDy = e.motion.yrel;
        }
        break;
        case SDL_MOUSEWHEEL:
        {
            scroll = -e.wheel.y;
        }
        break;
        case SDL_KEYDOWN:
        {
            KeyButton& b = keyButtons[(SDL_KeyCode)e.key.keysym.sym];
            b.status = ButtonStatus::PRESSED | ButtonStatus::HELD;
            b.duration = 0;
        }
        break;
        case SDL_KEYUP:
        {
            KeyButton& b = keyButtons[(SDL_KeyCode)e.key.keysym.sym];
            b.status = ButtonStatus::RELEASED;
        }
        break;
        case SDL_TEXTEDITING:
            break;
        case SDL_TEXTINPUT:
        {
            inputText = e.text.text;
        }
        break;
        default:
#ifdef EVENT_DEBUG
            std::cerr << "Unhandled Event" << std::endl;
#endif
            break;
    }
}

Event::Mouse Event::toMouse(int sdlButtonType) {
    switch (sdlButtonType) {
        case SDL_BUTTON_RIGHT:
            return Mouse::RIGHT;
            break;
        case SDL_BUTTON_MIDDLE:
            return Mouse::MIDDLE;
            break;
        default:
            return Mouse::LEFT;
            break;
    }
}