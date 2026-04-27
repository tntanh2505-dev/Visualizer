#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <string>
#include <cmath>
#include <vector>
#include <map>

#include "DSA-Visualization/ui/UI_Theme.hpp"
#include "DSA-Visualization/ui/components/graphic_node.hpp"
#include "DSA-Visualization/ui/components/NodeArrow.hpp"
#include "DSA-Visualization/ui/button.hpp"
#include "DSA-Visualization/ui/codepanel.hpp"

namespace SLL {

extern int UniqueID;

// --- Core Visual Elements ---

class VisualNode {
public:
    int value;
    GraphicNode m_graphic;
    float radius = 30.f;
    int UID;
    
    bool isDragging = false;
    sf::Vector2f dragOffset;   
    sf::Vector2f targetPos;    
    
    float currentAlpha = 0.f;      
    bool isVisible = false;        
    bool isHighlighted = false;    
    
    int index = -1;
    sf::Text indexText; 

    VisualNode(int value, sf::Font& font, sf::Vector2f startPos);

    void setValue(int val);
    void updateTextPos();
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(const sf::RenderWindow& window, float dt);
    bool contains(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window);
};

class Connector {
public:
    NodeArrow m_arrow;
    int start_UID;
    int end_UID;
    float currentAlpha = 0.f;   
    bool isVisible = false;

    Connector(VisualNode& p1, VisualNode& p2);

    void update(VisualNode& startNode, VisualNode& endNode, float dt);
    void draw(sf::RenderWindow& window);
};

VisualNode* FindNode(int UID, std::vector<VisualNode>& nodes);
void ResolveCollisions(std::vector<VisualNode>& nodes, const sf::RenderWindow& window, float leftBound = 0, float rightBound = 0);

// --- Basic UI Components ---

class TextBox {
public:
    sf::RectangleShape box;
    sf::Text text;
    std::string input;
    std::string placeholder = "Enter Value..."; 
    bool isSelected = false;
    bool allowNegative = false;
    bool allowAllChars = false;

    TextBox(sf::Font& font, sf::Vector2f pos, float length = UITheme::Size::TextBoxDefault.x, float width = UITheme::Size::TextBoxDefault.y);

    void HandleEvent_IB(sf::Event& event, sf::RenderWindow& window);
    void setPosition(sf::Vector2f pos);
    void draw(sf::RenderWindow& window);
    void clear();
};

// --- Timeline & Snapshot Definitions ---

struct NodeRecord {
    int UID; 
    int value; 
    bool isHighlighted; 
    float targetX; 
    float targetY; 
    int index = -1;
};

struct LineRecord {
    int start_UID; 
    int end_UID; 
    bool isDirected; 
    int weight; 
    bool isHighlighted; 
};

struct Snapshot {
    std::vector<NodeRecord> nodes;
    std::vector<LineRecord> lines;
    std::string description; 
    bool isKeyStage = false; 
    std::string algorithmName = ""; 
    int activeLine = -1;
};

void SyncToFrame(const Snapshot& currentSnap, std::vector<VisualNode>& nodes, std::vector<Connector>& lines, sf::Font& font);

// --- Panels ---

enum class PanelAction { NONE, CHANGE_VALUE, DELETE_NODE, CLOSE };

struct PanelResult { 
    PanelAction action = PanelAction::NONE; 
    int targetUID = INT_MIN; 
    int newValue = 0; 
};

class NodePanel {
public:
    sf::RectangleShape bg; 
    sf::Text infoText; 
    TextBox inputField;
    ModernButton changeBtn; 
    ModernButton deleteBtn; 
    ModernButton closeBtn;
    
    bool isVisible = false; 
    int targetUID = INT_MIN; 
    sf::Font* fontPtr;

    NodePanel(sf::Font& font, float x = 1120, float y = 690);

    void setPosition(sf::Vector2f pos);
    PanelResult HandleEvent(sf::Event& event, sf::RenderWindow& window);
    void show(VisualNode* node);
    void hide();
    void draw(sf::RenderWindow& window);
};

class TimelineSlider {
public:
    sf::RectangleShape track; 
    sf::CircleShape handle;
    bool isDragging = false; 
    float trackX, trackY, trackWidth, trackHeight;

    TimelineSlider(sf::Vector2f pos, float width);

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window, int& currentFrame, int maxFrames, std::vector<Snapshot>& timeline, std::vector<VisualNode>& nodes, std::vector<Connector>& lines, sf::Font& font, bool& isAutoPlaying, ModernButton& pauseBtn);
    void setPosition(sf::Vector2f pos, float width);
    void update(int currentFrame, int maxFrames);
    void draw(sf::RenderWindow& window, const std::vector<Snapshot>& timeline);
};

class SpeedController {
public:
    sf::RectangleShape track; 
    sf::CircleShape handle; 
    sf::Text label;
    bool isDragging = false; 
    float speedValue = 2.0f;
    float baseTimeInterval = 1.0f; 
    float* targetTimeInterval;     

    SpeedController(sf::Font& font, float* timeIntervalPtr);

    void updateTimeInterval();
    void HandleEvent(sf::Event& event, sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    void setPosition(sf::Vector2f pos);
    void draw(sf::RenderWindow& window);
};

// --- Scene Management ---

enum class SceneType { NONE, MAIN_MENU, LINKED_LIST, HEAP, AVL_TREE, GRAPH, SETTINGS, QUIT };

class Scene {
public:
    virtual ~Scene() = default;
    virtual void HandleEvent(sf::Event& event, sf::RenderWindow& window, SceneType& nextScene) = 0;
    virtual void Update(float dt, sf::RenderWindow& window) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;
};

} // namespace SLL