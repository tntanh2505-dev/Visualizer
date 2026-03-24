#include "DSA-Visualization/ui/MenuScreen.hpp"
#include <iostream>

static const sf::Vector2f BUTTON_SIZE    = {260.f, 55.f};
static const float        BUTTON_X       = 200.f;
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
        ModernButton btn(labels[i], font, BUTTON_SIZE);
        btn.setPosition(BUTTON_X, BUTTON_START_Y + i * BUTTON_GAP);
        mButtons.push_back(btn);
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
        sf::Vector2f mouseRaw = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (auto& btn : mButtons) btn.update(mouseRaw);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }

            bool leftPressed = (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);
            if (leftPressed) {
                if (mButtons[0].isClicked(mouseRaw, true)) return 1; // LinkedList
                if (mButtons[1].isClicked(mouseRaw, true)) return 2; // Heap
                if (mButtons[2].isClicked(mouseRaw, true)) return 3; // Graph
                if (mButtons[3].isClicked(mouseRaw, true)) return 4; // AVL
                if (mButtons[4].isClicked(mouseRaw, true)) return -1; // Exit
            }
        }

        window.clear();
        window.draw(mBgSprite);

        // Dynamic Title Render
        sf::Text title;
        title.setFont(font);
        title.setString("ALGORITHM VISUALIZER");
        title.setCharacterSize(64);
        title.setLetterSpacing(1.2f);
        title.setFillColor(sf::Color::White);
        title.setPosition(200.f, 60.f); 
        
        sf::Text titleShadow = title;
        titleShadow.setFillColor(sf::Color(0, 0, 0, 180));
        titleShadow.move(4.f, 4.f);
        
        window.draw(titleShadow);
        window.draw(title);

        for (auto& btn : mButtons) window.draw(btn);
        window.display();
    }
    return -1;
}