#include "DSA-Visualization/ui/Game.hpp"
#include "DSA-Visualization/ui/AVL_Screen.hpp"
#include "DSA-Visualization/ui/heap_screen.hpp"
#include "DSA-Visualization/ui/LinkedList_Screen.hpp"
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
   // SLLScreen  sll;
    AVLScreen  avl;
    HeapVisualizer heap(mFont);
    LL_screen linkedlist;
    DijkstraScreen graph;

    std::vector<Screen*> screens = {
        &menu,   // 0
        &linkedlist,    // 1 LinkedList
        &heap, // 2 Heap (later)
        &graph, // 3 Dijkstra (later)
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