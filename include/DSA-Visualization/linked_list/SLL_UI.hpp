#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <string>
#include <cmath>
#include <math.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>

namespace SLL {
inline int UniqueID = 0; // Global ID counter

// --- Core Visual Elements ---

class VisualNode {
public:
    int value;
    sf::CircleShape shape;
    sf::Text text;
    float radius = 30.f;
    int UID;
    
    bool isDragging = false;
    sf::Vector2f dragOffset;   
    sf::Vector2f targetPos;    
    
    float currentAlpha = 0.f;      
    bool isVisible = false;        
    bool isHighlighted = false;    
    sf::Color defaultOutlineColor = sf::Color(0, 255, 255, 0); 
    sf::Color highlightColor = sf::Color(255, 255, 0); 
    
    int index = -1;
    sf::Text indexText; 

    VisualNode(int value, sf::Font& font, sf::Vector2f startPos) {
        this->value = value;
        UID = UniqueID++;
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(defaultOutlineColor); 
        shape.setOrigin(radius, radius); 
        shape.setPosition(startPos);
        targetPos = startPos; 

        text.setFont(font);
        text.setString(std::to_string(value));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color(255, 255, 255, 0));
        centerText();

        indexText.setFont(font);
        indexText.setCharacterSize(14);
        indexText.setFillColor(sf::Color(150, 150, 150)); 
    }
    
    void centerText() {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
        text.setPosition(shape.getPosition().x, shape.getPosition().y);
    }

    void updateTextPos() {
        text.setPosition(shape.getPosition());
        if (index >= 0) {
            indexText.setString(std::to_string(index));
            sf::FloatRect b = indexText.getLocalBounds();
            indexText.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            indexText.setPosition(shape.getPosition().x, shape.getPosition().y - radius - 15.f);
        }
    }

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (contains(mousePos)) {
                isDragging = true;
                dragOffset = shape.getPosition() - mousePos; 
            }
        }
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
        }
    }

    void update(const sf::RenderWindow& window, float dt) {
        if (isDragging) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            shape.setPosition(mousePos + dragOffset);
            targetPos = shape.getPosition(); 
        } else {
            sf::Vector2f currentPos = shape.getPosition();
            sf::Vector2f diff = targetPos - currentPos;
            if (std::abs(diff.x) > 0.5f || std::abs(diff.y) > 0.5f) {
                float glideSpeed = 10.0f; 
                currentPos.x += diff.x * glideSpeed * dt;
                currentPos.y += diff.y * glideSpeed * dt;
                shape.setPosition(currentPos);
            } else {
                shape.setPosition(targetPos); 
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

        sf::Color baseOutlineColor = isHighlighted ? highlightColor : defaultOutlineColor;
        baseOutlineColor.a = static_cast<sf::Uint8>(currentAlpha);
        shape.setOutlineColor(baseOutlineColor);

        sf::Color txtColor = text.getFillColor();
        txtColor.a = static_cast<sf::Uint8>(currentAlpha);
        text.setFillColor(txtColor);
    }

    bool contains(sf::Vector2f mousePos) { return shape.getGlobalBounds().contains(mousePos); }
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
        if (index >= 0 && isVisible) window.draw(indexText);
    }
};

class Connector {
public:
    sf::VertexArray line;
    sf::ConvexShape head;
    int start_UID;
    int end_UID;
    float currentAlpha = 0.f;   
    bool isVisible = false;

    Connector(VisualNode& p1, VisualNode& p2) {
        if (line.getVertexCount() < 2) {
            line.setPrimitiveType(sf::Lines);
            line.resize(2);
        }
        head.setPointCount(3);
        head.setPoint(0, sf::Vector2f(0, 0));          
        head.setPoint(1, sf::Vector2f(-15, -7));      
        head.setPoint(2, sf::Vector2f(-15, 7));       
        line[0].color = sf::Color(255, 255, 255, 0);
        line[1].color = sf::Color(255, 255, 255, 0);
        head.setFillColor(sf::Color(255, 255, 255, 0));
        start_UID = p1.UID;
        end_UID = p2.UID;
    }

    void update(VisualNode& startNode, VisualNode& endNode, float dt) {
        sf::Vector2f start = startNode.shape.getPosition();
        sf::Vector2f end = endNode.shape.getPosition();
        sf::Vector2f dir = end - start;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        
        if (dist > 0.001f) {
            sf::Vector2f unitDir = dir / dist;
            float startDist = std::min(startNode.radius, dist / 2.f);
            float endDist = std::min(endNode.radius, dist / 2.f);
            line[0].position = start + (unitDir * startDist);
            line[1].position = end - (unitDir * endDist);
            head.setPosition(line[1].position);
            float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;
            head.setRotation(angle);
        }

        if (this->isVisible && startNode.isVisible && endNode.isVisible && currentAlpha < 255.f) {
            currentAlpha += 500.f * dt;
            if (currentAlpha > 255.f) currentAlpha = 255.f;
            line[0].color.a = static_cast<sf::Uint8>(currentAlpha);
            line[1].color.a = static_cast<sf::Uint8>(currentAlpha);
            sf::Color headColor = head.getFillColor();
            headColor.a = static_cast<sf::Uint8>(currentAlpha);
            head.setFillColor(headColor);
        } else if ((!this->isVisible || !startNode.isVisible || !endNode.isVisible) && currentAlpha > 0.f) {
            currentAlpha = 0.f;
            line[0].color.a = 0;
            line[1].color.a = 0;
            sf::Color headColor = head.getFillColor();
            headColor.a = 0;
            head.setFillColor(headColor);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(line);
        window.draw(head);
    }
};

// --- Utilities & General Physics ---

inline VisualNode* FindNode(int UID, std::vector<VisualNode>& nodes) {
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i].UID == UID) return &nodes[i];
    }
    return nullptr;
}

inline void ResolveCollisions(std::vector<VisualNode>& nodes, const sf::RenderWindow& window) {
    float width = window.getSize().x;
    float height = window.getSize().y;
    float padding = 10.0f; 
    float collisionPadding = 10.0f; 

    auto isReadyForPhysics = [](const VisualNode& n) {
        if (!n.isVisible || n.currentAlpha < 255.f) return false;
        if (!n.isDragging) {
            sf::Vector2f diff = n.targetPos - n.shape.getPosition();
            if (std::abs(diff.x) > 0.5f || std::abs(diff.y) > 0.5f) return false;
        }
        return true;
    };

    for (auto& node : nodes) {
        if (!isReadyForPhysics(node)) continue;
        sf::Vector2f pos = node.shape.getPosition();
        float r = node.radius;

        float rightLimit = width - r - 500; 
        if (pos.x - r < 0) pos.x = r + padding;
        if (pos.x + r > rightLimit) pos.x = rightLimit - r - padding;
        
        float bottomLimit = height - 250; 
        if (pos.y - r < 0) pos.y = r + padding;
        if (pos.y + r > bottomLimit) pos.y = bottomLimit - r - padding; 

        node.shape.setPosition(pos);
        node.updateTextPos();
    }

    for (int iter = 0; iter < 3; ++iter) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (!isReadyForPhysics(nodes[i])) continue;
            for (size_t j = i + 1; j < nodes.size(); ++j) {
                if (!isReadyForPhysics(nodes[j])) continue;
                sf::Vector2f p1 = nodes[i].shape.getPosition();
                sf::Vector2f p2 = nodes[j].shape.getPosition();
                
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
                    nodes[i].shape.setPosition(p1); nodes[j].shape.setPosition(p2);
                    nodes[i].updateTextPos(); nodes[j].updateTextPos();
                }
            }
        }
    }
}

// --- Basic UI Components ---

class Button {
public:
    sf::RectangleShape shape;
    sf::Text label;
    float l, w;

    Button(std::string txt, sf::Font& font, sf::Vector2f pos, int length = 100, int width = 40) {
        l = length; w = width;
        shape.setSize({l, w});
        shape.setFillColor(sf::Color(0, 150, 0)); 
        shape.setPosition(pos);

        label.setFont(font); label.setString(txt); label.setCharacterSize(18);
        label.setFillColor(sf::Color::White);
        sf::FloatRect b = label.getLocalBounds();
        label.setOrigin(b.left + b.width/2.f, b.top + b.height/2.f);
        label.setPosition(pos.x + l / 2, pos.y + w / 2);
    }
    void setPosition(sf::Vector2f pos) {
        shape.setPosition(pos);
        sf::FloatRect b = label.getLocalBounds();
        label.setOrigin(b.left + b.width/2.f, b.top + b.height/2.f);
        label.setPosition(pos.x + l / 2.f, pos.y + w / 2.f);
    }
    void setText(std::string s) { label.setString(s); }
    bool isClicked(sf::Vector2f mPos) { return shape.getGlobalBounds().contains(mPos); }
    void draw(sf::RenderWindow& window) { window.draw(shape); window.draw(label); }
};

class TextBox {
public:
    sf::RectangleShape box;
    sf::Text text;
    std::string input;
    std::string placeholder = "Enter Value..."; 
    bool isSelected = false;
    bool allowNegative = false;
    bool allowAllChars = false;

    TextBox(sf::Font& font, sf::Vector2f pos, float length = 140, float width = 40) {
        box.setSize({length, width}); box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2); box.setOutlineColor(sf::Color::Cyan);
        box.setPosition(pos);

        text.setFont(font); text.setCharacterSize(18); text.setPosition(pos.x + 5, pos.y + 8);
        text.setFillColor(sf::Color(150, 150, 150)); text.setString(placeholder);
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
                text.setFillColor(sf::Color(150, 150, 150)); text.setString(placeholder);
            } else {
                text.setFillColor(sf::Color::Black); text.setString(input);
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
                existingNode->value = record.value;
                existingNode->text.setString(std::to_string(record.value));
                existingNode->centerText();
            }
            if (record.targetX != 0.f || record.targetY != 0.f) {
                existingNode->targetPos = sf::Vector2f(record.targetX, record.targetY);
            }
        } else {
            sf::Vector2f spawnPos = (record.targetX != 0.f || record.targetY != 0.f) ? sf::Vector2f(record.targetX, record.targetY) : sf::Vector2f(100.f, 200.f);
            VisualNode newNode(record.value, font, {100.f, 200.f});
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
    Button changeBtn; Button deleteBtn; Button closeBtn;
    bool isVisible = false; int targetUID = INT_MIN; sf::Font* fontPtr;

    NodePanel(sf::Font& font, float x = 1120, float y = 690)
        : inputField(font, {1140, 800}),  
          changeBtn("CHANGE", font, {1290, 800}, 90, 40), 
          deleteBtn("DELETE", font, {1390, 800}, 85, 40), 
          closeBtn("X", font, {1440, 710}, 30, 30) 
    {
        fontPtr = &font;
        bg.setSize({360, 160}); bg.setPosition(x, y); 
        bg.setFillColor(sf::Color(40, 40, 40, 240));
        bg.setOutlineThickness(2); bg.setOutlineColor(sf::Color::White);

        infoText.setFont(font); infoText.setCharacterSize(18);
        infoText.setPosition(x + 20, y + 20); infoText.setFillColor(sf::Color::White);
        closeBtn.shape.setFillColor(sf::Color::Red);
        deleteBtn.shape.setFillColor(sf::Color(200, 0, 0)); 
    }

    void setPosition(sf::Vector2f pos) {
        bg.setPosition(pos); infoText.setPosition(pos.x + 20, pos.y + 20);
        inputField.setPosition({pos.x + 20, pos.y + 110});
        changeBtn.setPosition({pos.x + 170, pos.y + 110});
        deleteBtn.setPosition({pos.x + 270, pos.y + 110});
        closeBtn.setPosition({pos.x + 320, pos.y + 20});
    }

    PanelResult HandleEvent(sf::Event& event, sf::RenderWindow& window) {
        PanelResult result;
        if (!isVisible) return result;
        inputField.HandleEvent_IB(event, window);

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (closeBtn.isClicked(mPos)) { hide(); result.action = PanelAction::CLOSE; } 
            else if (changeBtn.isClicked(mPos) && !inputField.input.empty()) {
                result.action = PanelAction::CHANGE_VALUE;
                result.targetUID = this->targetUID;
                result.newValue = std::stoi(inputField.input);
            }
            else if (deleteBtn.isClicked(mPos)) {
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
        changeBtn.draw(window); deleteBtn.draw(window); closeBtn.draw(window);
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

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window, int& currentFrame, int maxFrames, std::vector<Snapshot>& timeline, std::vector<VisualNode>& nodes, std::vector<Connector>& lines, sf::Font& font, bool& isAutoPlaying, Button& pauseBtn) {
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

class CodePanel {
public:
    sf::RectangleShape bg; sf::RectangleShape highlight;
    std::map<std::string, std::vector<std::string>> codeSnippets;
    std::vector<sf::Text> displayLines;
    sf::Font* fontPtr; bool isVisible = false;

    CodePanel(sf::Font& font) {
        fontPtr = &font; bg.setSize({360, 180}); bg.setFillColor(sf::Color(40, 40, 40, 240));
        bg.setOutlineThickness(2); bg.setOutlineColor(sf::Color::White);
        highlight.setFillColor(sf::Color(255, 255, 0, 100));
    }
    void loadSnippets(const std::map<std::string, std::vector<std::string>>& newSnippets) { codeSnippets = newSnippets; }
    void setPosition(sf::Vector2f pos) { bg.setPosition(pos); }
    
    void update(std::string algoName, int activeLine) {
        if (algoName.empty()) { isVisible = false; return; }
        isVisible = true; displayLines.clear(); highlight.setSize({0, 0}); 

        if (codeSnippets.find(algoName) != codeSnippets.end()) {
            std::vector<std::string>& lines = codeSnippets[algoName];
            float startX = bg.getPosition().x + 10; float startY = bg.getPosition().y + 10;
            for (int i = 0; i < lines.size(); ++i) {
                sf::Text t; t.setFont(*fontPtr); t.setString(lines[i]); t.setCharacterSize(16);
                t.setFillColor(sf::Color::White); t.setPosition(startX, startY + (i * 25));
                displayLines.push_back(t);
                if (i == activeLine) {
                    highlight.setPosition(bg.getPosition().x + 2, startY + (i * 25) - 2);
                    highlight.setSize({bg.getSize().x - 4, 24});
                }
            }
        }
    }
    void draw(sf::RenderWindow& window) {
        if (!isVisible) return;
        window.draw(bg); if (highlight.getSize().x > 0) window.draw(highlight);
        for (auto& t : displayLines) window.draw(t);
    }
};

class SpeedController {
public:
    Button minusBtn; Button plusBtn; TextBox inputBox; sf::Text label;
    int multiplier = 0; float baseTimeInterval = 0.001f; float* targetTimeInterval;     

    SpeedController(sf::Font& font, float* timeIntervalPtr)
        : minusBtn("-", font, {0, 0}, 40, 40), plusBtn("+", font, {0, 0}, 40, 40),
          inputBox(font, {0, 0}, 50, 35), targetTimeInterval(timeIntervalPtr) 
    {
        inputBox.allowNegative = true; label.setFont(font); label.setString("Speed Mult.");
        label.setCharacterSize(16); label.setFillColor(sf::Color::White);
        updateTimeInterval(); 
    }

    void updateTimeInterval() {
        if (multiplier > 3) multiplier = 3; if (multiplier < -2) multiplier = -2;
        inputBox.input = std::to_string(multiplier); inputBox.text.setString(inputBox.input); inputBox.text.setFillColor(sf::Color::Black);
        *targetTimeInterval = baseTimeInterval * std::pow(10.0f, multiplier);
    }

    void applyTextInput() {
        if (!inputBox.input.empty() && inputBox.input != "-") {
            try { multiplier = std::stoi(inputBox.input); } catch (...) { multiplier = 0; }
        } else { multiplier = 0; }
        updateTimeInterval();
    }

    void HandleEvent(sf::Event& event, sf::RenderWindow& window) {
        bool wasSelected = inputBox.isSelected;
        inputBox.HandleEvent_IB(event, window);

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (minusBtn.isClicked(mPos)) { multiplier--; updateTimeInterval(); } 
            else if (plusBtn.isClicked(mPos)) { multiplier++; updateTimeInterval(); } 
            else if (wasSelected && !inputBox.isSelected) { applyTextInput(); }
        }
        if (inputBox.isSelected && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
            applyTextInput(); inputBox.isSelected = false; inputBox.box.setOutlineColor(sf::Color::Cyan);
        }
    }
    void setPosition(sf::Vector2f pos) {
        label.setPosition(pos.x, pos.y - 20); minusBtn.setPosition(pos);
        inputBox.setPosition({pos.x + 50, pos.y}); plusBtn.setPosition({pos.x + 110, pos.y});
    }
    void draw(sf::RenderWindow& window) { window.draw(label); minusBtn.draw(window); inputBox.draw(window); plusBtn.draw(window); }
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

