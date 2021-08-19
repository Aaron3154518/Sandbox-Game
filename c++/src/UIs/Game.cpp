#include "Game.h"

//#define DEBUG_GAME

GameObjects& Game::game = GameObjects::Get();

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
    game.world().tick(e.dt);
    game.player().tick(e);
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
    game.world().draw(game.player().getCPos());
    game.player().draw();
}
