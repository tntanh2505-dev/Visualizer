#pragma once
#include <SFML/Graphics.hpp>
#include "DSA-Visualization/ui/Screen.hpp"
#include "DSA-Visualization/linked_list/SLL.hpp"

class LL_screen : public Screen {
public:
    int run(sf::RenderWindow &window, sf::Font & font) override;
};
