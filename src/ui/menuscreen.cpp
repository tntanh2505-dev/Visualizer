#include "DSA-Visualization/ui/MenuScreen.hpp"
#include <iostream>
#include <ctime>
#include <algorithm> // For std::max

static const sf::Vector2f BUTTON_SIZE    = {260.f, 55.f};
static const float        BUTTON_X       = 200.f;
static const float        BUTTON_START_Y = 180.f;
static const float        BUTTON_GAP     = 80.f;

MenuScreen::MenuScreen() {}

void MenuScreen::buildButtons(const sf::Font& font) {
    mButtons.clear();
    std::vector<std::string> labels = {"Singly Linked List", "Heap", "Shortest Path", "AVL Tree", "Exit"};
    for (int i = 0; i < (int)labels.size(); i++) {
        ModernButton btn(labels[i], font, BUTTON_SIZE);
        btn.setPosition(BUTTON_X, BUTTON_START_Y + i * BUTTON_GAP);
        mButtons.push_back(btn);
    }
}

int MenuScreen::run(sf::RenderWindow& window, sf::Font& font) {
    if (!mBgTexture.loadFromFile("assets/textures/background.png"))
        std::cerr << "Failed to load background.png\n";

    sf::Vector2u winSize = window.getSize();
    mBgSprite.setTexture(mBgTexture);
    mBgSprite.setScale((float)winSize.x / mBgTexture.getSize().x, (float)winSize.y / mBgTexture.getSize().y);

    buildButtons(font);

    // --- SWARM CONFIG ---
    std::vector<SwarmNode> nodes;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    // 90 nodes is the visual/performance sweet spot 
    for (int i = 0; i < 90; ++i) {
        float x = static_cast<float>(std::rand() % winSize.x);
        float y = static_cast<float>(std::rand() % winSize.y);
        float vx = ((std::rand() % 100) - 50) / 70.f;
        float vy = ((std::rand() % 100) - 50) / 70.f;
        nodes.emplace_back(x, y, sf::Vector2f(vx, vy));
    }
    
    // Pre-allocate to prevent lag spikes
    sf::VertexArray lines(sf::Lines); 
    // --------------------

    while (window.isOpen()) {
        sf::Vector2f mouseCurrent = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (auto& btn : mButtons) btn.update(mouseCurrent);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); return -1; }

            // Isolate click coordinates directly from the hardware event
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f clickPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (mButtons[0].isClicked(clickPos, true)) return 1;
                if (mButtons[1].isClicked(clickPos, true)) return 2;
                if (mButtons[2].isClicked(clickPos, true)) return 3;
                if (mButtons[3].isClicked(clickPos, true)) return 4;
                if (mButtons[4].isClicked(clickPos, true)) return -1;
            }
        }

        // --- UPDATE NODES ---
        for (auto& n : nodes) {
            n.pos += n.vel;
            if (n.pos.x < 0 || n.pos.x > winSize.x) n.vel.x *= -1;
            if (n.pos.y < 0 || n.pos.y > winSize.y) n.vel.y *= -1;
            n.shape.setPosition(n.pos);
        }

        window.clear();
        window.draw(mBgSprite);

        // --- RENDER SWARM (NO-SQRT OPTIMIZATION) ---
        lines.clear();
        
        // We compare against Squared Distances to skip expensive math
        const float mouseDistSq = 35000.f; // ~187px radius
        const float nodeDistSq = 18000.f;  // ~134px radius

        for (size_t i = 0; i < nodes.size(); ++i) {
            // 1. Mouse Interaction
            float dxM = nodes[i].pos.x - mouseCurrent.x;
            float dyM = nodes[i].pos.y - mouseCurrent.y;
            float dSqM = dxM * dxM + dyM * dyM;
            
            if (dSqM < mouseDistSq) {
                // Linear alpha approximation
                sf::Uint8 alpha = static_cast<sf::Uint8>(180.f * (1.0f - (dSqM / mouseDistSq)));
                sf::Color mCol(130, 200, 255, alpha);
                lines.append(sf::Vertex(nodes[i].pos, mCol));
                lines.append(sf::Vertex(mouseCurrent, mCol));
            }

            // 2. Node-to-Node Connections
            // Note: Inner loop iterates normally. To boost performance further on older PCs,
            // change `j = i + 1` to `j = i + 2` to draw half as many lines.
            for (size_t j = i + 1; j < nodes.size(); ++j) {
                float dx = nodes[i].pos.x - nodes[j].pos.x;
                float dy = nodes[i].pos.y - nodes[j].pos.y;
                float dSq = dx * dx + dy * dy;

                if (dSq < nodeDistSq) {
                    sf::Uint8 alpha = static_cast<sf::Uint8>(120.f * (1.0f - (dSq / nodeDistSq)));
                    sf::Color c(255, 255, 255, alpha);
                    lines.append(sf::Vertex(nodes[i].pos, c));
                    lines.append(sf::Vertex(nodes[j].pos, c));
                }
            }
        }
        
        window.draw(lines);
        for (auto& n : nodes) window.draw(n.shape);

        // --- RENDER UI ---
        sf::Text title("ALGORITHM VISUALIZER", font, 64);
        title.setLetterSpacing(1.2f);
        title.setPosition(200.f, 60.f);
        sf::Text titleShadow = title;
        titleShadow.setFillColor(sf::Color(0, 0, 0, 180));
        titleShadow.move(4.f, 4.f);
        
        window.draw(titleShadow);
        window.draw(title);
        for (auto& btn : mButtons) window.draw(btn);

        window.display();
    }
    return -1;
}