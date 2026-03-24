#include "DSA-Visualization/ui/components/graphic_node.hpp"

GraphicNode::GraphicNode(float radius, const std::string& label, const sf::Font& font)
    : m_isHovered(false), m_isSelected(false) {

    // 1. Setup the "Outline" Shape (This fixes the pixelated edges)
    float outlineThickness = 3.f;
    m_outlineShape.setRadius(radius + outlineThickness);
    m_outlineShape.setOrigin(radius + outlineThickness, radius + outlineThickness);
    m_outlineShape.setFillColor(sf::Color(240, 240, 240)); // Off-white border

    // 2. Setup Main Inner Shape
    m_shape.setRadius(radius);
    m_shape.setOrigin(radius, radius); 
    m_baseColor = sf::Color(70, 130, 180); // Modern Steel Blue
    m_shape.setFillColor(m_baseColor);
    // Notice: We no longer use m_shape.setOutlineThickness()!

    // 3. Setup Drop Shadow 
    m_shadow.setRadius(radius + outlineThickness); // Shadow matches the outline size
    m_shadow.setOrigin(radius + outlineThickness, radius + outlineThickness);
    m_shadow.setFillColor(sf::Color(0, 0, 0, 80)); 
    m_shadow.setPosition(4.f, 5.f); 

    // 4. Setup Text Label
    m_text.setFont(font);
    m_text.setString(label);
    m_text.setCharacterSize(static_cast<unsigned int>(radius * 0.8f));
    m_text.setFillColor(sf::Color::White);
    
    centerText();
}

void GraphicNode::setFillColor(const sf::Color& color) {
    m_baseColor = color;
    if (!m_isHovered && !m_isSelected) {
        m_shape.setFillColor(color);
    }
}

void GraphicNode::setOutlineColor(const sf::Color& color) {
    // Apply color to our custom background shape instead of a native outline
    m_outlineShape.setFillColor(color);
}

void GraphicNode::setTextColor(const sf::Color& color) {
    m_text.setFillColor(color);
}

void GraphicNode::setHovered(bool hovered) {
    m_isHovered = hovered;
    if (m_isSelected) return; 

    if (m_isHovered) {
        sf::Color hoverColor = m_baseColor + sf::Color(30, 30, 30);
        m_shape.setFillColor(hoverColor);
    } else {
        m_shape.setFillColor(m_baseColor);
    }
}

void GraphicNode::setSelected(bool selected) {
    m_isSelected = selected;
    if (m_isSelected) {
        m_outlineShape.setFillColor(sf::Color(255, 215, 0)); // Gold selection
        
        // Slightly enlarge the outline to make selection pop
        float newRadius = getRadius() + 5.f;
        m_outlineShape.setRadius(newRadius);
        m_outlineShape.setOrigin(newRadius, newRadius);
    } else {
        m_outlineShape.setFillColor(sf::Color(240, 240, 240));
        
        // Revert to normal outline size
        float normalRadius = getRadius() + 3.f;
        m_outlineShape.setRadius(normalRadius);
        m_outlineShape.setOrigin(normalRadius, normalRadius);
        
        setHovered(m_isHovered);
    }
}

float GraphicNode::getRadius() const {
    return m_shape.getRadius();
}

void GraphicNode::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    // Draw order: Shadow -> Outline Shape -> Main Shape -> Text
    target.draw(m_shadow, states);
    target.draw(m_outlineShape, states); 
    target.draw(m_shape, states);
    target.draw(m_text, states);
}

void GraphicNode::centerText() {
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f,
                     textRect.top + textRect.height / 2.0f);
}