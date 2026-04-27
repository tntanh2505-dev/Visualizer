#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <string>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>

#include "DSA-Visualization/ui/UI_Theme.hpp"
#include "DSA-Visualization/ui/components/graphic_node.hpp"
#include "DSA-Visualization/ui/components/NodeArrow.hpp"
#include "DSA-Visualization/ui/button.hpp"
#include "DSA-Visualization/ui/codepanel.hpp"

namespace SLL {
inline int UniqueID = 0;

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

    VisualNode(int value, sf::Font& font, sf::Vector2f startPos) 
        : m_graphic(30.f, std::to_string(value), font) 
    {
        this->value = value;
        UID = UniqueID++;
        
        m_graphic.setPosition(startPos);
        targetPos = startPos; 

        indexText.setFont(font);
        indexText.setCharacterSize(UITheme::Size::FontSmall);
        indexText.setFillColor(UITheme::Color::TextMuted); 
    }

    void setValue(int val) {
        value = val;
        m_graphic.setText(std::to_string(val));
    }

    void updateTextPos() {
        if (index >= 0) {
            indexText.setString(std::to_string(index));
            sf::FloatRect b = indexText.getLocalBounds();
            indexText.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            indexText.setPosition(m_graphic.getPosition().x, m_graphic.getPosition().y - radius - 15.f);
        }
    }

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (contains(mousePos)) {
                isDragging = true;
                dragOffset = m_graphic.getPosition() - mousePos; 
            }
        }
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
        }
    }

    void update(const sf::RenderWindow& window, float dt) {
        if (isDragging) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            m_graphic.setPosition(mousePos + dragOffset);
            targetPos = m_graphic.getPosition(); 
        } else {
            sf::Vector2f currentPos = m_graphic.getPosition();
            sf::Vector2f diff = targetPos - currentPos;
            if (std::abs(diff.x) > 0.5f || std::abs(diff.y) > 0.5f) {
                float glideSpeed = 10.0f; 
                currentPos.x += diff.x * glideSpeed * dt;
                currentPos.y += diff.y * glideSpeed * dt;
                m_graphic.setPosition(currentPos);
            } else {
                m_graphic.setPosition(targetPos); 
            }
        }
        updateTextPos();

        if (isVisible && currentAlpha < 255.f) {
            currentAlpha += 500.f * dt; 
            if (currentAlpha > 255.f) currentAlpha = 255.f;
        } else if (!isVisible && currentAlpha > 0.f) {
            currentAlpha -= 500.f * dt; 
            if (currentAlpha < 0.f) currentAlpha = 0.f;
        }

        m_graphic.setSelected(isHighlighted);

        sf::Color fill = UITheme::Color::NodeFillColor;
        fill.a = static_cast<sf::Uint8>(currentAlpha);
        m_graphic.setFillColor(fill);

        sf::Color outline = isHighlighted ? UITheme::Color::NodeHighlightColor : UITheme::Color::NodeOutlineColor;
        outline.a = static_cast<sf::Uint8>(currentAlpha);
        m_graphic.setOutlineColor(outline);

        sf::Color txtColor = UITheme::Color::TextWhite;
        txtColor.a = static_cast<sf::Uint8>(currentAlpha);
        m_graphic.setTextColor(txtColor);
    }

    bool contains(sf::Vector2f mousePos) { 
        sf::Vector2f center = m_graphic.getPosition();
        float dist = std::sqrt(std::pow(mousePos.x - center.x, 2) + std::pow(mousePos.y - center.y, 2));
        return dist <= radius;
    }

    void draw(sf::RenderWindow& window) {
        // Only draw the node if it's not fully faded out
        if (currentAlpha > 0.f) {
            window.draw(m_graphic);
            if (index >= 0 && isVisible) window.draw(indexText);
        }
    }
};

class Connector {
public:
    NodeArrow m_arrow;
    int start_UID;
    int end_UID;
    float currentAlpha = 0.f;   
    bool isVisible = false;

    Connector(VisualNode& p1, VisualNode& p2) {
        start_UID = p1.UID;
        end_UID = p2.UID;
    }

    void update(VisualNode& startNode, VisualNode& endNode, float dt) {
        sf::Vector2f start = startNode.m_graphic.getPosition();
        sf::Vector2f end = endNode.m_graphic.getPosition();
        sf::Vector2f dir = end - start;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        
        if (dist > 0.001f) {
            sf::Vector2f unitDir = dir / dist;
            float startDist = std::min(startNode.radius, dist / 2.f);
            float endDist = std::min(endNode.radius, dist / 2.f);
            m_arrow.setPoints(start + unitDir * startDist, end - unitDir * endDist);
        }

        if (this->isVisible && startNode.isVisible && endNode.isVisible && currentAlpha < 255.f) {
            currentAlpha += 500.f * dt;
            if (currentAlpha > 255.f) currentAlpha = 255.f;
        } else if ((!this->isVisible || !startNode.isVisible || !endNode.isVisible) && currentAlpha > 0.f) {
            currentAlpha -= 500.f * dt;
            if (currentAlpha < 0.f) currentAlpha = 0.f;
        }

        sf::Color color = UITheme::Color::ConnectorBase;
        color.a = static_cast<sf::Uint8>(currentAlpha);
        m_arrow.setColor(color);
    }

    void draw(sf::RenderWindow& window) {
        if (currentAlpha > 0.f) {
            window.draw(m_arrow);
        }
    }
};

inline VisualNode* FindNode(int UID, std::vector<VisualNode>& nodes) {
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i].UID == UID) return &nodes[i];
    }
    return nullptr;
}

inline void ResolveCollisions(std::vector<VisualNode>& nodes, const sf::RenderWindow& window, float leftBound = 0, float rightBound = 0) {
    float width = window.getSize().x;
    float height = window.getSize().y;
    if (rightBound == 0) rightBound = width;
    
    float padding = 10.0f; 
    float collisionPadding = 10.0f; 

    auto isReadyForPhysics = [](const VisualNode& n) {
        if (!n.isVisible || n.currentAlpha < 255.f) return false;
        if (!n.isDragging) {
            sf::Vector2f diff = n.targetPos - n.m_graphic.getPosition();
            if (std::abs(diff.x) > 0.5f || std::abs(diff.y) > 0.5f) return false;
        }
        return true;
    };

    for (auto& node : nodes) {
        if (!isReadyForPhysics(node)) continue;
        sf::Vector2f pos = node.m_graphic.getPosition();
        float r = node.radius;

        float rightLimit = rightBound - r - padding; 
        if (pos.x - r < leftBound + padding) pos.x = leftBound + r + padding;
        if (pos.x + r > rightLimit) pos.x = rightLimit - r;
        
        float bottomLimit = height - 100; 
        if (pos.y - r < 0) pos.y = r + padding;
        if (pos.y + r > bottomLimit) pos.y = bottomLimit - r - padding; 

        node.m_graphic.setPosition(pos);
        node.updateTextPos();
    }

    for (int iter = 0; iter < 3; ++iter) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (!isReadyForPhysics(nodes[i])) continue;
            for (size_t j = i + 1; j < nodes.size(); ++j) {
                if (!isReadyForPhysics(nodes[j])) continue;
                sf::Vector2f p1 = nodes[i].m_graphic.getPosition();
                sf::Vector2f p2 = nodes[j].m_graphic.getPosition();
                
                float dx = p2.x - p1.x;
                float dy = p2.y - p1.y;
                float dist = std::sqrt(dx * dx + dy * dy);
                float minDist = nodes[i].radius + nodes[j].radius + collisionPadding;

                if (dist < minDist) {
                    if (dist == 0.0f) { dx = 1.0f; dist = 1.0f; }
                    float overlap = minDist - dist;
                    float nx = dx / dist; 
                    float ny = dy / dist; 

                    if (nodes[i].isDragging && !nodes[j].isDragging) {
                        p2.x += nx * overlap; p2.y += ny * overlap;
                    } else if (!nodes[i].isDragging && nodes[j].isDragging) {
                        p1.x -= nx * overlap; p1.y -= ny * overlap;
                    } else {
                        p1.x -= nx * (overlap / 2.0f); p1.y -= ny * (overlap / 2.0f);
                        p2.x += nx * (overlap / 2.0f); p2.y += ny * (overlap / 2.0f);
                    }
                    nodes[i].m_graphic.setPosition(p1); nodes[j].m_graphic.setPosition(p2);
                    nodes[i].updateTextPos(); nodes[j].updateTextPos();
                }
            }
        }
    }
}

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

    TextBox(sf::Font& font, sf::Vector2f pos, float length = UITheme::Size::TextBoxDefault.x, float width = UITheme::Size::TextBoxDefault.y) {
        box.setSize({length, width}); 
        box.setFillColor(UITheme::Color::TextBoxBg);
        box.setOutlineThickness(UITheme::Size::BoxOutlineThickness); 
        box.setOutlineColor(UITheme::Color::TextBoxBorder);
        box.setPosition(pos);

        text.setFont(font); text.setCharacterSize(UITheme::Size::FontNormal); 
        text.setPosition(pos.x + 5, pos.y + 8);
        text.setFillColor(UITheme::Color::TextMuted); 
        text.setString(placeholder);
    }

    void HandleEvent_IB(sf::Event& event, sf::RenderWindow& window) {
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            isSelected = box.getGlobalBounds().contains(mPos);
            box.setOutlineColor(isSelected ? sf::Color::Yellow : sf::Color::Cyan);
        }

        if (isSelected && event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8) { if (!input.empty()) input.pop_back(); }
            else if (event.text.unicode == 22) { sf::String clip = sf::Clipboard::getString(); input += clip.toAnsiString(); }
            else if (allowAllChars && event.text.unicode >= 32 && event.text.unicode <= 126) { input += static_cast<char>(event.text.unicode); }
            else if (event.text.unicode >= 48 && event.text.unicode <= 57) { if (input.size() < 5) input += static_cast<char>(event.text.unicode); }
            else if (allowNegative && event.text.unicode == 45 && input.empty()) { input += '-'; }

            if (input.empty()) {
                text.setFillColor(UITheme::Color::TextMuted); text.setString(placeholder);
            } else {
                text.setFillColor(UITheme::Color::TextDark); text.setString(input);
            }
        }
    }
    void setPosition(sf::Vector2f pos) { box.setPosition(pos); text.setPosition(pos.x + 5, pos.y + 8); }
    void draw(sf::RenderWindow& window) { window.draw(box); window.draw(text); }
    void clear() { input = ""; text.setString(""); }
};

// --- Timeline & Snapshot Definitions ---

struct NodeRecord {
    int UID; int value; bool isHighlighted; float targetX; float targetY; int index = -1;
};

struct LineRecord {
    int start_UID; int end_UID; bool isDirected; int weight; bool isHighlighted; 
};

struct Snapshot {
    std::vector<NodeRecord> nodes;
    std::vector<LineRecord> lines;
    std::string description; 
    bool isKeyStage = false; 
    std::string algorithmName = ""; 
    int activeLine = -1;
};

inline void SyncToFrame(const Snapshot& currentSnap, std::vector<VisualNode>& nodes, std::vector<Connector>& lines, sf::Font& font) {
    for (auto& node : nodes) node.isVisible = false;
    for (const auto& record : currentSnap.nodes) {
        VisualNode* existingNode = FindNode(record.UID, nodes);
        if (existingNode != nullptr) {
            existingNode->isVisible = true; 
            existingNode->isHighlighted = record.isHighlighted; 
            if (existingNode->value != record.value) {
                existingNode->setValue(record.value);
            }
            if (record.targetX != 0.f || record.targetY != 0.f) {
                existingNode->targetPos = sf::Vector2f(record.targetX, record.targetY);
            }
        } else {
            sf::Vector2f spawnPos = (record.targetX != 0.f || record.targetY != 0.f) ? sf::Vector2f(record.targetX, record.targetY) : sf::Vector2f(100.f, 200.f);
            VisualNode newNode(record.value, font, spawnPos);
            newNode.UID = record.UID; newNode.isVisible = true; newNode.isHighlighted = record.isHighlighted; 
            nodes.push_back(newNode);
        }
    }

    for (auto& line : lines) line.isVisible = false;
    for (const auto& record : currentSnap.lines) {
        bool lineExists = false;
        for (auto& line : lines) {
            if (line.start_UID == record.start_UID && line.end_UID == record.end_UID) {
                lineExists = true; line.isVisible = true; break;
            }
        }
        if (!lineExists) {
            VisualNode* startNode = FindNode(record.start_UID, nodes);
            VisualNode* endNode = FindNode(record.end_UID, nodes);
            if (startNode != nullptr && endNode != nullptr) {
                lines.emplace_back(*startNode, *endNode);
                lines.back().isVisible = true; 
            }
        }
    }
}

// --- Panels ---

enum class PanelAction { NONE, CHANGE_VALUE, DELETE_NODE, CLOSE };
struct PanelResult { PanelAction action = PanelAction::NONE; int targetUID = INT_MIN; int newValue = 0; };
class NodePanel {
public:
    sf::RectangleShape bg; sf::Text infoText; TextBox inputField;
    ModernButton changeBtn; ModernButton deleteBtn; ModernButton closeBtn;
    bool isVisible = false; int targetUID = INT_MIN; sf::Font* fontPtr;

    NodePanel(sf::Font& font, float x = 1120, float y = 690)
        : inputField(font, {1140, 800}),  
          changeBtn("CHANGE", font, {90.f, 40.f}), 
          deleteBtn("DELETE", font, {85.f, 40.f}), 
          closeBtn("X", font, {30.f, 30.f}) 
    {
        fontPtr = &font;
        bg.setSize(UITheme::Size::PanelDefault); 
        bg.setPosition(x, y); 
        bg.setFillColor(UITheme::Color::PanelBg);
        
        // INCREASED BORDER THICKNESS
        bg.setOutlineThickness(UITheme::Size::PanelOutlineThickness); 
        bg.setOutlineColor(UITheme::Color::PanelBorder);

        infoText.setFont(font); infoText.setCharacterSize(UITheme::Size::FontNormal);
        infoText.setPosition(x + 20, y + 20); infoText.setFillColor(UITheme::Color::TextWhite);
    }

    void setPosition(sf::Vector2f pos) {
        bg.setPosition(pos); 
        infoText.setPosition(pos.x + 20, pos.y + 20);
        inputField.setPosition({pos.x + 20, pos.y + 80 + 10});
        
        changeBtn.setPosition({pos.x + 200 + 15, pos.y + 100 + 10});
        deleteBtn.setPosition({pos.x + 300 + 15, pos.y + 100 + 10});
        closeBtn.setPosition({pos.x + bg.getSize().x - 20.f, pos.y + 20});
    }

    PanelResult HandleEvent(sf::Event& event, sf::RenderWindow& window) {
        PanelResult result;
        if (!isVisible) return result;
        inputField.HandleEvent_IB(event, window);
        
        sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        changeBtn.update(mPos); deleteBtn.update(mPos); closeBtn.update(mPos);

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (closeBtn.isClicked(mPos, true)) { hide(); result.action = PanelAction::CLOSE; } 
            else if (changeBtn.isClicked(mPos, true) && !inputField.input.empty()) {
                result.action = PanelAction::CHANGE_VALUE;
                result.targetUID = this->targetUID;
                result.newValue = std::stoi(inputField.input);
            }
            else if (deleteBtn.isClicked(mPos, true)) {
                result.action = PanelAction::DELETE_NODE;
                result.targetUID = this->targetUID;
                hide();
            }
        }
        return result;
    }

    void show(VisualNode* node) {
        if (!node) return;
        targetUID = node->UID; isVisible = true; inputField.clear(); 
        infoText.setString("Value: " + std::to_string(node->value) + "\nType: Node");
    }
    void hide() { isVisible = false; targetUID = INT_MIN; }
    void draw(sf::RenderWindow& window) {
        if (!isVisible) return;
        window.draw(bg); window.draw(infoText); inputField.draw(window);
        window.draw(changeBtn); window.draw(deleteBtn); window.draw(closeBtn);
    }
};

class TimelineSlider {
public:
    sf::RectangleShape track; sf::CircleShape handle;
    bool isDragging = false; float trackX, trackY, trackWidth, trackHeight;

    TimelineSlider(sf::Vector2f pos, float width) {
        trackWidth = width; trackHeight = 8.f; trackX = pos.x; trackY = pos.y;
        track.setSize({trackWidth, trackHeight}); track.setPosition(trackX, trackY);
        track.setFillColor(sf::Color(80, 80, 80)); track.setOutlineThickness(1); track.setOutlineColor(sf::Color(120, 120, 120));
        float handleRadius = 12.f; handle.setRadius(handleRadius); handle.setOrigin(handleRadius, handleRadius);
        handle.setPosition(trackX, trackY + trackHeight / 2.f); handle.setFillColor(sf::Color::White);
    }

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window, int& currentFrame, int maxFrames, std::vector<Snapshot>& timeline, std::vector<VisualNode>& nodes, std::vector<Connector>& lines, sf::Font& font, bool& isAutoPlaying, ModernButton& pauseBtn) {
        sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::FloatRect grabArea(trackX - 15, trackY - 15, trackWidth + 30, trackHeight + 30);
            if (grabArea.contains(mPos)) {
                isDragging = true; isAutoPlaying = false; pauseBtn.setText("PAUSE");
            }
        } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
        } else if (event.type == sf::Event::MouseMoved) {
            if (isDragging && maxFrames > 0) {
                float clampedX = std::max(trackX, std::min(mPos.x, trackX + trackWidth));
                float percent = (clampedX - trackX) / trackWidth;
                int targetFrame = std::round(percent * maxFrames);
                if (targetFrame != currentFrame) {
                    currentFrame = targetFrame;
                    SyncToFrame(timeline[currentFrame], nodes, lines, font);
                }
            }
        }
    }
    void setPosition(sf::Vector2f pos, float width) {
        trackX = pos.x; trackY = pos.y; trackWidth = width;
        track.setPosition(trackX, trackY); track.setSize({trackWidth, trackHeight});
    }
    void update(int currentFrame, int maxFrames) {
        if (maxFrames > 0) {
            float percent = static_cast<float>(currentFrame) / maxFrames;
            handle.setPosition(trackX + (percent * trackWidth), trackY + trackHeight / 2.f);
        } else {
            handle.setPosition(trackX, trackY + trackHeight / 2.f); 
        }
    }
    void draw(sf::RenderWindow& window, const std::vector<Snapshot>& timeline) {
        window.draw(track);
        int maxFrames = timeline.size() - 1;
        if (maxFrames > 0) {
            for (int i = 0; i <= maxFrames; ++i) {
                if (timeline[i].isKeyStage) {
                    sf::RectangleShape marker({4.f, 16.f}); marker.setFillColor(sf::Color::Yellow); marker.setOrigin(2.f, 8.f);
                    float percent = static_cast<float>(i) / maxFrames;
                    marker.setPosition(trackX + (percent * trackWidth), trackY + trackHeight / 2.f);
                    window.draw(marker);
                }
            }
        }
        window.draw(handle);
    }
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

    SpeedController(sf::Font& font, float* timeIntervalPtr)
        : targetTimeInterval(timeIntervalPtr) 
    {
        track.setSize({220.f, 6.f});
        track.setFillColor(UITheme::Color::SliderTrack);
        track.setOutlineThickness(1.f);
        track.setOutlineColor(UITheme::Color::NodeOutlineColor);

        handle.setRadius(10.f);
        handle.setOrigin(10.f, 10.f);
        handle.setFillColor(UITheme::Color::SliderHandle);

        label.setFont(font); 
        label.setCharacterSize(13); 
        label.setFillColor(UITheme::Color::AVLSpeedSliderText);
        
        updateTimeInterval(); 
    }

    void updateTimeInterval() {
        *targetTimeInterval = baseTimeInterval / speedValue;
        label.setString("Speed: " + std::to_string((int)speedValue) + "x");
    }

    void HandleEvent(sf::Event& event, sf::RenderWindow& window) {
        sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::FloatRect bounds = track.getGlobalBounds();
            bounds.top -= 10.f; bounds.height += 20.f; 
            bounds.left -= 10.f; bounds.width += 20.f;
            if (bounds.contains(mPos) || handle.getGlobalBounds().contains(mPos)) {
                isDragging = true;
            }
        }
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
        }
    }
    
    void update(sf::RenderWindow& window) {
        if (isDragging) {
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            float left = track.getPosition().x;
            float right = left + track.getSize().x;
            float clampedX = std::max(left, std::min(mPos.x, right));
            float ratio = (clampedX - left) / track.getSize().x;
            speedValue = 0.5f + ratio * 7.5f; 
            updateTimeInterval();
        }
        float ratio = (speedValue - 0.5f) / 7.5f;
        handle.setPosition(track.getPosition().x + ratio * track.getSize().x, track.getPosition().y + 3.f);
    }

    void setPosition(sf::Vector2f pos) {
        label.setPosition(pos.x, pos.y - 20); 
        track.setPosition(pos);
    }
    
    void draw(sf::RenderWindow& window) { 
        window.draw(label); 
        
        sf::RectangleShape filled({handle.getPosition().x - track.getPosition().x, 6.f});
        filled.setPosition(track.getPosition());
        filled.setFillColor(UITheme::Color::NodeOutlineColor);

        window.draw(track); 
        window.draw(filled);
        window.draw(handle); 
    }
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
}