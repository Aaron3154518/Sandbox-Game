#include "Game.h"

//#define DEBUG_GAME

Game::GameData Game::data;

// GameData
void Game::GameData::init(const std::string& pFile, const std::string& uFile) {
    // Read info file
    FileRead fr;
    fr.open(gameVals::univInfoFile(uFile));
    fr.close();
    // Load world
    world.setFile(gameVals::worldFile(uFile, "world"));
    // Load player
    player.setFile(gameVals::playerFile(pFile));
}

// Game
PlayerAccess& Game::Player() {
    return data.player;
}

WorldAccess& Game::World() {
    return data.world;
}

Game::Game(const std::string& _player, const std::string& _universe) :
    player(_player), universe(_universe) {}

void Game::initUI() {
    data.init(player, universe);
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
        data.world.saveWorld();
        running = false;
        return;
    }
    data.world.tick(e.dt);
    data.player.handleEvents(e);
}
void Game::render() {
#ifdef DEBUG_GAME
    std::cerr << "Render" << std::endl;
#endif
    data.world.draw(data.player.getCPos());
    data.player.draw();
}