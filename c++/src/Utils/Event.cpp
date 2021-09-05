#include "Event.h"

//#define DEBUG_EVENT

void Event::nextUI(Rect r, bool textInput) {
    if (mouseStatus == Access::ACTIVE) {
        mouseStatus = Access::HANDLED;
    } else if (mouseStatus != Access::HANDLED) {
        mouseStatus = SDL_PointInRect(&mouse, &r) ? Access::ACTIVE
            : Access::INACTIVE;
    }
    if (keyboardStatus == Access::ACTIVE) {
        keyboardStatus = Access::HANDLED;
    } else if (keyboardStatus != Access::HANDLED) {
        keyboardStatus = textInput ? Access::ACTIVE : Access::INACTIVE;
    }
}

bool Event::checkScroll() const {
    return canUse(mouseStatus);
}

bool Event::checkHover() const {
    return canUse(mouseStatus);
}

uint8_t Event::operator[](const Mouse& mouse) {
    return canUse(mouseStatus) ? mouseButtons[mouse].status : 0x00;
}

uint8_t Event::operator[](const Key& key) {
    // Keyboard is not handled and is either active or
    // the handled bit for this specific key is not set
    Status status = keyButtons[key].status;
    if (keyboardStatus != Access::HANDLED
        && (keyboardStatus == Access::ACTIVE
            || !any8(status, Button::K_HANDLED))) {
        return status;
    }
    return 0x00;
}

void Event::handleKey(Key key) {
    keyButtons[key].status |= Button::K_HANDLED;
}

bool Event::canUse(Access status) {
    return status == Access::ACTIVE || status == Access::IGNORED;
}

void Event::update(Timestep ts) {
    dt = ts;
    uint32_t ms = dt.milliseconds();
    // Update buttons
    for (auto& pair : mouseButtons) {
        MouseButton& b = pair.second;
        if (any8(b.status, Button::HELD)) { b.duration += ms; }
        // Reset pressed/released
        b.status &= Button::HELD;
    }
    // Update keys
    for (auto& pair : keyButtons) {
        KeyButton& b = pair.second;
        if (any8(b.status, Button::HELD)) { b.duration += ms; }
        // Reset pressed/released and handled
        b.status &= Button::HELD;
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
#ifdef DEBUG_EVENT
                std::cerr << "Quit" << std::endl;
#endif
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
    mouseStatus = keyboardStatus = Access::IGNORED;
}

void Event::update(SDL_Event& e) {
    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
        {
            MouseButton& b = mouseButtons[toMouse(e.button.button)];
            b.status = Button::PRESSED | Button::HELD;
            b.duration = 0;
            b.clickPos = mouse;
        }
        break;
        case SDL_MOUSEBUTTONUP:
        {
            MouseButton& b = mouseButtons[toMouse(e.button.button)];
            b.status = (distance(b.clickPos, mouse) < MAX_CLICK_DIFF ?
                Button::M_CLICKED : 0) | Button::RELEASED;
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
            b.status = Button::PRESSED | Button::HELD;
            b.duration = 0;
        }
        break;
        case SDL_KEYUP:
        {
            KeyButton& b = keyButtons[(SDL_KeyCode)e.key.keysym.sym];
            b.status = Button::RELEASED;
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