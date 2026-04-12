#include "DSA-Visualization/ui/Game.hpp"
#include "DSA-Visualization/ui/DijkstraScreen.hpp"
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
    DijkstraScreen graph;

    std::vector<Screen*> screens = {
        &menu,   // 0
        nullptr,    // 1 LinkedList
        nullptr,    // 2 Heap (later)
        &graph,     // 3 Dijkstra (later)
        nullptr,    // 4 AVL
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