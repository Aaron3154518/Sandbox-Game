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
}
