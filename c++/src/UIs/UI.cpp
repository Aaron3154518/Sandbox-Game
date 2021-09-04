#include "UI.h"
#include "../Window.h"

//#define DEBUG_UI

void UI::runUI() {
#ifdef DEBUG_UI
    std::cerr << "Running" << std::endl;
#endif
    // Create/use a copy in case textInput is changed
    bool _textInput = textInput;
    if (_textInput) { SDL_StartTextInput(); }
    running = true;
    initUI();
    Event e;
    Uint32 time = SDL_GetTicks();
    while (running) {
        Timestep dt(SDL_GetTicks() - time);
        time = SDL_GetTicks();

        e.update(dt);

        SDL_RenderClear(Window::Get().renderer());
        tickUI(e);
        SDL_RenderPresent(Window::Get().renderer());

        if (dt.milliseconds() < gameVals::FRAME_DELAY()) {
            SDL_Delay(gameVals::FRAME_DELAY() - dt.milliseconds());
        }
    }
    if (_textInput) { SDL_StopTextInput(); }
}