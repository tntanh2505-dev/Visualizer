#include "DSA-Visualization/ui/Game.hpp"
#include "DSA-Visualization/ui/AVL_Screen.hpp"
#include "DSA-Visualization/ui/SLL_Screen.hpp"
#include <iostream>
#include <vector>

Game::Game() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    mWindow.create(sf::VideoMode(1280, 720), "DSA Visualization", sf::Style::Default, settings);
    mWindow.setFramerateLimit(60);
    if (!mFont.loadFromFile("assets/fonts/Lexi.ttf"))
        std::cerr << "Failed to load font.ttf\n";
}

void Game::run() {
    MenuScreen menu;
   // SLLScreen  sll;
    AVLScreen  avl;
    

    std::vector<Screen*> screens = {
        &menu,   // 0
        nullptr,    // 1 LinkedList
        nullptr, // 2 Heap (later)
        nullptr, // 3 Dijkstra (later)
        &avl,    // 4
    };

    int current = 0;
    while (current >= 0 && mWindow.isOpen()) {
        if (current >= (int)screens.size() || screens[current] == nullptr) {
            current = 0;
            continue;
        }
        current = screens[current]->run(mWindow, mFont);
    }
}