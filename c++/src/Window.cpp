#include "Window.h"

Window& Window::Get() {
    static Window WINDOW;
    return WINDOW;
}

Window::Window() {
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

        mWindow.reset(SDL_CreateWindow("Sandbox Game",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            gameVals::MIN_W(), gameVals::MIN_H(), flags));

        mAssetManager.initAssets(mWindow.get());
    }

    // Initialize SDL fonts module
    if (TTF_Init() == 0) {
        mAssetManager.initFonts();
        std::cerr << "SDL_TTF Initialized" << std::endl;
    }
}

Window::~Window() {
    mAssetManager.quit();
    mWindow.reset();
    TTF_Quit();
    std::cerr << "SDL_TTF Uninitialized" << std::endl;
    SDL_Quit();
    std::cerr << "SDL Uninitialized" << std::endl;
}

void Window::run() {
    while (!activeUIs.empty()) {
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

SDL_Point Window::screenDim() const {
    return mAssetManager.getTargetSize();
}
SDL_Renderer* Window::renderer() { return mAssetManager.renderer(); }
AssetManager& Window::assets() { return mAssetManager; }