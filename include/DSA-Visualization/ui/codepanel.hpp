#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class CodePanel {
public:
    CodePanel();
    CodePanel(const sf::Font& font, sf::Vector2f position, sf::Vector2f size);

    void setCode(const std::vector<std::string>& lines);
    void highlight(int lineIndex);
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape    mBackground;
    sf::RectangleShape    mHighlightBar;
    std::vector<sf::Text> mLines;
    int                   mHighlightedLine;
    sf::Vector2f          mPosition;
    float                 mLineHeight;
    const sf::Font*       mFont;
};