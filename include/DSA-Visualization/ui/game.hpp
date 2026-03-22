#pragma once
#include <SFML/Graphics.hpp>

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();

    sf::RenderWindow    mWindow;
    sf::Texture         mBgTexture;
    sf::Sprite          mBgSprite;
    sf::RectangleShape  mRects[5];
};