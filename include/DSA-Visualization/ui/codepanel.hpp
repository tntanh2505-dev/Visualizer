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
    void generateSyntaxHighlighting(const std::string& rawLine, int lineIndex, float yPos);

    sf::ConvexShape       mBackground;
    sf::ConvexShape       mTitleBar;
    sf::CircleShape       mMacButtons[3];
    sf::RectangleShape    mHighlightBar;
    sf::RectangleShape    mDropShadow;

    // Line components
    std::vector<sf::Text> mLineNumbers;
    std::vector<std::vector<sf::Text>> mSyntaxLines; // multiple colored words per line

    int                   mHighlightedLine;
    sf::Vector2f          mPosition;
    sf::Vector2f          mSize;
    float                 mLineHeight;
    const sf::Font*       mFont;
};