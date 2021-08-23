#include "UI.h"

#include "../Utils/AssetManager.h"

bool UI::initialized = false;
int UI::maxW = 0, UI::maxH = 0, UI::w = 0, UI::h = 0;
SDL_Renderer* UI::mRenderer = NULL;
SDL_Window* UI::mWindow = NULL;
AssetManager UI::mAssetManager = AssetManager();

std::deque<std::shared_ptr<UI>> UI::activeUIs;

void UI::runUI() {
    std::cerr << "Running" << std::endl;
    if (textInput) { SDL_StartTextInput(); }
    running = true;
    initUI();
    Event e;
    while (running) {
        Timestep dt(SDL_GetTicks() - gameTime);
        gameTime += dt.milliseconds();

        e.update(dt);
        if (e.resize) {
            UI::resize(e.newW, e.newH);
        }

        SDL_RenderClear(mRenderer);
        tickUI(e);
        SDL_RenderPresent(mRenderer);

        if (dt.milliseconds() < gameVals::FRAME_DELAY()) {
            SDL_Delay(gameVals::FRAME_DELAY() - dt.milliseconds());
        }
    }
    if (textInput) { SDL_StopTextInput(); }
}

void UI::init() {
    if (initialized) { return; }
    // Create any necessary directories
    std::vector<std::string> dirs = { "saves/", "saves/players/", "saves/universes/" };
    for (auto& dir : dirs) {
        if (!isDir(dir)) { mkDir(dir.c_str()); }
    }

    // Initialize main SDL module
    int flags = SDL_WINDOW_RESIZABLE;
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cerr << "SDL Initialized" << std::endl;
        // Use opengl
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

        Rect temp;
        SDL_GetDisplayUsableBounds(0, &temp);
        maxW = temp.w; maxH = temp.h;

        w = gameVals::MIN_W(); h = gameVals::MIN_H();
        mWindow = SDL_CreateWindow("Sandbox Game", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, w, h, flags);
        std::cerr << "Window Dimensions: " << w << " X " << h << std::endl;

        mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
        if (renderer) {
            resetDrawColor();
            resetRenderTarget();
            resetRenderBlendMode();
        }
    }

    // Initialize SDL fonts module
    if (TTF_Init() == 0) {
        std::cerr << "SDL_TTF Initialized" << std::endl;
        UI::mAssetManager.loadFont(gameVals::LARGE_FONT(),
            gameVals::fontFile(), -1, gameVals::MIN_H() / 10);
        UI::mAssetManager.loadFont(gameVals::SMALL_FONT(),
            gameVals::fontFile(), -1, gameVals::MIN_H() / 25);
        UI::mAssetManager.loadFont(gameVals::INV_FONT(), gameVals::fontFile(),
            gameVals::INV_FONT_W(), gameVals::INV_FONT_W());
    }

    initialized = true;
}

void UI::clean() {
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(mRenderer);
    SDL_Quit();
    initialized = false;
    std::cerr << "SDL Uninitialized" << std::endl;
}

void UI::run() {
    while (!UI::activeUIs.empty()) {
        // Add current
        std::shared_ptr<UI> current = activeUIs.front();
        activeUIs.pop_front();

        if (!current) { std::cerr << "Null UI" << std::endl; continue; }
        // Run current
        current->runUI();

        // Add any new UIs
        for (auto it = current->nextUIs.end(); it != current->nextUIs.begin();) {
            activeUIs.push_front(std::move(*(--it)));
        }
        current->nextUIs.clear();
        if (current->subUIs.size() > 0) { activeUIs.push_front(current); }
        for (auto it = current->subUIs.end(); it != current->subUIs.begin();) {
            activeUIs.push_front(std::move(*(--it)));
        }
        current->subUIs.clear();
        current.reset();
    }
}

int UI::width() { return w; }
int UI::height() { return h; }
SDL_Renderer* UI::renderer() { return mRenderer; }
AssetManager& UI::assets() { return mAssetManager; }
SDL_Point UI::mouse() {
    SDL_Point p;
    SDL_GetMouseState(&p.x, &p.y);
    return p;
}

void UI::setDrawColor(const SDL_Color& c) {
    SDL_SetRenderDrawColor(mRenderer, c.r, c.g, c.b, c.a);
}
void UI::resetDrawColor() {
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
}
void UI::setRenderTarget(SDL_Texture* tex) {
    if (SDL_SetRenderTarget(mRenderer, tex) != 0) {
        std::cerr << "Unable to set render target" << std::endl;
    }
}
void UI::resetRenderTarget() {
    SDL_SetRenderTarget(mRenderer, NULL);
    resetDrawColor();
}
void UI::setRenderBlendMode(SDL_BlendMode mode) {
    SDL_SetRenderDrawBlendMode(mRenderer, mode);
}
void UI::resetRenderBlendMode() {
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
}