#include "DSA-Visualization/ui/button.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ModernButton::ModernButton(const std::string& text, const sf::Font& font, sf::Vector2f size, float radius)
    : m_size(size), m_radius(radius), m_isHovered(false)
{
    // Soft Blue Theme
    m_topNormal    = sf::Color(74, 144, 226);  
    m_bottomNormal = sf::Color(53, 115, 195);  
    m_topHover     = sf::Color(90, 160, 240);  
    m_bottomHover  = sf::Color(65, 130, 215);
    m_borderColor  = sf::Color(40, 95, 170, 150);

    // Initialize visual elements
    buildGeometry();
    updateColors();

    // Setup Text
    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(18);
    m_text.setFillColor(sf::Color::White);

    // Center Text perfectly within the ENTIRE button
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                     textBounds.top  + textBounds.height / 2.0f);
    
    // Position exactly in the middle of m_size
    m_text.setPosition(m_size.x / 2.f, m_size.y / 2.f);

    // Note: m_arrow and m_divider are intentionally left unconfigured here
    // because we are choosing not to display them.
}

void ModernButton::update(sf::Vector2f mousePos) {
    sf::FloatRect localBounds(0.f, 0.f, m_size.x, m_size.y);
    sf::FloatRect globalBounds = getTransform().transformRect(localBounds);

    bool isNowHovered = globalBounds.contains(mousePos);

    if (isNowHovered != m_isHovered) {
        m_isHovered = isNowHovered;
        updateColors();
    }
}

bool ModernButton::isClicked(sf::Vector2f mousePos, bool mousePressed) {
    return m_isHovered && mousePressed;
}

void ModernButton::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    // Only draw the background, border, and text. 
    // The arrow and divider are completely ignored.
    target.draw(m_background, states);
    target.draw(m_border, states);
    target.draw(m_text, states);
}

void ModernButton::buildGeometry() {
    int pointsPerCorner = 15;
    std::vector<sf::Vector2f> outlinePoints;

    // Top-Left
    for (int i = 0; i < pointsPerCorner; ++i) {
        float angle = M_PI + (M_PI / 2.0f) * (i / (float)(pointsPerCorner - 1));
        outlinePoints.push_back({m_radius + std::cos(angle) * m_radius, m_radius + std::sin(angle) * m_radius});
    }
    // Top-Right
    for (int i = 0; i < pointsPerCorner; ++i) {
        float angle = M_PI * 1.5f + (M_PI / 2.0f) * (i / (float)(pointsPerCorner - 1));
        outlinePoints.push_back({m_size.x - m_radius + std::cos(angle) * m_radius, m_radius + std::sin(angle) * m_radius});
    }
    // Bottom-Right
    for (int i = 0; i < pointsPerCorner; ++i) {
        float angle = 0.0f + (M_PI / 2.0f) * (i / (float)(pointsPerCorner - 1));
        outlinePoints.push_back({m_size.x - m_radius + std::cos(angle) * m_radius, m_size.y - m_radius + std::sin(angle) * m_radius});
    }
    // Bottom-Left
    for (int i = 0; i < pointsPerCorner; ++i) {
        float angle = M_PI / 2.0f + (M_PI / 2.0f) * (i / (float)(pointsPerCorner - 1));
        outlinePoints.push_back({m_radius + std::cos(angle) * m_radius, m_size.y - m_radius + std::sin(angle) * m_radius});
    }

    m_background.setPrimitiveType(sf::TriangleFan);
    m_background.resize(outlinePoints.size() + 2); 

    m_border.setPrimitiveType(sf::LineStrip);
    m_border.resize(outlinePoints.size() + 1); 

    m_background[0].position = sf::Vector2f(m_size.x / 2.0f, m_size.y / 2.0f);

    for (size_t i = 0; i < outlinePoints.size(); ++i) {
        m_background[i + 1].position = outlinePoints[i];
        m_border[i].position = outlinePoints[i];
    }

    m_background[outlinePoints.size() + 1].position = outlinePoints[0];
    m_border[outlinePoints.size()].position = outlinePoints[0];
}

void ModernButton::updateColors() {
    sf::Color topColor    = m_isHovered ? m_topHover : m_topNormal;
    sf::Color bottomColor = m_isHovered ? m_bottomHover : m_bottomNormal;

    for (size_t i = 0; i < m_background.getVertexCount(); ++i) {
        float t = m_background[i].position.y / m_size.y;
        t = std::max(0.0f, std::min(1.0f, t)); 
        m_background[i].color = interpolateColor(topColor, bottomColor, t);
    }

    for (size_t i = 0; i < m_border.getVertexCount(); ++i) {
        m_border[i].color = m_borderColor;
    }
}

sf::Color ModernButton::interpolateColor(const sf::Color& color1, const sf::Color& color2, float t) const {
    return sf::Color(
        static_cast<sf::Uint8>(color1.r + (color2.r - color1.r) * t),
        static_cast<sf::Uint8>(color1.g + (color2.g - color1.g) * t),
        static_cast<sf::Uint8>(color1.b + (color2.b - color1.b) * t),
        static_cast<sf::Uint8>(color1.a + (color2.a - color1.a) * t)
    );
}