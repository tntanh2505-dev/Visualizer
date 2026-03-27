#include "DSA-Visualization/ui/game.hpp"
#include <iostream>

int main() {
    // Load the shared font once at startup so every UI screen can reuse the same asset.
    sf::Font mainFont;
    if (!mainFont.loadFromFile("assets/fonts/PhongChu.ttf")) {
        std::cerr << "CRITICAL ERROR: Failed to load PhongChu.ttf\n";
        return -1; 
    }

    // Pass the font into Game so buttons and visualizers can keep references to one loaded font.
    Game game(mainFont);
 
    // Hand control to the main SFML loop until the window closes.
    game.run();
    return 0;
}
