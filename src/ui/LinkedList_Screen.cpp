#include "DSA-Visualization/ui/LinkedList_Screen.hpp"

int LL_screen::run(sf::RenderWindow &window, sf::Font &font) {
    SLL::LinkedListScene scene(font, window.getSize().x, window.getSize().y);
    SLL::SceneType nextScene = SLL::SceneType::NONE;
    sf::Clock deltaClock;

    while (window.isOpen()) {
        float dt = deltaClock.restart().asSeconds();
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
            scene.HandleEvent(event, window, nextScene);
        }

        if (nextScene != SLL::SceneType::NONE) {
            if (nextScene == SLL::SceneType::MAIN_MENU) {
                return 0; // Return to main menu
            }
            if (nextScene == SLL::SceneType::QUIT) {
                window.close();
                return -1;
            }
            // Ignore other scenes (Heap, AVL, Graph) since Game class manages those
            nextScene = SLL::SceneType::NONE;
        }

        scene.Update(dt, window);

        window.clear(sf::Color(20, 20, 20)); 
        scene.Draw(window);
        window.display();
    }
    return -1;
}
