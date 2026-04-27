#ifndef GRAPHIC_NODE_HPP
#define GRAPHIC_NODE_HPP

#include <SFML/Graphics.hpp>
#include <string>

class GraphicNode : public sf::Drawable, public sf::Transformable {
public:
    GraphicNode(float radius, const std::string& label, const sf::Font& font);

    void setFillColor(const sf::Color& color);
    void setOutlineColor(const sf::Color& color);
    void setText(const std::string& str);
    void setTextColor(const sf::Color& color);
    
    void setHovered(bool hovered);
    void setSelected(bool selected);

    float getRadius() const;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void centerText();

    // The core shapes
    sf::CircleShape m_outlineShape; // The new "fake" outline
    sf::CircleShape m_shape;        // The main inner circle
    sf::CircleShape m_shadow;       // The drop shadow
    sf::Text m_text;

    sf::Color m_baseColor;
    bool m_isHovered;
    bool m_isSelected;
};

#endif // GRAPHIC_NODE_HPP