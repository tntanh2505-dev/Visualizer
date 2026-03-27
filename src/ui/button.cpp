#include "DSA-Visualization/ui/button.hpp"

Button::Button(const std::string& label, const sf::Font& font,
               sf::Vector2f position, sf::Vector2f size)
{
    mBox.setPosition(position);
    mBox.setSize(size);
    mBox.setFillColor(sf::Color(70, 130, 180));
    mBox.setOutlineThickness(2.f);
    mBox.setOutlineColor(sf::Color::White);

    mText.setFont(font);
    mText.setString(label);
    mText.setCharacterSize(22);
    mText.setFillColor(sf::Color::White);

    while (mText.getLocalBounds().width > size.x - 24.f && mText.getCharacterSize() > 14) {
        mText.setCharacterSize(mText.getCharacterSize() - 1);
    }

    sf::FloatRect textBounds = mText.getLocalBounds();
    mText.setOrigin(textBounds.left + textBounds.width  / 2.f,
                    textBounds.top  + textBounds.height / 2.f);
    mText.setPosition(position.x + size.x / 2.f,
                      position.y + size.y / 2.f - 2.f);
}

void Button::setHighlight(bool highlight) {
    mBox.setFillColor(highlight
        ? sf::Color(100, 160, 210)
        : sf::Color(70, 130, 180));
}

bool Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type != sf::Event::MouseButtonReleased) return false;
    if (event.mouseButton.button != sf::Mouse::Left)  return false;

    sf::Vector2f mouse = window.mapPixelToCoords(
        sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    return mBox.getGlobalBounds().contains(mouse);
}

sf::FloatRect Button::getGlobalBounds() const {
    return mBox.getGlobalBounds();
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(mBox);
    window.draw(mText);
}
