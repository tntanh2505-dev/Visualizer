#include "UI.hpp"
#include "SLL.hpp"
#include "Heap.hpp"

// --- Main Menu Scene ---
class MainMenuScene : public Scene {
private:
    sf::Text title;
    std::vector<Button> menuButtons;
    Button settingsBtn;

public:
    MainMenuScene(sf::Font& font, float windowWidth, float windowHeight) 
        : settingsBtn("SETTINGS", font, {windowWidth - 120.f, 20.f}, 100, 40) 
    {
        title.setFont(font); title.setString("Algorithm & Data Structure Visualizer");
        title.setCharacterSize(40); title.setFillColor(sf::Color::White);
        sf::FloatRect titleBounds = title.getLocalBounds();
        title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
        title.setPosition(windowWidth / 2.0f, windowHeight * 0.2f); 

        float btnWidth = 300.f; float btnHeight = 60.f; float startX = (windowWidth - btnWidth) / 2.0f; 
        float startY = windowHeight * 0.35f; float spacing = 80.f;

        menuButtons.emplace_back(Button("Singly Linked List", font, {startX, startY}, btnWidth, btnHeight));
        menuButtons.emplace_back(Button("Heap", font, {startX, startY + spacing}, btnWidth, btnHeight));
        menuButtons.emplace_back(Button("AVL Tree", font, {startX, startY + spacing * 2}, btnWidth, btnHeight));
        menuButtons.emplace_back(Button("Graph", font, {startX, startY + spacing * 3}, btnWidth, btnHeight));
        
        menuButtons.emplace_back(Button("EXIT", font, {startX, startY + spacing * 4}, btnWidth, btnHeight));
        menuButtons.back().shape.setFillColor(sf::Color(200, 0, 0)); 
    }

    void HandleEvent(sf::Event& event, sf::RenderWindow& window, SceneType& nextScene) override {
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (menuButtons[0].isClicked(mPos)) nextScene = SceneType::LINKED_LIST;
            else if (menuButtons[1].isClicked(mPos)) nextScene = SceneType::HEAP;
            else if (menuButtons[2].isClicked(mPos)) nextScene = SceneType::AVL_TREE;
            else if (menuButtons[3].isClicked(mPos)) nextScene = SceneType::GRAPH;
            else if (menuButtons[4].isClicked(mPos)) nextScene = SceneType::QUIT;
            else if (settingsBtn.isClicked(mPos)) nextScene = SceneType::SETTINGS;
        }
    }
    void Update(float dt, sf::RenderWindow& window) override {}
    void Draw(sf::RenderWindow& window) override {
        window.draw(title);
        for (auto& btn : menuButtons) btn.draw(window);
        settingsBtn.draw(window);
    }
};

int main() {
    srand(time(0));
    
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int safeWidth = static_cast<unsigned int>(desktop.width * 0.95f);
    unsigned int safeHeight = static_cast<unsigned int>(desktop.height * 0.90f);
    
    sf::RenderWindow window(sf::VideoMode(safeWidth, safeHeight), "Algorithm Visualizer", sf::Style::Default);
    window.setPosition(sf::Vector2i((desktop.width - safeWidth) / 2, (desktop.height - safeHeight) / 2 - 50.0f));
    window.setFramerateLimit(60); 

    sf::Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) return -1;

    Scene* currentScene = new MainMenuScene(font, safeWidth, safeHeight);
    SceneType nextScene = SceneType::NONE;
    sf::Clock deltaClock;
    
    while (window.isOpen()) {
        float dt = deltaClock.restart().asSeconds();
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
            if (currentScene) currentScene->HandleEvent(event, window, nextScene);
        }

        if (nextScene != SceneType::NONE) {
            if (nextScene == SceneType::QUIT) {
                window.close(); 
            } else {
                delete currentScene; 
                if (nextScene == SceneType::MAIN_MENU) currentScene = new MainMenuScene(font, window.getSize().x, window.getSize().y);
                else if (nextScene == SceneType::LINKED_LIST) currentScene = new LinkedListScene(font, window.getSize().x, window.getSize().y);
                else if (nextScene == SceneType::HEAP) currentScene = new HeapScene(font, window.getSize().x, window.getSize().y);
                else currentScene = new MainMenuScene(font, window.getSize().x, window.getSize().y); // Fallback for AVL/Graph 
            }
            nextScene = SceneType::NONE;
        }
        
        if (currentScene) currentScene->Update(dt, window);
        
        window.clear(sf::Color(20, 20, 20)); 
        if (currentScene) currentScene->Draw(window);
        window.display();
    }

    delete currentScene;
    return 0;
}