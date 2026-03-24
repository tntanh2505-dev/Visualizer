#include "DSA-Visualization/ui/MenuScreen.hpp"
#include <iostream>

static const sf::Vector2f BUTTON_SIZE    = {260.f, 55.f};
static const float        BUTTON_X       = 60.f;
static const float        BUTTON_START_Y = 180.f;
static const float        BUTTON_GAP     = 80.f;

MenuScreen::MenuScreen() {}

void MenuScreen::buildButtons(const sf::Font& font) {
    mButtons.clear();
    std::vector<std::string> labels = {
        "Singly Linked List",
        "Heap",
        "Shortest Path",
        "AVL Tree",
        "Exit"
    };
    for (int i = 0; i < (int)labels.size(); i++) {
        mButtons.emplace_back(
            labels[i], font,
            sf::Vector2f(BUTTON_X, BUTTON_START_Y + i * BUTTON_GAP),
            BUTTON_SIZE
        );
    }
}

int MenuScreen::run(sf::RenderWindow& window, sf::Font& font) {
    if (!mBgTexture.loadFromFile("assets/textures/background.png"))
        std::cerr << "Failed to load background.png\n";

    sf::Vector2u size = mBgTexture.getSize();
    mBgSprite.setTexture(mBgTexture);
    mBgSprite.setScale(1280.f / size.x, 720.f / size.y);

    buildButtons(font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }

            if (mButtons[0].isClicked(event, window)) return 1; // LinkedList
            if (mButtons[1].isClicked(event, window)) return 2; // Heap
            if (mButtons[2].isClicked(event, window)) return 3; // Graph
            if (mButtons[3].isClicked(event, window)) return 4; // AVL
            if (mButtons[4].isClicked(event, window)) return -1; // Exit
        }

        // Hover
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (auto& btn : mButtons)
            btn.setHighlight(btn.getGlobalBounds().contains(mouse));

        window.clear();
        window.draw(mBgSprite);
        for (auto& btn : mButtons)
            btn.draw(window);
        window.display();
    }
    return -1;
}