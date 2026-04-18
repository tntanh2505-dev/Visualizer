#include "DSA-Visualization/ui/components/NodeArrow.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"
#include <cmath>
#include <algorithm>

NodeArrow::NodeArrow(sf::Vector2f start, sf::Vector2f end, float lineThickness, float headSize) :
    m_start(start), 
    m_end(end), 
    m_thickness(lineThickness), 
    m_headSize(headSize)
{
    m_head.setPointCount(3); // Arrowhead is a triangle
    setColor(sf::Color::White); // Default color
    updateGeometry();
}

void NodeArrow::setPoints(sf::Vector2f start, sf::Vector2f end)
{
    m_start = start;
    m_end = end;
    updateGeometry(); // Recalculate math whenever points move
}

void NodeArrow::setColor(const sf::Color& color)
{
    m_line.setFillColor(color);
    m_head.setFillColor(color);
}

void NodeArrow::updateGeometry()
{
    // 1. Calculate distance and direction
    sf::Vector2f direction = m_end - m_start;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    // Prevent rendering glitches if start and end are exactly the same
    if (length == 0.f) return;

    // 2. Calculate the angle in degrees
    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.141592654f;

    // 3. Setup the Line
    // We subtract the head size from the total length so the line doesn't poke out the front of the triangle
    float lineLength = std::max(0.f, length - m_headSize); 
    
    m_line.setSize(sf::Vector2f(lineLength, m_thickness));
    m_line.setOrigin(0.f, m_thickness / 2.f); // Anchor line to its vertical center
    m_line.setPosition(m_start);
    m_line.setRotation(angle);

    // 4. Setup the Head
    // Create a triangle pointing to the right (0 degrees)
    m_head.setPoint(0, sf::Vector2f(0.f, 0.f));                            // Tip (Origin)
    m_head.setPoint(1, sf::Vector2f(-m_headSize, -m_headSize / 2.f));      // Top corner
    m_head.setPoint(2, sf::Vector2f(-m_headSize, m_headSize / 2.f));       // Bottom corner

    m_head.setPosition(m_end);
    m_head.setRotation(angle);
}

void NodeArrow::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_start != m_end) 
    {
        target.draw(m_line, states);
        target.draw(m_head, states);
    }
}