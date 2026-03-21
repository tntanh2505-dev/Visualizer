#pragma once
#include <SFML/Graphics.hpp>

class App
{
public:
    App();
    void run();

private:
    sf::RenderWindow    m_window;
    sf::Texture         m_bgTexture;
    sf::Sprite          m_bgSprite;
    sf::RectangleShape  m_rect;
};