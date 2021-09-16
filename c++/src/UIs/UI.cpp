#include "UI.h"
#include "../Window.h"

//#define DEBUG_UI

void UI::printPerformance() {
    std::cerr << "Performance:" << std::endl;
    if (dtSum == 0 || dtCnt == 0) {
        std::cerr << "No Timesteps Collected" << std::endl;
    } else {
        std::cerr << "Average Timestep: " << ((double)dtSum / dtCnt) << std::endl;
        std::cerr << "Average FPS: " << (1000. * dtCnt / dtSum) << std::endl;
    }
}

void UI::runUI() {
#ifdef DEBUG_UI
    std::cerr << "Running" << std::endl;
#endif
    dtSum = dtCnt = 0;
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

        dtSum += dt.milliseconds();
        dtCnt++;

        e.update(dt);

        SDL_RenderClear(Window::Get().renderer());
        tickUI(e);
        SDL_RenderPresent(Window::Get().renderer());

        Uint32 localDt = SDL_GetTicks() - time;
        if (localDt < gameVals::FRAME_DELAY()) {
            SDL_Delay(gameVals::FRAME_DELAY() - localDt);
        }
    }
    if (_textInput) { SDL_StopTextInput(); }

#ifdef DEBUG_UI
    printPerformance();
#endif
}