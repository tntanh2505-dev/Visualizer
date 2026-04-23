#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class ModernButton : public sf::Drawable, public sf::Transformable {
public:
    // Constructor (Signature unchanged for compatibility)
    ModernButton(const std::string& text, const sf::Font& font, sf::Vector2f size = {200.f, 60.f}, float radius = 20.f);

    // Update state based on mouse position
    void update(sf::Vector2f mousePos);

    // Check if the button was clicked
    bool isClicked(sf::Vector2f mousePos, bool mousePressed);

    // Get button boundary coordinates
    sf::FloatRect getGlobalBounds() const;

    //Set Color
    void setColors(sf::Color top, sf::Color bottom, sf::Color border, sf::Color textColor = sf::Color::White);

    void setText(const std::string &str);

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
    sf::VertexArray m_glow;       // NEW: Glow effect layer
    sf::Text m_text;
    
    mutable sf::Clock m_clock;    // NEW: Time tracker for pulsing glow animation

    // Theme Colors
    sf::Color m_topNormal;
    sf::Color m_bottomNormal;
    sf::Color m_topHover;
    sf::Color m_bottomHover;
    sf::Color m_borderColor;
};

class Button {
public:
    // Constructor (Signature unchanged for compatibility)
    Button(const std::string& label, const sf::Font& font,
           sf::Vector2f position, sf::Vector2f size);

    void draw(sf::RenderWindow& window) const;
    bool isClicked(const sf::Event& event, const sf::RenderWindow& window);
    void setHighlight(bool highlight);
    sf::FloatRect getGlobalBounds() const;

private:
    sf::ConvexShape mBox;         // UPGRADED: From RectangleShape to ConvexShape for round corners
    sf::ConvexShape mGlow;        // NEW: Glow effect layer
    sf::Text        mText;
    bool            mIsHighlighted = false;
    mutable sf::Clock mClock;     // NEW: Time tracker for pulsing glow animation
};