#pragma once
#include "Screen.hpp"
#include "DSA-Visualization/graph/Dijkstra.hpp"
#include <SFML/Graphics.hpp>

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

    bool returnStatus = false;
    bool isEditMode = true;
    bool isAutoMode = false;
    bool isDeleting = false;
    bool isDirected = false;
    int sourceNode = -1;
    int selectNode = -1;
    int editingNode = -1;
    int editingEdge = -1;
    int draggingNode = -1;
    int visitingNode = -1;
    int processingNode = -1;
    std::string inputBuffer;

    float leftWidth = 0.f;
    float rightWidth = 0.f;
    bool leftExpanded = true;
    bool rightExpanded = true;
    std::pair<int, int> currentLine;
    TabState activeTab = TabState::Info;
    sf::Clock tickClock;

    void handleInput(sf::RenderWindow &window, sf::Event &event, sf::Vector2i mPos);
    bool isSegmentHovering(sf::Vector2f pos, sf::Vector2f A, sf::Vector2f B);
    bool isPosValid(sf::Vector2f pos, float winW, int ignoreNode = -1);
    void updateAnimation(float dt);
    void drawGraph(sf::RenderWindow &window, sf::Font &font);
    void drawUI(sf::RenderWindow &window, sf::Font &font, sf::Vector2i mPos);
};