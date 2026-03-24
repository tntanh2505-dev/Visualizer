#pragma once
#include <SFML/Graphics.hpp>

class Screen {
public:
    virtual int run(sf::RenderWindow& window, sf::Font& font) = 0;
    virtual ~Screen() {}
};