#include "Game.h"

//#define DEBUG_GAME

Game::Game(const std::string& player, const std::string& universe) {
    std::string infoFile = UNIVERSES + universe + "/" + INFO_FILE;
    FileRead fr;
    fr.open(infoFile);
    fr.close();
    world.setFile(UNIVERSES + universe + "/world" + WORLD_EXT);
}

void Game::tick(Event& e) {
    handleEvents(e);
    update();
    render();
}
void Game::handleEvents(Event &e) {
#ifdef DEBUG_GAME
    std::cout << "Events" << std::endl;
#endif
    if (e.quit) {
        running = false;
        return;
    }
    if (e.left.pressed) {
        SDL_Point pxDim = world.getPixelDim();
        SDL_Point c = { UI::width() / 2, UI::height() / 2 };
        double mag = distance(c, e.mouse) / 2;
        double dx = (double)(e.mouse.x - c.x) / mag;
        double dy = (double)(e.mouse.y - c.y) / mag;
        playerPos.x += dx;
        playerPos.y += dy;
        if (playerPos.x < 0) { playerPos.x = 0; }
        if (playerPos.y < 0) { playerPos.y = 0; }
        if (playerPos.x > pxDim.x) { playerPos.x = pxDim.x; }
        if (playerPos.y > pxDim.y) { playerPos.y = pxDim.y; }
    }
}
void Game::update() {
#ifdef DEBUG_GAME
    std::cout << "Update" << std::endl;
#endif
}
void Game::render() {
#ifdef DEBUG_GAME
    std::cout << "Render" << std::endl;
#endif
    world.draw(playerPos);
}
