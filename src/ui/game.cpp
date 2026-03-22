#include "DSA-Visualization/ui/Game.hpp"
#include <iostream>

Game::Game()
: mWindow(sf::VideoMode(1280, 720), "DSA Visualization")
{
    std::cout << "Game constructor started\n";
    mWindow.setFramerateLimit(60);

    if (!mBgTexture.loadFromFile("assets/textures/background.png")) {
        std::cerr << "Failed to load background.png\n";
        std::cerr << "Current working directory issue\n";
    } else {
        std::cout << "Successfully loaded background.png\n";
    }

    sf::Vector2u size = mBgTexture.getSize();
    mBgSprite.setTexture(mBgTexture);
    mBgSprite.setScale(1280.f / size.x, 720.f / size.y);

    sf::Color colors[5] = {
        sf::Color::Red,
        sf::Color::Green,
        sf::Color::Blue,
        sf::Color::Yellow,
        sf::Color::Cyan
    };

    for (int i = 0; i < 5; i++) {
        mRects[i].setSize({100.f, 60.f});
        mRects[i].setFillColor(colors[i]);
        mRects[i].setPosition(150.f + i * 200.f, 330.f);
    }
}

void Game::run() {
    std::cout << "Run loop started, window open: " << mWindow.isOpen() << "\n";
    while (mWindow.isOpen()) {
        processEvents();
        update();
        render();
    }
    std::cout << "Run loop ended\n";
}

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();
    }
}

void Game::update() {
}

void Game::render() {
    mWindow.clear();
    mWindow.draw(mBgSprite);
    for (int i = 0; i < 5; i++)
        mWindow.draw(mRects[i]);
    mWindow.display();
}