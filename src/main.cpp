#include "DSA-Visualization/ui/game.hpp"
#include <iostream>

int main() {
    sf::Font mainFont;
    if (!mainFont.loadFromFile("assets/fonts/PhongChu.ttf")) {
        std::cerr << "CRITICAL ERROR: Failed to load PhongChu.ttf\n";
        return -1; 
    }

    Game game(mainFont);
 
    game.run();
    return 0;
}