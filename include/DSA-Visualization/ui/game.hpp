#pragma once
#include <SFML/Graphics.hpp>
#include "DSA-Visualization/ui/Button.hpp"
#include <vector>

enum class AppState {
    MENU,
    LINKED_LIST,
    HEAP,
    GRAPH,
    AVL_TREE
};

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();

    void processMenuEvents(const sf::Event& event);
    void renderMenu();
    void renderLinkedList();
    void renderHeap();
    void renderGraph();
    void renderAVLTree();

    sf::RenderWindow mWindow;
    sf::Texture      mBgTexture;
    sf::Sprite       mBgSprite;
    sf::Font         mFont;
    AppState         mState;

    std::vector<Button> mMenuButtons;
    Button              mReturnButton;
};