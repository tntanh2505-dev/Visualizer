#pragma once
#include "button.hpp"
#include "Screen.hpp"
#include "DSA-Visualization/graph/Dijkstra.hpp"
#include "DSA-Visualization/ui/CodePanel.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

enum class TabState { Info, Dist, Code };

struct EditState {
    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;
};

struct RunState {
    int m_currentLine;
    int m_visitingNode;
    int m_processingNode;
    std::vector<Node> m_nodes;
    std::vector<long long> m_dist;
};

class DijkstraScreen : public Screen {
public:
    virtual int run(sf::RenderWindow &window, sf::Font &font) override;

private:
    int counting;
    int currentIndex;
    std::vector<int> path;
    std::vector<EditState> m_edit;
    std::vector<RunState> m_run;

    std::vector<int> visitingList;
    std::vector<long long> dist;
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    Dijkstra algorithm;

    std::unique_ptr<ModernButton> button[10];

    bool returnFlag;
    bool finishFlag;
    bool isEditMode;
    bool isAutoMode;
    bool isDeleting;
    bool isDirected;
    bool isAlgoDone;
    bool isDragging;
    bool isInputActive;
    bool isDraggingSpeed;

    int sourceNode;
    int selectNode;
    int editingNode;
    int editingEdge;
    int draggingNode;
    int visitingNode;
    int processingNode;
    std::string inputBuffer;

    float leftWidth = 0.f;
    float rightWidth = 0.f;
    bool leftExpanded = true;
    bool rightExpanded = true;
    int currentLine;
    TabState activeTab;
    CodePanel panel;

    float delayTime;
    sf::Clock tickClock;

    void initialization();
    void handleInput(sf::RenderWindow &window, sf::Event &event, sf::Vector2i mPos);
    bool isSegmentHovering(sf::Vector2f pos, sf::Vector2f A, sf::Vector2f B);
    bool isPosValid(sf::Vector2f pos, float winW, float winH, int ignoreNode = -1);
    void processInputBuffer();
    void fixedSeekBar(float mouseX, float startX, float barWidth);
    void updateAnimation(float dt);
    sf::Color getNodeColor(sf::RenderWindow &window, int index);
    void drawGraph(sf::RenderWindow &window, sf::Font &font);
    void drawUI(sf::RenderWindow &window, sf::Font &font, sf::Vector2i mPos);

    // --- Node Customization ---
    float mNodeRadius;
    sf::Color mCurrentNodeColor;
    std::vector<sf::Color> mThemeColors;
    std::vector<sf::RectangleShape> mColorSwatches;

    // --- Slider UI ---
    sf::RectangleShape mSizeSliderTrack;
    sf::CircleShape mSizeSliderKnob;
    bool mIsDraggingSizeSlider;
};