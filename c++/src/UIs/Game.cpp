#include "Game.h"

//#define DEBUG_GAME

Game::Game(const std::string& player, const std::string& universe) {
    std::cerr << "Player: " << player << "\nUniverse: " << universe << std::endl;
    std::vector<std::string> vec(4);
    size_t l;
    std::string s;
    int i;
    std::cerr << "Info File: " << createFile(UNIVERSES, universe, "/info.txt") << std::endl;
    std::fstream file(createFile(UNIVERSES, universe, "/info.txt").c_str(),
        std::ios_base::in | std::ios_base::binary);
    //readFile(file, &s);
    for (int i = 0; i < vec.size(); i++) {
        //readFile(file, &vec[i]);
    }
    //readFile(file, &i);
    file.read(&l, sizeof(size_t));
    file.read(&s, l);
    file.read(&i, sizeof(int));
    file.close();
    //for (std::string s : vec) { std::cerr << s << ", "; } std::cerr << std::endl;
    std::cerr << s << std::endl;
    std::cerr << i << std::endl;
    //world.setFile(UNIVERSES + universe + "/world" + WORLD_EXT);
    std::cerr << "All Done" << std::endl;
}

void Game::tick(Event& e) {
    std::cerr << "Tick" << std::endl;
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
