#include "Game.h"

//#define DEBUG_GAME

Game::Game(const std::string& player, const std::string& universe) {
    std::cerr << "Player: " << player << "\nUniverse: " << universe << std::endl;
    std::cerr << "Info File: " << createFile(UNIVERSES, universe, "/info.txt") << std::endl;
    std::fstream file(createFile(UNIVERSES, universe, "/info.txt").c_str(),
        std::ios_base::in | std::ios_base::binary);
    file.close();
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