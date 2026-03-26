#include "DSA-Visualization/ui/MenuScreen.hpp"
#include <iostream>
#include <vector>
#include <cmath>
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

    // --- OPTIMIZED SWARM CONFIG ---
    std::vector<SwarmNode> nodes;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // Reduced to 80 for better performance on varied hardware
    for (int i = 0; i < 80; ++i) {
        nodes.emplace_back(
            (float)(std::rand() % winSize.x), 
            (float)(std::rand() % winSize.y), 
            sf::Vector2f(((std::rand() % 100) - 50) / 70.f, ((std::rand() % 100) - 50) / 70.f)
        );
    }
    const float connectDist = 130.f;
    const float connectDistSq = connectDist * connectDist;
    // Pre-allocate VertexArray to prevent mid-frame memory reallocations
    sf::VertexArray lines(sf::Lines); 
    // ------------------------------

    while (window.isOpen()) {
        // Update mouse for hovering (visual only)
        sf::Vector2f mouseCurrent = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (auto& btn : mButtons) btn.update(mouseCurrent);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); return -1; }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                // IMPORTANT: Map the click coordinates specifically from the event
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

        // --- RENDER SWARM ---
        lines.clear();
        for (size_t i = 0; i < nodes.size(); ++i) {
            // Mouse Influence
            float dxM = nodes[i].pos.x - mouseCurrent.x;
            float dyM = nodes[i].pos.y - mouseCurrent.y;
            float dSqM = dxM * dxM + dyM * dyM;
            if (dSqM < connectDistSq * 2.0f) {
                float alpha = 180.f * (1.0f - (std::sqrt(dSqM) / (connectDist * 1.4f)));
                sf::Color mCol(130, 200, 255, static_cast<sf::Uint8>(std::max(0.f, alpha)));
                lines.append(sf::Vertex(nodes[i].pos, mCol));
                lines.append(sf::Vertex(mouseCurrent, mCol));
            }

            // Node to Node Connections
            for (size_t j = i + 1; j < nodes.size(); ++j) {
                float dx = nodes[i].pos.x - nodes[j].pos.x;
                float dy = nodes[i].pos.y - nodes[j].pos.y;
                float dSq = dx * dx + dy * dy;

                if (dSq < connectDistSq) {
                    float alpha = 100.f * (1.0f - (std::sqrt(dSq) / connectDist));
                    sf::Color c(255, 255, 255, static_cast<sf::Uint8>(std::max(0.f, alpha)));
                    lines.append(sf::Vertex(nodes[i].pos, c));
                    lines.append(sf::Vertex(nodes[j].pos, c));
                }
            }
        }
        window.draw(lines);
        for (auto& n : nodes) window.draw(n.shape);

        // --- RENDER UI (Last so it's on top) ---
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