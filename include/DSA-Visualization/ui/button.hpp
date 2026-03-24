#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class ModernButton : public sf::Drawable, public sf::Transformable {
public:
    // Constructor
    ModernButton(const std::string& text, const sf::Font& font, sf::Vector2f size = {200.f, 60.f}, float radius = 20.f);

    // Update state based on mouse position
    void update(sf::Vector2f mousePos);

    // Check if the button was clicked
    bool isClicked(sf::Vector2f mousePos, bool mousePressed);

protected:
    // Standard SFML draw override
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void buildGeometry();
    void updateColors();
    sf::Color interpolateColor(const sf::Color& color1, const sf::Color& color2, float t) const;

    sf::Vector2f m_size;
    float m_radius;
    bool m_isHovered;

    // Visual Components
    sf::VertexArray m_background;
    sf::VertexArray m_border;
    sf::Text m_text;
    // Theme Colors
    sf::Color m_topNormal;
    sf::Color m_bottomNormal;
    sf::Color m_topHover;
    sf::Color m_bottomHover;
    sf::Color m_borderColor;
};