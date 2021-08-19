#include "Game.h"
#include "../World/World.h"
#include "../Player/Player.h"

//#define DEBUG_GAME

Game::Game(const std::string& _player, const std::string& _universe) :
    player(_player), universe(_universe) {}

void Game::initUI() {
    std::string infoFile = gameVals::univInfoFile(universe);
    FileRead fr;
    fr.open(infoFile);
    fr.close();
    GameObjects::world().setFile(gameVals::worldFile(universe, "world"));
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
    GameObjects::world().tick(e.dt);
    GameObjects::player().tick(e);
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
    GameObjects::world().draw(GameObjects::player().getCPos());
    GameObjects::player().draw();
}
