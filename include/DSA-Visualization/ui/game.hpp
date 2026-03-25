#pragma once
#include <SFML/Graphics.hpp>
#include "DSA-Visualization/ui/Screen.hpp"
#include "DSA-Visualization/ui/MenuScreen.hpp"
#include "DSA-Visualization/ui/AVL_Screen.hpp"
#include "DSA-Visualization/ui/SLL_Screen.hpp"

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow mWindow;
    sf::Font         mFont;
};