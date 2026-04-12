#ifndef NODE_ARROW_HPP
#define NODE_ARROW_HPP

#include <SFML/Graphics.hpp>

class NodeArrow : public sf::Drawable
{
public:
    /**
     * @brief Creates a connecting arrow between two points
     * @param start The starting coordinate (e.g., center of Node A)
     * @param end The ending coordinate (e.g., center of Node B)
     * @param lineThickness The thickness of the connecting line
     * @param headSize The length and width scale of the arrowhead
     */
    NodeArrow(sf::Vector2f start = {0.f, 0.f}, 
              sf::Vector2f end = {0.f, 0.f}, 
              float lineThickness = 2.f, 
              float headSize = 15.f);

    void setPoints(sf::Vector2f start, sf::Vector2f end);
    void setColor(const sf::Color& color);
    
    // Getters in case you need to read the positions later
    sf::Vector2f getStartPoint() const { return m_start; }
    sf::Vector2f getEndPoint() const { return m_end; }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void updateGeometry();

    sf::Vector2f m_start;
    sf::Vector2f m_end;
    float m_thickness;
    float m_headSize;

    sf::RectangleShape m_line;
    sf::ConvexShape m_head;
};

#endif // NODE_ARROW_HPP