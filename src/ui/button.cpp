#include "DSA-Visualization/ui/button.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"
#include <cmath>

ModernButton::ModernButton(const std::string& text, const sf::Font& font, sf::Vector2f size, float radius)
    : m_size(size), m_radius(radius), m_isHovered(false) 
{
    m_topNormal    = UITheme::Color::ModernBtnTop;
    m_bottomNormal = UITheme::Color::ModernBtnBottom;
    m_topHover     = UITheme::Color::ModernBtnHoverT;
    m_bottomHover  = UITheme::Color::ModernBtnHoverB;
    m_borderColor  = UITheme::Color::ModernBtnBorder;

    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(UITheme::Size::FontNormal - 2); 
    m_text.setFillColor(UITheme::Color::TextWhite);
    m_text.setLetterSpacing(1.1f);

    buildGeometry();

    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    m_text.setPosition(m_size.x / 2.0f, m_size.y / 2.0f);
    setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
}

void ModernButton::update(sf::Vector2f mousePos) {
    sf::Transform transform = getTransform();
    sf::FloatRect bounds = transform.transformRect(sf::FloatRect(0, 0, m_size.x, m_size.y));

    bool wasHovered = m_isHovered;
    m_isHovered = bounds.contains(mousePos);

    if (wasHovered != m_isHovered) {
        updateColors();
        if (m_isHovered) setScale(1.02f, 1.02f); 
        else setScale(1.0f, 1.0f);
    }
}

bool ModernButton::isClicked(sf::Vector2f mousePos, bool mousePressed) {
    if (m_isHovered) {
        if (mousePressed) {
            setScale(0.98f, 0.98f); 
            return true;
        } else {
            setScale(1.02f, 1.02f); 
        }
    }
    return false;
}

void ModernButton::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    
    // --- NEW: Render True Glow for ModernButton ---
    if (m_isHovered) {
        sf::RectangleShape glow(m_size);
        glow.setFillColor(sf::Color::Transparent);
        glow.setOutlineThickness(3.5f);
        glow.setOutlineColor(UITheme::Color::AVLGlowStrong);
        target.draw(glow, states);
    }

    target.draw(m_background, states);
    target.draw(m_border, states);
    target.draw(m_text, states);
}

void ModernButton::buildGeometry() {
    m_background.setPrimitiveType(sf::Quads);
    m_background.resize(4);
    m_background[0].position = sf::Vector2f(0.f, 0.f);             
    m_background[1].position = sf::Vector2f(m_size.x, 0.f);        
    m_background[2].position = sf::Vector2f(m_size.x, m_size.y);   
    m_background[3].position = sf::Vector2f(0.f, m_size.y);        

    m_border.setPrimitiveType(sf::LineStrip);
    m_border.resize(5);
    for (int i = 0; i < 4; ++i) {
        m_border[i].position = m_background[i].position;
        m_border[i].color = m_borderColor;
    }
    m_border[4].position = m_background[0].position; 
    m_border[4].color = m_borderColor;

    updateColors();
}

void ModernButton::updateColors() {
    sf::Color topColor = m_isHovered ? m_topHover : m_topNormal;
    sf::Color bottomColor = m_isHovered ? m_bottomHover : m_bottomNormal;
    m_background[0].color = topColor;    
    m_background[1].color = topColor;    
    m_background[2].color = bottomColor; 
    m_background[3].color = bottomColor; 
}

sf::Color ModernButton::interpolateColor(const sf::Color& color1, const sf::Color& color2, float t) const {
    t = std::max(0.0f, std::min(1.0f, t));
    return sf::Color(
        static_cast<sf::Uint8>(color1.r + t * (color2.r - color1.r)),
        static_cast<sf::Uint8>(color1.g + t * (color2.g - color1.g)),
        static_cast<sf::Uint8>(color1.b + t * (color2.b - color1.b)),
        static_cast<sf::Uint8>(color1.a + t * (color2.a - color1.a))
    );
}

// ----------------------------------------------------------------------
// STANDARD BUTTON 
// ----------------------------------------------------------------------

Button::Button(const std::string& label, const sf::Font& font,
               sf::Vector2f position, sf::Vector2f size)
{
    mIsHovered = false; // Initialize
    
    mBox.setSize(size);
    mBox.setOrigin(size.x / 2.f, size.y / 2.f); 
    mBox.setPosition(position.x + size.x / 2.f, position.y + size.y / 2.f);
    
    mBox.setFillColor(UITheme::Color::ButtonPrimary);
    mBox.setOutlineThickness(1.5f);
    mBox.setOutlineColor(UITheme::Color::ModernBtnBorder);

    mText.setFont(font);
    mText.setString(label);
    mText.setCharacterSize(14); 
    mText.setFillColor(UITheme::Color::TextWhite);

    sf::FloatRect textBounds = mText.getLocalBounds();
    mText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    mText.setPosition(position.x + size.x / 2.f, position.y + size.y / 2.f - 2.f);
}

void Button::setPosition(sf::Vector2f pos) {
    mBox.setPosition(pos.x + mBox.getSize().x / 2.f, pos.y + mBox.getSize().y / 2.f);
    mText.setPosition(pos.x + mBox.getSize().x / 2.f, pos.y + mBox.getSize().y / 2.f - 2.f);
}

void Button::setText(const std::string& text) {
    mText.setString(text);
    sf::FloatRect textBounds = mText.getLocalBounds();
    mText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    mText.setPosition(mBox.getPosition().x, mBox.getPosition().y - 2.f);
}

void Button::setHighlight(bool highlight) {
    mIsHovered = highlight; // Save state for draw()
    
    mBox.setFillColor(highlight ? UITheme::Color::ModernBtnHoverT : UITheme::Color::ButtonPrimary);
    mBox.setOutlineColor(highlight ? UITheme::Color::ButtonHoverBorder : UITheme::Color::ModernBtnBorder);
    
    if (highlight) {
        mBox.setScale(1.02f, 1.02f);
        mText.setScale(1.02f, 1.02f);
    } else {
        mBox.setScale(1.0f, 1.0f);
        mText.setScale(1.0f, 1.0f);
    }
}

bool Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) {
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (mBox.getGlobalBounds().contains(mouse)) {
            mBox.setScale(0.98f, 0.98f);
            mText.setScale(0.98f, 0.98f);
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        mBox.setScale(1.0f, 1.0f);
        mText.setScale(1.0f, 1.0f);
        return mBox.getGlobalBounds().contains(mouse);
    }
    return false;
}

sf::FloatRect Button::getGlobalBounds() const {
    return mBox.getGlobalBounds();
}

void Button::draw(sf::RenderWindow& window) const {
    // --- NEW: Render True Glow for Standard Button ---
    if (mIsHovered) {
        sf::RectangleShape glow = mBox;
        glow.setFillColor(sf::Color::Transparent);
        glow.setOutlineThickness(mBox.getOutlineThickness() + 3.5f);
        glow.setOutlineColor(UITheme::Color::AVLGlowStrong);
        window.draw(glow);
    }
    
    window.draw(mBox);
    window.draw(mText);
}