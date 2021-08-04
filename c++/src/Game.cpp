#include "Game.h"

//#define DEBUG_GAME

Game& Game::get() {
    static Game game;
    return game;
}

void Game::init() {
    if (mRunning) { return; }
    int flags = SDL_WINDOW_RESIZABLE;
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "SDL Initialized" << std::endl;

        Rect temp;
        SDL_GetDisplayUsableBounds(0, &temp);
        maxW = temp.w; maxH = temp.h;

        screen = Rect(0, 0, maxW / 2, maxH / 2);
        std::cout << "Screen Set: " << screen << std::endl;
        mWindow = SDL_CreateWindow("Wizard Idle", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, screen.w, screen.h, flags);

        renderer = SDL_CreateRenderer(mWindow, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }

        mRunning = true;
    }

    if (TTF_Init() == 0) {
        std::cout << "SDL_TTF Initialized" << std::endl;
    }
    else {
        mRunning = false;
        return;
    }

    assets = AssetManager();

    text_h = screen.h / 25;
    assets.loadFont(SMALL_FONT, FONT(times.ttf), -1, text_h);
    assets.loadFont(LARGE_FONT, FONT(times.ttf), -1, screen.h / 10);

    icon_w = (int)(fmin(maxW, maxH) / 15);
}
void Game::clean() {
    mRunning = false;
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    std::cout << "Renderer Cleaned" << std::endl;
}
void Game::updateTime() {
    ts = Timestep(SDL_GetTicks() - gameTime);
    gameTime += ts.milliseconds();
}
void Game::update() {
#ifdef DEBUG_GAME
    std::cout << "Update" << std::endl;
#endif
}
void Game::handleEvents() {
#ifdef DEBUG_GAME
    std::cout << "Events" << std::endl;
#endif
    // Update buttons and mouse
    int dx, dy;
    SDL_GetMouseState(&dx, &dy);
    event.mouse = { dx, dy };
    event.mouseDx = 0; event.mouseDy = 0;
    event.update(ts);
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            mRunning = false;
            return;
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                double oldCX = screen.cX(), oldCY = screen.cY();
                screen.resize(e.window.data1, e.window.data2, false);
                return;
            }
            break;
        default:
            event.update(e);
            break;
        }
    }
    event.handled = false;
}
void Game::render() {
#ifdef DEBUG_GAME
    std::cout << "Render" << std::endl;
#endif
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void Game::setDrawColor(const SDL_Color& c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}
void Game::resetDrawColor() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}
void Game::setRenderTarget(SDL_Texture* tex) {
    if (SDL_SetRenderTarget(renderer, tex) != 0) {
        std::cout << "Unable to set render target" << std::endl;
    }
}
void Game::resetRenderTarget() {
    SDL_SetRenderTarget(renderer, NULL);
    resetDrawColor();
}
