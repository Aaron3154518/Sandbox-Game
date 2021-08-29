#include "Game.h"

//#define DEBUG_GAME

Game::Game(const std::string& _player, const std::string& _universe) :
    player(_player), universe(_universe) {}

void Game::initUI() {
    std::string infoFile = gameVals::univInfoFile(universe);
    FileRead fr;
    fr.open(infoFile);
    fr.close();
    GameObjects::world().setFile(gameVals::worldFile(universe, "world"));
    GameObjects::player().setFile(gameVals::playerFile(player));
}

void Game::tickUI(Event& e) {
    handleEvents(e);
    render();
}
void Game::handleEvents(Event& e) {
#ifdef DEBUG_GAME
    std::cerr << "Events" << std::endl;
#endif
    if (e.quit) {
        // TODO: Exit button
        GameObjects::world().saveWorld();
        running = false;
        return;
    }
    GameObjects::world().tick(e.dt);
    GameObjects::player().handleEvents(e);
}
void Game::render() {
#ifdef DEBUG_GAME
    std::cerr << "Render" << std::endl;
#endif
    GameObjects::world().draw(GameObjects::player().getCPos());
    GameObjects::player().draw();
}
