#include "DSA-Visualization/ui/Game.hpp"
#include "DSA-Visualization/ui/AVL_Screen.hpp"
#include "DSA-Visualization/ui/heap_screen.hpp"
#include "DSA-Visualization/ui/LinkedList_Screen.hpp"
#include "DSA-Visualization/ui/DijkstraScreen.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"

#include <iostream>
#include <vector>

Game::Game() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int safeWidth = static_cast<unsigned int>(desktop.width * UITheme::Window::WidthRatio);
    unsigned int safeHeight = static_cast<unsigned int>(desktop.height * UITheme::Window::HeightRatio);
    // UITheme::Window::ReferenceHeight = safeHeight;
    // UITheme::Window::ReferenceWidth = safeWidth;
    mWindow.create(sf::VideoMode(safeWidth, safeHeight), "DSA Visualizer", sf::Style::Default, settings);
    
    // 2. Use UITheme for offset centering
    mWindow.setPosition(sf::Vector2i(
        (desktop.width - safeWidth) / 2, 
        (desktop.height - safeHeight) / 2 - UITheme::Window::CenterYOffset
    ));
    
    // 3. Use UITheme for framerate
    mWindow.setFramerateLimit(UITheme::Window::Framerate);

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