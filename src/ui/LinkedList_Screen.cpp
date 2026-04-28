#include "DSA-Visualization/ui/LinkedList_Screen.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"

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
            if (nextScene == SLL::SceneType::MAIN_MENU) return 0; 
            if (nextScene == SLL::SceneType::QUIT) {
                window.close();
                return -1;
            }
            nextScene = SLL::SceneType::NONE;
        }
        

        scene.Update(dt, window);

        // --- Used UITheme ---
        window.clear(UITheme::Color::PanelBg); 
        scene.Draw(window);
        window.display();
    }
    return -1;
}