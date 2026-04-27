#include "DSA-Visualization/ui/button.hpp" // Ensure this matches your project's include path
#include <cmath>
#include <algorithm>

// ==========================================
// MODERN BUTTON IMPLEMENTATION
// ==========================================

ModernButton::ModernButton(const std::string& text, const sf::Font& font, sf::Vector2f size, float radius)
    : m_size(size), m_radius(radius), m_isHovered(false)
{
    // 1. Setup Theme Colors (Amethyst Dark Mode)
    m_topNormal    = sf::Color(32, 26, 43);     // Deep plum
    m_bottomNormal = sf::Color(20, 16, 27);     // Darker plum
    m_topHover     = sf::Color(53, 38, 77);     // Vibrant amethyst hover
    m_bottomHover  = sf::Color(37, 24, 56);     // Darker amethyst
    m_borderColor  = sf::Color(181, 58, 199, 120); // Neon purple outline

    // 2. Setup Text properties
    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(16);
    m_text.setFillColor(sf::Color::White);
    m_text.setLetterSpacing(1.1f); 

    // 3. Generate the rounded geometry & glow
    buildGeometry();

    // 4. Center the text perfectly
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    m_text.setPosition(m_size.x / 2.0f, m_size.y / 2.0f);

    // Set origin to center for smooth scaling effects
    setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
}

void ModernButton::setText(const std::string &str) {
    m_text.setString(str);
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    m_text.setPosition(m_size.x / 2.0f, m_size.y / 2.0f);
}

void ModernButton::update(sf::Vector2f mousePos)
{
    sf::Transform transform = getTransform();
    sf::FloatRect bounds = transform.transformRect(sf::FloatRect(0, 0, m_size.x, m_size.y));

    bool wasHovered = m_isHovered;
    m_isHovered = bounds.contains(mousePos);

    if (wasHovered != m_isHovered) {
        updateColors();
        // Subtle pop effect 
        if (m_isHovered) {
            setScale(1.02f, 1.02f); 
        } else {
            setScale(1.0f, 1.0f);
        }
    }
}

bool ModernButton::isClicked(sf::Vector2f mousePos, bool mousePressed)
{
    if (m_isHovered) {
        if (mousePressed) {
            setScale(0.98f, 0.98f); // Physical press down
            return true;
        } else {
            setScale(1.02f, 1.02f); // Restore hover
        }
    }
    return false;
}

sf::FloatRect ModernButton::getGlobalBounds() const {
    sf::FloatRect localBounds(0.f, 0.f, m_size.x, m_size.y);
    return getTransform().transformRect(localBounds);
}

void ModernButton::setColors(sf::Color top, sf::Color bottom, sf::Color border, sf::Color textColor) 
{
    m_topNormal = top;
    m_bottomNormal = bottom;
    m_borderColor = border;
    m_text.setFillColor(textColor);
    m_topHover = sf::Color(std::min(top.r + 20, 255), std::min(top.g + 20, 255), std::min(top.b + 20, 255));
    m_bottomHover = sf::Color(std::min(bottom.r + 20, 255), std::min(bottom.g + 20, 255), std::min(bottom.b + 20, 255));
    updateColors();
}

void ModernButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    
    // Draw animated glow effect behind the button when hovered
    if (m_isHovered) {
        float time = m_clock.getElapsedTime().asSeconds();
        float pulse = (std::sin(time * 5.0f) + 1.0f) / 2.0f; // Oscillates between 0.0 and 1.0
        
        // Dynamically adjust center alpha for the pulsing effect
        sf::Color glowCenterColor = m_borderColor;
        glowCenterColor.a = static_cast<sf::Uint8>(60 + 60 * pulse);
        
        // Cast away constness locally to update the visual pulse effect
        const_cast<ModernButton*>(this)->m_glow[0].color = glowCenterColor;
        
        target.draw(m_glow, states);
    }

    target.draw(m_background, states);
    target.draw(m_border, states);
    target.draw(m_text, states);
}

void ModernButton::buildGeometry()
{
    float r = std::min({m_radius, m_size.x / 2.f, m_size.y / 2.f});
    int ptsPerCorner = 15; // Smoothness of the corners
    
    m_background.setPrimitiveType(sf::TriangleFan);
    m_background.resize(ptsPerCorner * 4 + 2);

    m_glow.setPrimitiveType(sf::TriangleFan);
    m_glow.resize(ptsPerCorner * 4 + 2);

    m_border.setPrimitiveType(sf::LineStrip);
    m_border.resize(ptsPerCorner * 4 + 1);

    sf::Vector2f center(m_size.x / 2.f, m_size.y / 2.f);
    m_background[0].position = center;
    m_glow[0].position = center;

    // The 4 corner pivot points
    sf::Vector2f cornerCenters[4] = {
        {m_size.x - r, m_size.y - r}, // Bottom-Right
        {r, m_size.y - r},            // Bottom-Left
        {r, r},                       // Top-Left
        {m_size.x - r, r}             // Top-Right
    };
    float startAngles[4] = {0.f, 90.f, 180.f, 270.f};

    int bgIdx = 1;
    float glowOffset = 12.f; // How far the glow extends outward
    float glowR = r + glowOffset;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < ptsPerCorner; ++j) {
            float angle = startAngles[i] + (90.f * j) / (ptsPerCorner - 1);
            float rad = angle * 3.14159265f / 180.f;
            
            sf::Vector2f offset(std::cos(rad), std::sin(rad));
            
            // Standard Button coordinates
            sf::Vector2f pt = cornerCenters[i] + sf::Vector2f(r * offset.x, r * offset.y);
            m_background[bgIdx].position = pt;
            m_border[bgIdx - 1].position = pt;

            // Expanded Glow coordinates
            sf::Vector2f glowPt = cornerCenters[i] + sf::Vector2f(glowR * offset.x, glowR * offset.y);
            m_glow[bgIdx].position = glowPt;

            bgIdx++;
        }
    }
    
    // Close the shapes
    m_background[bgIdx].position = m_background[1].position;
    m_border[bgIdx - 1].position = m_border[0].position;
    m_glow[bgIdx].position = m_glow[1].position;

    updateColors();
}

void ModernButton::updateColors()
{
    sf::Color topColor = m_isHovered ? m_topHover : m_topNormal;
    sf::Color bottomColor = m_isHovered ? m_bottomHover : m_bottomNormal;

    // Center point color (mix of top and bottom)
    m_background[0].color = interpolateColor(topColor, bottomColor, 0.5f);
    
    // Set outer vertex colors based on their Y position to preserve the flawless gradient
    for (size_t i = 1; i < m_background.getVertexCount(); ++i) {
        float yRatio = m_background[i].position.y / m_size.y; 
        m_background[i].color = interpolateColor(topColor, bottomColor, yRatio);
        
        if (i - 1 < m_border.getVertexCount()) {
            m_border[i - 1].color = m_borderColor;
        }

        // Fade the glow out to completely transparent at the outer edges
        sf::Color glowEdge = m_borderColor;
        glowEdge.a = 0; 
        m_glow[i].color = glowEdge;
    }
}

sf::Color ModernButton::interpolateColor(const sf::Color& color1, const sf::Color& color2, float t) const
{
    t = std::max(0.0f, std::min(1.0f, t));
    return sf::Color(
        static_cast<sf::Uint8>(color1.r + t * (color2.r - color1.r)),
        static_cast<sf::Uint8>(color1.g + t * (color2.g - color1.g)),
        static_cast<sf::Uint8>(color1.b + t * (color2.b - color1.b)),
        static_cast<sf::Uint8>(color1.a + t * (color2.a - color1.a))
    );
}

// ==========================================
// STANDARD BUTTON IMPLEMENTATION
// ==========================================

Button::Button(const std::string& label, const sf::Font& font,
               sf::Vector2f position, sf::Vector2f size)
{
    // Helper lambda to map rounded corners onto a ConvexShape
    auto buildRoundedRect = [](sf::ConvexShape& shape, sf::Vector2f s, float r) {
        int pts = 10;
        shape.setPointCount(pts * 4);
        sf::Vector2f centers[4] = {
            {s.x - r, s.y - r}, {r, s.y - r}, {r, r}, {s.x - r, r}
        };
        float angles[4] = {0.f, 90.f, 180.f, 270.f};
        int idx = 0;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < pts; ++j) {
                float rad = (angles[i] + 90.f * j / (pts - 1)) * 3.14159265f / 180.f;
                shape.setPoint(idx++, centers[i] + sf::Vector2f(r * std::cos(rad), r * std::sin(rad)));
            }
        }
    };

    float radius = std::min({12.f, size.x / 2.f, size.y / 2.f});

    // 1. Setup Main Button Shape
    buildRoundedRect(mBox, size, radius);
    mBox.setPosition(position);
    mBox.setFillColor(sf::Color(70, 130, 180));
    mBox.setOutlineThickness(2.f);
    mBox.setOutlineColor(sf::Color::White);

    // 2. Setup Glow Shape (Slightly larger footprint)
    float glowExpand = 6.f;
    buildRoundedRect(mGlow, size + sf::Vector2f(glowExpand * 2, glowExpand * 2), radius + glowExpand);
    mGlow.setPosition(position - sf::Vector2f(glowExpand, glowExpand));
    mGlow.setFillColor(sf::Color::Transparent); // Handled in draw loop

    // 3. Setup Text
    mText.setFont(font);
    mText.setString(label);
    mText.setCharacterSize(22);
    mText.setFillColor(sf::Color::White);

    while (mText.getLocalBounds().width > size.x - 24.f && mText.getCharacterSize() > 14) {
        mText.setCharacterSize(mText.getCharacterSize() - 1);
    }

    sf::FloatRect textBounds = mText.getLocalBounds();
    mText.setOrigin(textBounds.left + textBounds.width  / 2.f,
                    textBounds.top  + textBounds.height / 2.f);
    mText.setPosition(position.x + size.x / 2.f,
                      position.y + size.y / 2.f - 2.f);
}

void Button::setHighlight(bool highlight) {
    mIsHighlighted = highlight;
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

void Button::draw(sf::RenderWindow& window) const {
    // Render an animated glowing border when highlighted
    if (mIsHighlighted) {
        float time = mClock.getElapsedTime().asSeconds();
        float pulse = (std::sin(time * 6.0f) + 1.0f) / 2.0f; // 0.0 to 1.0
        
        sf::Color glowColor = sf::Color(100, 160, 210); // Matches highlight color
        glowColor.a = static_cast<sf::Uint8>(30 + 70 * pulse);
        
        const_cast<Button*>(this)->mGlow.setFillColor(glowColor);
        window.draw(mGlow);
    }

    window.draw(mBox);
    window.draw(mText);
}