#pragma once
#include "DSA-Visualization/ui/Screen.hpp"
#include "DSA-Visualization/ui/Button.hpp"
#include <vector>
#include <random>

class MenuScreen : public Screen {
public:
    MenuScreen();
    int run(sf::RenderWindow& window, sf::Font& font) override;

private:
    sf::Texture                 mBgTexture;
    sf::Sprite                  mBgSprite;
    std::vector<ModernButton>   mButtons;

    void buildButtons(const sf::Font& font);
};

// --- HIGH PERFORMANCE SWARM NODE ---
struct SwarmNode {
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::CircleShape shape;

    SwarmNode(float x, float y, sf::Vector2f velocity) : pos(x, y), vel(velocity) {
        shape.setRadius(1.5f);
        shape.setOrigin(1.5f, 1.5f);
        shape.setFillColor(sf::Color(255, 255, 255, 150));
    }
};