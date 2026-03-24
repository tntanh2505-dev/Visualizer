#include "DSA-Visualization/ui/Button.hpp"
#include <cmath>

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

    sf::FloatRect textBounds = mText.getLocalBounds();
    mText.setOrigin(textBounds.left + textBounds.width  / 2.f,
                    textBounds.top  + textBounds.height / 2.f);
    mText.setPosition(position.x + size.x / 2.f,
                      position.y + size.y / 2.f);
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

void Button::draw(sf::RenderWindow& window) {
    window.draw(mBox);
    window.draw(mText);
}


ModernButton::ModernButton(const std::string& text, const sf::Font& font, sf::Vector2f size, float radius)
    : m_size(size), m_radius(radius), m_isHovered(false) 
{
    // Define our "Glassmorphism" Purple Gradient theme
    m_topNormal = sf::Color(140, 80, 255);    // Lighter top
    m_bottomNormal = sf::Color(90, 30, 230);  // Darker bottom
    m_topHover = sf::Color(160, 100, 255);    // Brighter on hover
    m_bottomHover = sf::Color(110, 50, 250);
    m_borderColor = sf::Color(255, 255, 255, 60); // Semi-transparent white

    buildGeometry();

    // Setup Text
    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(22);
    m_text.setFillColor(sf::Color::White);
    
    // Center text nicely in the left partition
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    m_text.setPosition(m_size.x * 0.45f, m_size.y / 2.0f);

    // Setup Vertical Divider
    m_divider.setSize({1.5f, m_size.y * 0.6f});
    m_divider.setFillColor(sf::Color(255, 255, 255, 40));
    m_divider.setOrigin(0.75f, m_divider.getSize().y / 2.0f);
    m_divider.setPosition(m_size.x * 0.75f, m_size.y / 2.0f);

    // Setup Dropdown Arrow
    m_arrow.setPointCount(3);
    m_arrow.setPoint(0, {0.f, 0.f});
    m_arrow.setPoint(1, {12.f, 0.f});
    m_arrow.setPoint(2, {6.f, 7.f});
    m_arrow.setFillColor(sf::Color::White);
    m_arrow.setOrigin(6.f, 3.5f);
    m_arrow.setPosition(m_size.x * 0.88f, m_size.y / 2.0f);
}

void ModernButton::buildGeometry() {
    int cornerPoints = 15; // Smoothness of corners
    const float pi = 3.141592654f;
    
    m_background.setPrimitiveType(sf::TriangleFan);
    m_border.setPrimitiveType(sf::LineStrip);

    // Center point of the Triangle Fan
    m_background.append(sf::Vertex({m_size.x / 2.f, m_size.y / 2.f}, interpolateColor(m_topNormal, m_bottomNormal, 0.5f)));

    // Calculate perimeter points
    for (int i = 0; i < 4; ++i) {
        sf::Vector2f center;
        float startAngle;

        // Determine which corner we are drawing
        switch (i) {
            case 0: center = {m_size.x - m_radius, m_size.y - m_radius}; startAngle = 0.f; break; // Bottom Right
            case 1: center = {m_radius, m_size.y - m_radius}; startAngle = 90.f; break;           // Bottom Left
            case 2: center = {m_radius, m_radius}; startAngle = 180.f; break;                     // Top Left
            case 3: center = {m_size.x - m_radius, m_radius}; startAngle = 270.f; break;          // Top Right
        }

        for (int j = 0; j <= cornerPoints; ++j) {
            float angle = (startAngle + (90.f * j / cornerPoints)) * pi / 180.f;
            sf::Vector2f point(center.x + m_radius * std::cos(angle), center.y + m_radius * std::sin(angle));
            
            // Calculate gradient color based on Y position (0 to m_size.y)
            float ratio = point.y / m_size.y;
            sf::Color vertexColor = interpolateColor(m_topNormal, m_bottomNormal, ratio);

            m_background.append(sf::Vertex(point, vertexColor));
            m_border.append(sf::Vertex(point, m_borderColor));
        }
    }

    // Close the shapes
    m_background.append(m_background[1]); 
    m_border.append(m_border[0]);
}

void ModernButton::updateColors() {
    sf::Color top = m_isHovered ? m_topHover : m_topNormal;
    sf::Color bottom = m_isHovered ? m_bottomHover : m_bottomNormal;

    // Update the center vertex
    m_background[0].color = interpolateColor(top, bottom, 0.5f);

    // Update perimeter vertices
    for (size_t i = 1; i < m_background.getVertexCount(); ++i) {
        float ratio = m_background[i].position.y / m_size.y;
        m_background[i].color = interpolateColor(top, bottom, ratio);
    }
}

sf::Color ModernButton::interpolateColor(const sf::Color& c1, const sf::Color& c2, float t) const {
    return sf::Color(
        static_cast<sf::Uint8>(c1.r + t * (c2.r - c1.r)),
        static_cast<sf::Uint8>(c1.g + t * (c2.g - c1.g)),
        static_cast<sf::Uint8>(c1.b + t * (c2.b - c1.b)),
        static_cast<sf::Uint8>(c1.a + t * (c2.a - c1.a))
    );
}

void ModernButton::update(sf::Vector2f mousePos) {
    // Transform mouse position to button's local space to check bounds
    sf::FloatRect bounds = getTransform().transformRect(sf::FloatRect(0, 0, m_size.x, m_size.y));
    bool isNowHovered = bounds.contains(mousePos);

    if (m_isHovered != isNowHovered) {
        m_isHovered = isNowHovered;
        updateColors();
    }
}

bool ModernButton::isClicked(sf::Vector2f mousePos, bool mousePressed) {
    return m_isHovered && mousePressed;
}

void ModernButton::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform(); // Apply position, rotation, scale
    target.draw(m_background, states);
    target.draw(m_border, states);
    target.draw(m_divider, states);
    target.draw(m_text, states);
    target.draw(m_arrow, states);
}