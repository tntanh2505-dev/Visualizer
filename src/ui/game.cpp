#include "DSA-Visualization/ui/game.hpp"
#include <iostream>

static const sf::Vector2f BUTTON_SIZE    = {260.f, 55.f};
static const float        BUTTON_X       = 60.f;
static const float        BUTTON_START_Y = 180.f;
static const float        BUTTON_GAP     = 80.f;

Game::Game(const sf::Font& font)
: mWindow(sf::VideoMode(1280, 720), "DSA Visualization")
, mState(AppState::MENU)
, mFont(font)
, mReturnButton("Return to Menu", mFont, {20.f, 20.f}, {200.f, 45.f})
, mHeapVisualizer(mFont)
{
    mWindow.setFramerateLimit(60);

    if (!mBgTexture.loadFromFile("assets/textures/background.png"))
        std::cerr << "Failed to load background.png\n";

    sf::Vector2u size = mBgTexture.getSize();
    mBgSprite.setTexture(mBgTexture);
    mBgSprite.setScale(1280.f / size.x, 720.f / size.y);

    std::vector<std::string> labels = {
        "Singly Linked List",
        "Heap",
        "Shortest Path",
        "AVL Tree",
        "Exit"
    };

    for (int i = 0; i < 5; i++) {
        mMenuButtons.emplace_back(
            labels[i], mFont,
            sf::Vector2f(BUTTON_X, BUTTON_START_Y + i * BUTTON_GAP),
            BUTTON_SIZE
        );
    }
}

void Game::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        const float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        if (mState == AppState::MENU) {
            processMenuEvents(event);
        } else {
            if (mReturnButton.isClicked(event, mWindow)) {
                mState = AppState::MENU;
                mHeapVisualizer.reset();
            }

            if (mState == AppState::HEAP) {
                mHeapVisualizer.handleEvent(event, mWindow);
            }
        }
    }
}

void Game::processMenuEvents(const sf::Event& event) {
    if (mMenuButtons[0].isClicked(event, mWindow)) mState = AppState::LINKED_LIST;
    if (mMenuButtons[1].isClicked(event, mWindow)) mState = AppState::HEAP;
    if (mMenuButtons[2].isClicked(event, mWindow)) mState = AppState::GRAPH;
    if (mMenuButtons[3].isClicked(event, mWindow)) mState = AppState::AVL_TREE;
    if (mMenuButtons[4].isClicked(event, mWindow)) mWindow.close();
}

void Game::update(float deltaTime) {
    if (mState == AppState::MENU) {
        sf::Vector2f mouse = mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow));
        for (auto& btn : mMenuButtons)
            btn.setHighlight(btn.getGlobalBounds().contains(mouse));
    } else if (mState == AppState::HEAP) {
        mHeapVisualizer.update(deltaTime, mWindow);
    }
}

void Game::render() {
    mWindow.clear();
    mWindow.draw(mBgSprite);

    switch (mState) {
        case AppState::MENU:        renderMenu();       break;
        case AppState::LINKED_LIST: renderLinkedList(); break;
        case AppState::HEAP:        renderHeap();       break;
        case AppState::GRAPH:       renderGraph();      break;
        case AppState::AVL_TREE:    renderAVLTree();    break;
    }

    mWindow.display();
}

void Game::renderMenu() {
    for (auto& btn : mMenuButtons)
        btn.draw(mWindow);
}

void Game::renderLinkedList() {
    mReturnButton.draw(mWindow);
}

void Game::renderHeap() {
    mReturnButton.draw(mWindow);
    mHeapVisualizer.render(mWindow);
}

void Game::renderGraph() {
    mReturnButton.draw(mWindow);
}

void Game::renderAVLTree() {
    mReturnButton.draw(mWindow);
}
