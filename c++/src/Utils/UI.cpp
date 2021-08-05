#include "UI.h"

#include "Utils.h"
#include "AssetManager.h"

bool UI::initialized = false;
int UI::maxW = 0, UI::maxH = 0, UI::w = 0, UI::h = 0;
SDL_Renderer* UI::mRenderer = NULL;
SDL_Window* UI::mWindow = NULL;
AssetManager UI::mAssetManager = AssetManager();

void UI::run() {
    std::cerr << "Running" << std::endl;
    Event e;
    while (running) {
        Timestep dt(SDL_GetTicks() - gameTime);
        gameTime += dt.milliseconds();

        e.update(dt);
        if (e.resize) {
            UI::resize(e.newW, e.newH);
        }

        SDL_RenderClear(mRenderer);
        tick(e);
        SDL_RenderPresent(mRenderer);

        if (dt.milliseconds() < gameVals::FRAME_DELAY) {
            SDL_Delay(gameVals::FRAME_DELAY - dt.milliseconds());
        }
    }
}

int UI::width() { return w; }
int UI::height() { return h; }
SDL_Renderer* UI::renderer() { return mRenderer; }
AssetManager& UI::assets() { return mAssetManager; }

void UI::init() {
    // Initialize main SDL module
    int flags = SDL_WINDOW_RESIZABLE;
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "SDL Initialized" << std::endl;

        Rect temp;
        SDL_GetDisplayUsableBounds(0, &temp);
        maxW = temp.w; maxH = temp.h;

        w = gameVals::MIN_W; h = gameVals::MIN_H;
        mWindow = SDL_CreateWindow("Sandbox Game", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, w, h, flags);
        std::cout << "Window Dimensions: " << w << " X " << h << std::endl;

        mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
        }
    }

    // Initialize SDL fonts module
    if (TTF_Init() == 0) {
        std::cout << "SDL_TTF Initialized" << std::endl;

        UI::mAssetManager.loadFont(gameVals::LARGE_FONT, FONT(times.ttf), -1, gameVals::MIN_H / 10);
        UI::mAssetManager.loadFont(gameVals::SMALL_FONT, FONT(times.ttf), -1, gameVals::MIN_H / 25);
    }

    initialized = true;
}

void UI::clean() {
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(mRenderer);
    SDL_Quit();
    mAssetManager.clean();
    initialized = false;
    std::cout << "SDL Uninitialized" << std::endl;
}

void UI::setDrawColor(const SDL_Color& c) {
    SDL_SetRenderDrawColor(mRenderer, c.r, c.g, c.b, c.a);
}
void UI::resetDrawColor() {
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
}
void UI::setRenderTarget(SDL_Texture* tex) {
    if (SDL_SetRenderTarget(mRenderer, tex) != 0) {
        std::cout << "Unable to set render target" << std::endl;
    }
}
void UI::resetRenderTarget() {
    SDL_SetRenderTarget(mRenderer, NULL);
    resetDrawColor();
}