#include "DSA-Visualization/ui/CodePanel.hpp"

static const float PADDING     = 12.f;
static const float LINE_HEIGHT = 22.f;
static const int   FONT_SIZE   = 12;

CodePanel::CodePanel()
: mFont(nullptr)
, mHighlightedLine(-1)
, mLineHeight(LINE_HEIGHT)
{}

CodePanel::CodePanel(const sf::Font& font, sf::Vector2f position, sf::Vector2f size)
: mFont(&font)
, mPosition(position)
, mHighlightedLine(-1)
, mLineHeight(LINE_HEIGHT)
{
    mBackground.setPosition(position);
    mBackground.setSize(size);
    mBackground.setFillColor(sf::Color(16, 13, 20, 240));
    mBackground.setOutlineThickness(1.5f);
    mBackground.setOutlineColor(sf::Color(181, 58, 199, 60));

    mHighlightBar.setSize({size.x, LINE_HEIGHT});
    mHighlightBar.setFillColor(sf::Color(181, 58, 199, 30));
}

void CodePanel::setCode(const std::vector<std::string>& lines) {
    if (!mFont) return;
    mLines.clear();
    for (int i = 0; i < (int)lines.size(); i++) {
        sf::Text text;
        text.setFont(*mFont);
        text.setString(lines[i]);
        text.setCharacterSize(FONT_SIZE);
        text.setFillColor(sf::Color(230, 235, 240));
        text.setLetterSpacing(1.1f);
        text.setPosition(
            mPosition.x + PADDING,
            mPosition.y + PADDING + i * LINE_HEIGHT
        );
        mLines.push_back(text);
    }
}

void CodePanel::highlight(int lineIndex) {
    mHighlightedLine = lineIndex;
    if (lineIndex >= 0 && lineIndex < (int)mLines.size()) {
        mHighlightBar.setPosition(
            mPosition.x,
            mPosition.y + PADDING + lineIndex * LINE_HEIGHT - 2.f
        );
    }
}

void CodePanel::draw(sf::RenderWindow& window) {
    window.draw(mBackground);
    if (mHighlightedLine >= 0 && mHighlightedLine < (int)mLines.size()) {
        window.draw(mHighlightBar);
        sf::RectangleShape accent({4.f, mLineHeight});
        accent.setPosition(mHighlightBar.getPosition());
        accent.setFillColor(sf::Color(33, 238, 252));
        window.draw(accent);
    }
    for (auto& line : mLines)
        window.draw(line);
}