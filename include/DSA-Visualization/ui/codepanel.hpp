#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

class CodePanel {
public:
    CodePanel();
    CodePanel(const sf::Font& font, sf::Vector2f position = {0.f, 0.f}, sf::Vector2f size = {380.f, 200.f});

    void loadSnippets(const std::map<std::string, std::vector<std::string>>& newSnippets);
    void setPosition(sf::Vector2f pos);
    void update(std::string algoName, int activeLine);
    
    void setCode(const std::vector<std::string>& lines);
    void highlight(int lineIndex);
    void draw(sf::RenderWindow& window);

private:
    void generateSyntaxHighlighting(const std::string& rawLine, int lineIndex, float yPos);

    sf::ConvexShape       mBackground;
    sf::ConvexShape       mTitleBar;
    sf::CircleShape       mMacButtons[3];
    sf::RectangleShape    mHighlightBar;
    sf::RectangleShape    mAccentBar; 
    sf::RectangleShape    mDropShadow;

    std::map<std::string, std::vector<std::string>> mSnippets;
    std::vector<sf::Text> mLineNumbers;
    std::vector<std::vector<sf::Text>> mSyntaxLines; 

    int                   mHighlightedLine;
    sf::Vector2f          mPosition;
    sf::Vector2f          mSize;
    float                 mLineHeight;
    const sf::Font* mFont;
    bool                  mIsVisible;
};