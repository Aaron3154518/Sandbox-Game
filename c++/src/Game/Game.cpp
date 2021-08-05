#include "Game.h"

//#define DEBUG_GAME

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
    td.x = td.w = UI::width() / 2;
    td.y = td.h = UI::height() / 2;
    UI::assets().drawText(td, NULL);
}
