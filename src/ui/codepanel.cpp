#include "DSA-Visualization/ui/CodePanel.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"
#include <cmath>
#include <sstream>

static const float PADDING     = 12.f;
static const float LINE_HEIGHT = 22.f;
static const int   FONT_SIZE   = UITheme::Size::FontCode;



CodePanel::CodePanel()
: mFont(nullptr)
, mHighlightedLine(-1)
, mLineHeight(LINE_HEIGHT)
{}

static void buildRoundedRect(sf::ConvexShape& shape, sf::Vector2f size, float radius) {
    shape.setPointCount(40);
    const float pi = 3.141592654f;
    for (int i = 0; i < 10; ++i) {
        float step = i * (pi / 2.f) / 9.f;
        shape.setPoint(i, sf::Vector2f(size.x - radius + radius * std::cos(step), radius - radius * std::sin(step)));
        shape.setPoint(10 + i, sf::Vector2f(radius + radius * std::cos(step + pi / 2.f), radius - radius * std::sin(step + pi / 2.f)));
        shape.setPoint(20 + i, sf::Vector2f(radius + radius * std::cos(step + pi), size.y - radius - radius * std::sin(step + pi)));
        shape.setPoint(30 + i, sf::Vector2f(size.x - radius + radius * std::cos(step + 3.f * pi / 2.f), size.y - radius - radius * std::sin(step + 3.f * pi / 2.f)));
    }
}

static void buildTopRoundedRect(sf::ConvexShape& shape, sf::Vector2f size, float radius) {
    shape.setPointCount(22);
    const float pi = 3.141592654f;
    for (int i = 0; i < 10; ++i) {
        float step = i * (pi / 2.f) / 9.f;
        shape.setPoint(i, sf::Vector2f(size.x - radius + radius * std::cos(step), radius - radius * std::sin(step)));
        shape.setPoint(10 + i, sf::Vector2f(radius + radius * std::cos(step + pi / 2.f), radius - radius * std::sin(step + pi / 2.f)));
    }
    shape.setPoint(20, sf::Vector2f(0.f, size.y));
    shape.setPoint(21, sf::Vector2f(size.x, size.y));
}
CodePanel::CodePanel(const sf::Font& font, sf::Vector2f position, sf::Vector2f size)
: mFont(&font), mPosition(position), mSize(size), mHighlightedLine(-1), mLineHeight(LINE_HEIGHT)
{
    float radius = 10.f;

    mDropShadow.setSize(size);
    mDropShadow.setPosition(position + sf::Vector2f(6.f, 8.f));
    mDropShadow.setFillColor(sf::Color(0, 0, 0, 100)); 

    buildRoundedRect(mBackground, size, radius);
    mBackground.setPosition(position);
    // --- Standardized UITheme Elements ---
    mBackground.setFillColor(UITheme::Color::CodePanelBg);
    mBackground.setOutlineThickness(1.5f);
    mBackground.setOutlineColor(UITheme::Color::CodePanelBorder);

    float titleHeight = 24.f;
    buildTopRoundedRect(mTitleBar, sf::Vector2f(size.x, titleHeight), radius);
    mTitleBar.setPosition(position);
    mTitleBar.setFillColor(UITheme::Color::CodeTitleBar);

    sf::Color macColors[3] = { sf::Color(255, 95, 86), sf::Color(255, 189, 46), sf::Color(39, 201, 63) };
    for (int i = 0; i < 3; ++i) {
        mMacButtons[i].setRadius(5.f);
        mMacButtons[i].setFillColor(macColors[i]);
        mMacButtons[i].setPosition(position.x + 12.f + i * 16.f, position.y + 7.f);
    }

    mHighlightBar.setSize({size.x, LINE_HEIGHT});
    mHighlightBar.setFillColor(UITheme::Color::CodeHighlight);
}    
void CodePanel::generateSyntaxHighlighting(const std::string& rawLine, int lineIndex, float yPos) {
    std::string token;
    std::stringstream ss(rawLine);
    
    float currentX = mPosition.x + PADDING + 30.f; 
    int indentSpaces = 0;
    while(indentSpaces < (int)rawLine.size() && rawLine[indentSpaces] == ' ') {
        indentSpaces++;
    }
    
    std::vector<sf::Text> parsedLine;
    sf::Text indentText;
    // ... [indentText init] ...

    std::stringstream tokenStream(rawLine.substr(indentSpaces));
    while (tokenStream >> token) {
        sf::Color color = UITheme::Color::CodeTextDefault;
        
        if (token == "if" || token == "else" || token == "return" || token == "else:" || token == "void") {
            color = UITheme::Color::CodeKeyword;
        } else if (token == "node" || token == "null" || token == "null:" || token == "left" || token == "right") {
            color = UITheme::Color::CodeType;
        } else if (token.find("(") != std::string::npos || token.find(")") != std::string::npos) {
            color = UITheme::Color::CodeFunction;
        }

        sf::Text word;
        word.setFont(*mFont);
        word.setString(token + " ");
        word.setCharacterSize(FONT_SIZE);
        word.setFillColor(color);
        word.setLetterSpacing(1.1f);
        word.setPosition(currentX, yPos);
        
        currentX += word.getLocalBounds().width;
        parsedLine.push_back(word);
    }
    mSyntaxLines.push_back(parsedLine);
}

void CodePanel::setCode(const std::vector<std::string>& lines) {
    if (!mFont) return;
    
    mLineNumbers.clear();
    mSyntaxLines.clear();

    float topMargin = PADDING + 24.f;

    for (int i = 0; i < (int)lines.size(); i++) {
        float yPos = mPosition.y + topMargin + i * LINE_HEIGHT;
        
        sf::Text numText;
        numText.setFont(*mFont);
        numText.setString(std::to_string(i + 1));
        numText.setCharacterSize(FONT_SIZE - 1);
        numText.setFillColor(UITheme::Color::CodeLineNum);
        numText.setPosition(mPosition.x + 10.f, yPos);
        mLineNumbers.push_back(numText);

        generateSyntaxHighlighting(lines[i], i, yPos);
    }
}

void CodePanel::highlight(int lineIndex) {
    mHighlightedLine = lineIndex;
    if (lineIndex >= 0 && lineIndex < (int)mLineNumbers.size()) {
        float topMargin = PADDING + 24.f;
        mHighlightBar.setPosition(
            mPosition.x,
            mPosition.y + topMargin + lineIndex * LINE_HEIGHT - 2.f
        );
    }
}

void CodePanel::draw(sf::RenderWindow& window) {
    window.draw(mDropShadow);
    window.draw(mBackground);
    window.draw(mTitleBar);
    for (int i = 0; i < 3; ++i) window.draw(mMacButtons[i]);

    if (mHighlightedLine >= 0 && mHighlightedLine < (int)mLineNumbers.size()) {
        window.draw(mHighlightBar);
        sf::RectangleShape accent({4.f, mLineHeight});
        accent.setPosition(mHighlightBar.getPosition());
        accent.setFillColor(UITheme::Color::CodeAccent);
        window.draw(accent);
    }

    for (const auto& num : mLineNumbers) window.draw(num);

    for (const auto& lineStruct : mSyntaxLines) {
        for (const auto& word : lineStruct) {
            window.draw(word);
        }
    }
}