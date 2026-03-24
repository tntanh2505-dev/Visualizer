#pragma once
#include "DSA-Visualization/ui/Screen.hpp"
#include "DSA-Visualization/ui/Button.hpp"
#include <vector>

class MenuScreen : public Screen {
public:
    MenuScreen();
    int run(sf::RenderWindow& window, sf::Font& font) override;

private:
    sf::Texture           mBgTexture;
    sf::Sprite            mBgSprite;
    std::vector<ModernButton>   mButtons;

    void buildButtons(const sf::Font& font);
};