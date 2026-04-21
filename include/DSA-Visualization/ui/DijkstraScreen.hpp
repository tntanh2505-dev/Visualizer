#pragma once
#include "button.hpp"
#include "Screen.hpp"
#include "DSA-Visualization/Shortest_Path/Dijkstra.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

enum class TabState { Info, Dist, Code };

class DijkstraScreen : public Screen {
public:
    virtual int run(sf::RenderWindow &window, sf::Font &font) override;

private:
    std::vector<int> visitingList;
    std::vector<long long> dist;
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    Dijkstra algorithm;

    std::unique_ptr<ModernButton> button[5];

    bool returnFlag;
    bool finishFlag;
    bool isEditMode;
    bool isAutoMode;
    bool isDeleting;
    bool isDirected;

    int sourceNode;
    int selectNode;
    int editingNode;
    int editingEdge;
    int draggingNode;
    int visitingNode;
    int processingNode;
    std::string inputBuffer;

    float leftWidth;
    float rightWidth;
    bool leftExpanded;
    bool rightExpanded;
    std::pair<int, int> currentLine;
    TabState activeTab;
    sf::Clock tickClock;

    void initialization();
    void handleInput(sf::RenderWindow &window, sf::Event &event, sf::Vector2i mPos);
    bool isSegmentHovering(sf::Vector2f pos, sf::Vector2f A, sf::Vector2f B);
    bool isPosValid(sf::Vector2f pos, float winW, int ignoreNode = -1);
    void updateAnimation(float dt);
    void drawGraph(sf::RenderWindow &window, sf::Font &font);
    void drawUI(sf::RenderWindow &window, sf::Font &font, sf::Vector2i mPos);
};