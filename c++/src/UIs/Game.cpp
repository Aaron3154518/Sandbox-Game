#include "Game.h"

//#define DEBUG_GAME

GameInterface& Game::game = GameInterface::Get();

Game::Game(const std::string& _player, const std::string& _universe) :
    player(_player), universe(_universe) {}

void Game::initUI() {
    std::string infoFile = gameVals::univInfoFile(universe);
    FileRead fr;
    fr.open(infoFile);
    fr.close();
    game.world().setFile(gameVals::worldFile(universe, "world"));
}

void Game::tickUI(Event& e) {
    handleEvents(e);
    update();
    render();
}
void Game::handleEvents(Event& e) {
#ifdef DEBUG_GAME
    std::cout << "Events" << std::endl;
#endif
    if (e.quit) {
        running = false;
        return;
    }
    auto dt = e.dt.milliseconds() / 5;
    if (e.keyDown(SDLK_a)) { playerPos.x -= dt; }
    if (e.keyDown(SDLK_d)) { playerPos.x += dt; }
    if (e.keyDown(SDLK_w)) { playerPos.y -= dt; }
    if (e.keyDown(SDLK_s)) { playerPos.y += dt; }
    SDL_Point pxDim = game.world().getPixelDim();
    if (playerPos.x < 0) { playerPos.x = 0; }
    if (playerPos.y < 0) { playerPos.y = 0; }
    if (playerPos.x > pxDim.x) { playerPos.x = pxDim.x; }
    if (playerPos.y > pxDim.y) { playerPos.y = pxDim.y; }
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
    game.world().draw(playerPos);
}
