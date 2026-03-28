#include "DSA-Visualization/ui/button.hpp"
#include <cmath>

ModernButton::ModernButton(const std::string& text, const sf::Font& font, sf::Vector2f size, float radius)
    : m_size(size), m_radius(radius), m_isHovered(false) // radius is kept in signature to match your .hpp, but unused visually
{
    // 1. Setup Theme Colors
    m_topNormal    = sf::Color(60, 60, 60);
    m_bottomNormal = sf::Color(30, 30, 30);
    m_topHover     = sf::Color(80, 80, 80);
    m_bottomHover  = sf::Color(50, 50, 50);
    m_borderColor  = sf::Color(100, 100, 100, 150);

    // 2. Setup Text properties (Scaled down for smaller button)
    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(16); // Reduced from 22
    m_text.setFillColor(sf::Color::White);
    m_text.setLetterSpacing(1.1f); 

    // 3. Generate the simple rectangle geometry
    buildGeometry();

    // 4. Center the text perfectly
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    m_text.setPosition(m_size.x / 2.0f, m_size.y / 2.0f);

    // Set origin to center for smooth scaling effects
    setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
}

void ModernButton::update(sf::Vector2f mousePos)
{
    sf::Transform transform = getTransform();
    sf::FloatRect bounds = transform.transformRect(sf::FloatRect(0, 0, m_size.x, m_size.y));

    bool wasHovered = m_isHovered;
    m_isHovered = bounds.contains(mousePos);

    if (wasHovered != m_isHovered) {
        updateColors();
        // Subtle pop effect (reduced scaling for smaller button)
        if (m_isHovered) {
            setScale(1.02f, 1.02f); 
        } else {
            setScale(1.0f, 1.0f);
        }
    }
}

bool ModernButton::isClicked(sf::Vector2f mousePos, bool mousePressed)
{
    if (m_isHovered) {
        if (mousePressed) {
            setScale(0.98f, 0.98f); // Physical press down
            return true;
        } else {
            setScale(1.02f, 1.02f); // Restore hover
        }
    }
    return false;
}

void ModernButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_background, states);
    target.draw(m_border, states);
    target.draw(m_text, states);
}

void ModernButton::buildGeometry()
{
    // Fast, simple Quad for a pure rectangle
    m_background.setPrimitiveType(sf::Quads);
    m_background.resize(4);

    m_background[0].position = sf::Vector2f(0.f, 0.f);             // Top-Left
    m_background[1].position = sf::Vector2f(m_size.x, 0.f);        // Top-Right
    m_background[2].position = sf::Vector2f(m_size.x, m_size.y);   // Bottom-Right
    m_background[3].position = sf::Vector2f(0.f, m_size.y);        // Bottom-Left

    // 5-point LineStrip to draw the outer border outline
    m_border.setPrimitiveType(sf::LineStrip);
    m_border.resize(5);
    for (int i = 0; i < 4; ++i) {
        m_border[i].position = m_background[i].position;
        m_border[i].color = m_borderColor;
    }
    m_border[4].position = m_background[0].position; // Close the loop
    m_border[4].color = m_borderColor;

    updateColors();
}

void ModernButton::updateColors()
{
    sf::Color topColor = m_isHovered ? m_topHover : m_topNormal;
    sf::Color bottomColor = m_isHovered ? m_bottomHover : m_bottomNormal;

    // Direct assignment for instant, flawless gradients
    m_background[0].color = topColor;    // Top-Left
    m_background[1].color = topColor;    // Top-Right
    m_background[2].color = bottomColor; // Bottom-Right
    m_background[3].color = bottomColor; // Bottom-Left
}

// We can keep this in case you need it later, or you can delete it from the .hpp and .cpp
sf::Color ModernButton::interpolateColor(const sf::Color& color1, const sf::Color& color2, float t) const
{
    t = std::max(0.0f, std::min(1.0f, t));
    return sf::Color(
        static_cast<sf::Uint8>(color1.r + t * (color2.r - color1.r)),
        static_cast<sf::Uint8>(color1.g + t * (color2.g - color1.g)),
        static_cast<sf::Uint8>(color1.b + t * (color2.b - color1.b)),
        static_cast<sf::Uint8>(color1.a + t * (color2.a - color1.a))
    );
}

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
