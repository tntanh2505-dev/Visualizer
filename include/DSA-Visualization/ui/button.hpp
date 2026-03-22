#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
    Button(const std::string& label, const sf::Font& font,
           sf::Vector2f position, sf::Vector2f size);

    void draw(sf::RenderWindow& window);
    bool isClicked(const sf::Event& event, const sf::RenderWindow& window);
    void setHighlight(bool highlight);
    sf::FloatRect getGlobalBounds() const;

private:
    sf::RectangleShape mBox;
    sf::Text           mText;
};