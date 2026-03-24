#include "DSA-Visualization/ui/Game.hpp"
#include "DSA-Visualization/ui/AVL_Screen.hpp"
#include <iostream>
#include <vector>

Game::Game()
: mWindow(sf::VideoMode(1280, 720), "DSA Visualization")
{
    mWindow.setFramerateLimit(60);
    if (!mFont.loadFromFile("assets/fonts/PhongChu.ttf"))
        std::cerr << "Failed to load PhongChu.ttf\n";
}

void Game::run() {
    MenuScreen menu;
    AVLScreen  avl;

    std::vector<Screen*> screens = {
        &menu,   // 0
        nullptr, // 1 LinkedList (later)
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