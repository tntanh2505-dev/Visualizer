#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class ModernButton : public sf::Drawable, public sf::Transformable {
public:
    ModernButton(const std::string& text, const sf::Font& font, sf::Vector2f size = {200.f, 60.f}, float radius = 20.f);
    void update(sf::Vector2f mousePos);
    bool isClicked(sf::Vector2f mousePos, bool mousePressed);

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void buildGeometry();
    void updateColors();
    sf::Color interpolateColor(const sf::Color& color1, const sf::Color& color2, float t) const;

    sf::Vector2f m_size;
    float m_radius;
    bool m_isHovered;

    sf::VertexArray m_background;
    sf::VertexArray m_border;
    sf::Text m_text;
    
    sf::Color m_topNormal;
    sf::Color m_bottomNormal;
    sf::Color m_topHover;
    sf::Color m_bottomHover;
    sf::Color m_borderColor;
};

class Button {
public:
    Button(const std::string& label, const sf::Font& font,
           sf::Vector2f position, sf::Vector2f size);
           
    void setPosition(sf::Vector2f pos);
    void setText(const std::string& text);
    void setHighlight(bool highlight);
    sf::FloatRect getGlobalBounds() const;

    void draw(sf::RenderWindow& window) const;
    bool isClicked(const sf::Event& event, const sf::RenderWindow& window);

private:
    sf::RectangleShape mBox;
    sf::Text           mText;
    bool               mIsHovered = false; 
};