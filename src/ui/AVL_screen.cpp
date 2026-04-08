#include "DSA-Visualization/ui/AVL_Screen.hpp"
#include "DSA-Visualization/ui/components/graphic_node.hpp"
#include <iostream>
#include <cmath>
#include <map>
#include <functional>

const std::vector<std::string> AVLScreen::INSERT_CODE = {
    "insert(value):",
    "  if node == null: return new node",
    "  if value < node: go left",
    "  if value > node: go right",
    "  updateHeight(node)",
    "  if balance > 1: rotateRight",
    "  if balance < -1: rotateLeft",
    "  return node"
};

const std::vector<std::string> AVLScreen::DELETE_CODE = {
    "void remove(value):",
    "  if (node == null): return node",
    "  if value < node: go left",
    "  if value > node: go right",
    "  else (found node):",
    "    if (child_cnt <= 1) replace child",
    "    if (2 child) swap min,delete",
    "  updateHeight(node)",
    "  if balance > 1 or < -1: rotate",
    "  return node"
};

const std::vector<std::string> AVLScreen::SEARCH_CODE = {
    "search(value):",
    "  if node == null: return not_found",
    "  if value < node: go left",
    "  if value > node: go right",
    "  else: return found"
};

static const float NODE_RADIUS   = 28.f;
static const float CANVAS_X      = 230.f;
static const float CANVAS_Y      = 20.f;
static const float CANVAS_W      = 760.f;
static const float CANVAS_H      = 560.f;

AVLScreen::AVLScreen()
: mInputActive(false)
, mCodePanel()
, mSpeedValue(2.f)
, mSliderDragging(false)
, mHistoryIndex(0)
{}

void AVLScreen::buildSteps(Operation op) {
    std::vector<AnimationStep> steps;
    if (op.type == OpType::Insert) {
        mCodePanel.setCode(INSERT_CODE);
        mTree.insert(op.value, &steps);
    } else if (op.type == OpType::Delete) {
        mCodePanel.setCode(DELETE_CODE);
        mTree.remove(op.value, &steps);
    } else if (op.type == OpType::Search) {
        mCodePanel.setCode(SEARCH_CODE);
        mTree.search(op.value, &steps);
    } else if (op.type == OpType::Clear) {
        mCodePanel.setCode({});
        mTree.clear();
        AnimationStep step;
        step.description = "Tree cleared.";
        step.codeLineIndex = -1;
        steps.push_back(step);
    }
    mController.loadSteps(steps);
}

int AVLScreen::run(sf::RenderWindow& window, sf::Font& font) {
    if (!mBgTexture.loadFromFile("assets/textures/avl_background.png"))
        std::cerr << "Failed to load background.png\n";
    sf::Vector2u sz = mBgTexture.getSize();
    mBgSprite.setTexture(mBgTexture);
    mBgSprite.setScale(1280.f / sz.x, 720.f / sz.y);
    mBgSprite.setColor(sf::Color(255, 255, 255, 20)); // ultra-dimmed for the clean look

    mCodePanel = CodePanel(font, sf::Vector2f(8.f, 16.f), sf::Vector2f(230.f, 280.f));
    mCodePanel.setCode(INSERT_CODE);
    
    float ver_align = 1115.f; // Centered in the control panel
    mInsertBtn.emplace("Insert",  font, sf::Vector2f(120.f, 40.f));
    mInsertBtn->setPosition(ver_align,  70.f);
    mDeleteBtn.emplace("Delete",  font, sf::Vector2f(120.f, 40.f));
    mDeleteBtn->setPosition(ver_align, 120.f);
    mSearchBtn.emplace("Search",  font, sf::Vector2f(120.f, 40.f));
    mSearchBtn->setPosition(ver_align, 170.f);
    mRandomBtn.emplace("Random",  font, sf::Vector2f(120.f, 40.f));
    mRandomBtn->setPosition(ver_align, 220.f);
    mClearBtn .emplace("Clear",   font, sf::Vector2f(120.f, 40.f));
    mClearBtn ->setPosition(ver_align, 270.f);
    mPrevBtn  .emplace("< Prev",  font, sf::Vector2f(80.f,  40.f));
    mPrevBtn  ->setPosition(1070.f, 320.f);
    mNextBtn  .emplace("Next >",  font, sf::Vector2f(80.f,  40.f));
    mNextBtn  ->setPosition(1160.f, 320.f);
    mReturnBtn.emplace("Return",  font, sf::Vector2f(120.f, 40.f));
    mReturnBtn->setPosition(ver_align, 660.f);

    // Speed slider track
    mSliderTrack = sf::RectangleShape({180.f, 8.f});
    mSliderTrack.setPosition(1025.f, 385.f);
    mSliderTrack.setFillColor(sf::Color(30, 25, 40));
    mSliderTrack.setOutlineThickness(1.f);
    mSliderTrack.setOutlineColor(sf::Color(181, 58, 199, 50));

    mSliderHandle = sf::CircleShape(10.f);
    mSliderHandle.setOrigin(10.f, 10.f);
    mSliderHandle.setFillColor(sf::Color::White); // Sleek white handle
    updateSliderHandle();

    // 1. Initialize dot grid VertexArray for clean background
    sf::VertexArray dotGrid(sf::Points);
    for (int x = 0; x <= 1280; x += 30) {
        for (int y = 0; y <= 720; y += 30) {
            dotGrid.append(sf::Vertex(sf::Vector2f(x, y), sf::Color(181, 58, 199, 30)));
        }
    }

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Vector2f mouseRaw = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        mInsertBtn->update(mouseRaw);
        mDeleteBtn->update(mouseRaw);
        mSearchBtn->update(mouseRaw);
        mRandomBtn->update(mouseRaw);
        mClearBtn->update(mouseRaw);
        mPrevBtn->update(mouseRaw);
        mNextBtn->update(mouseRaw);
        mReturnBtn->update(mouseRaw);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }

            bool leftPressed = (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);
            if (leftPressed) {
                if (mReturnBtn->isClicked(mouseRaw, true)) return 0;

                sf::FloatRect inputBox(CANVAS_X + 10.f, CANVAS_H + 10.f, 150.f, 42.f);
                mInputActive = inputBox.contains(mouseRaw);

                if (mSliderHandle.getGlobalBounds().contains(mouseRaw))
                    mSliderDragging = true;

                if (mInsertBtn->isClicked(mouseRaw, true) && !mInputString.empty()) {
                    int val = std::stoi(mInputString);
                    if (mHistoryIndex < (int)mHistory.size()) {
                        mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    }
                    Operation op{OpType::Insert, val};
                    mHistory.push_back(op);
                    mHistoryIndex++;
                    buildSteps(op);
                    mInputString.clear();
                }

                if (mDeleteBtn->isClicked(mouseRaw, true) && !mInputString.empty()) {
                    int val = std::stoi(mInputString);
                    if (mHistoryIndex < (int)mHistory.size()) {
                        mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    }
                    Operation op{OpType::Delete, val};
                    mHistory.push_back(op);
                    mHistoryIndex++;
                    buildSteps(op);
                    mInputString.clear();
                }

                if (mSearchBtn->isClicked(mouseRaw, true) && !mInputString.empty()) {
                    int val = std::stoi(mInputString);
                    if (mHistoryIndex < (int)mHistory.size())
                        mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    Operation op{OpType::Search, val};
                    mHistory.push_back(op);
                    mHistoryIndex++;
                    buildSteps(op);
                    mInputString.clear();
                }

                if (mClearBtn->isClicked(mouseRaw, true)) {
                    if (mHistoryIndex < (int)mHistory.size())
                        mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    Operation op{OpType::Clear, 0};
                    mHistory.push_back(op);
                    mHistoryIndex++;
                    buildSteps(op);
                    mInputString.clear();
                }

                if (mRandomBtn->isClicked(mouseRaw, true)) {
                    if (mHistoryIndex < (int)mHistory.size())
                        mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    
                    mHistory.push_back({OpType::Clear, 0});
                    mHistoryIndex++;
                    
                    int count = 5 + (rand() % 6);
                    for (int i = 0; i < count; ++i) {
                        int val = rand() % 100;
                        mHistory.push_back({OpType::Insert, val});
                        mHistoryIndex++;
                    }
                    
                    mTree.clear();
                    mController.clear();
                    for(int i = 0; i < mHistoryIndex - 1; ++i) {
                        if (mHistory[i].type == OpType::Insert)       mTree.insert(mHistory[i].value, nullptr);
                        else if (mHistory[i].type == OpType::Delete)  mTree.remove(mHistory[i].value, nullptr);
                        else if (mHistory[i].type == OpType::Clear)   mTree.clear();
                    }
                    buildSteps(mHistory[mHistoryIndex - 1]);
                    mController.skipToEnd();
                    mInputString.clear();
                }

                if (mPrevBtn->isClicked(mouseRaw, true)) {
                    if (mHistoryIndex > 0) {
                        mHistoryIndex--;
                        mTree.clear();
                        mController.clear();
                        for(int i = 0; i < mHistoryIndex - 1; ++i) {
                            if (mHistory[i].type == OpType::Insert)
                                mTree.insert(mHistory[i].value, nullptr);
                            else if (mHistory[i].type == OpType::Delete)
                                mTree.remove(mHistory[i].value, nullptr);
                            else if (mHistory[i].type == OpType::Clear)
                                mTree.clear();
                        }
                        if (mHistoryIndex > 0) {
                            buildSteps(mHistory[mHistoryIndex - 1]);
                            mController.skipToEnd();
                        }
                    }
                }
                if (mNextBtn->isClicked(mouseRaw, true)) {
                    if (mHistoryIndex < (int)mHistory.size()) {
                        Operation op = mHistory[mHistoryIndex];
                        mHistoryIndex++;
                        buildSteps(op);
                    }
                }
            }
            
            if (event.type == sf::Event::MouseButtonReleased)
                mSliderDragging = false;

            if (mSliderDragging && event.type == sf::Event::MouseMoved) {
                sf::Vector2f mouse = window.mapPixelToCoords(
                    sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                float trackLeft  = 1025.f;
                float trackRight = 1205.f;
                float ratio = (mouse.x - trackLeft) / (trackRight - trackLeft);
                ratio = std::max(0.f, std::min(1.f, ratio));
                mSpeedValue = 0.5f + ratio * 7.5f; // range 0.5x to 5x
                mController.setSpeed(mSpeedValue);
                updateSliderHandle();
            }

            if (mInputActive && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8 && !mInputString.empty())
                    mInputString.pop_back();
                else if (event.text.unicode >= '0' && event.text.unicode <= '9'
                         && mInputString.size() < 4)
                    mInputString += static_cast<char>(event.text.unicode);
            }
        }

        mController.update(dt);

        // 3. Render everything in the correct order
        window.clear(sf::Color(13, 11, 15)); // Charcoal

        window.draw(mBgSprite); 
        window.draw(dotGrid); 

        mCodePanel.highlight(mController.hasSteps()
            ? mController.currentStep()->codeLineIndex : -1);
        mCodePanel.draw(window);
        drawTree(window, font);
        drawControls(window, font);
        drawInputBox(window, font);
        drawDescription(window, font);
        drawSpeedSlider(window, font);
        
        window.display();
    }
    return -1;
}

void AVLScreen::updateSliderHandle() {
    float trackLeft  = 1025.f;
    float trackRight = 1205.f;
    float ratio = (mSpeedValue - 0.5f) / 7.5f;
    mSliderHandle.setPosition(trackLeft + ratio * (trackRight - trackLeft), 389.f);
}

void AVLScreen::drawTree(sf::RenderWindow& window, const sf::Font& font) {
    if (!mController.hasSteps()) return;
    const AnimationStep* step = mController.currentStep();
    float t = mController.t();
    drawEdges(window, step->nodes, t);
    for (const auto& ns : step->nodes)
        drawNode(window, font, ns, t);
}

void AVLScreen::drawNode(sf::RenderWindow& window, const sf::Font& font,
                          const NodeState& ns, float t)
{
    sf::Vector2f pos = ns.startPos + (ns.targetPos - ns.startPos) * t;

    GraphicNode node(NODE_RADIUS, std::to_string(ns.value), font);
    node.setPosition(pos);
    
    // Apply dynamic controller interpolated colors
    node.setFillColor(ns.fillColor);
    node.setOutlineColor(ns.outlineColor);

    window.draw(node);
}

void AVLScreen::drawEdges(sf::RenderWindow& window,
                           const std::vector<NodeState>& nodes, float t)
{
    std::map<int, sf::Vector2f> posMap;
    for (const auto& ns : nodes)
        posMap[ns.value] = ns.startPos + (ns.targetPos - ns.startPos) * t;

    auto drawThickLine = [&](sf::Vector2f p1, sf::Vector2f p2) {
        sf::Vector2f dir = p2 - p1;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len < 0.1f) return;
        float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

        // Glow Line
        sf::RectangleShape glow(sf::Vector2f(len, 7.f));
        glow.setOrigin(0, 3.5f);
        glow.setPosition(p1);
        glow.setFillColor(sf::Color(181, 58, 199, 30));
        glow.setRotation(angle);
        window.draw(glow);

        // Core Line
        sf::RectangleShape line(sf::Vector2f(len, 2.5f));
        line.setOrigin(0, 1.25f);
        line.setPosition(p1);
        line.setFillColor(sf::Color(218, 112, 214, 200));
        line.setRotation(angle);
        window.draw(line);
    };

    for (const auto& ns : nodes) {
        sf::Vector2f parentPos = posMap[ns.value];
        if (ns.leftChild != -1 && posMap.count(ns.leftChild)) {
            drawThickLine(parentPos, posMap[ns.leftChild]);
        }
        if (ns.rightChild != -1 && posMap.count(ns.rightChild)) {
            drawThickLine(parentPos, posMap[ns.rightChild]);
        }
    }
}

void AVLScreen::drawControls(sf::RenderWindow& window, const sf::Font& font) {
    // --- Unified Control Panel Background ---
    sf::ConvexShape bgBox(40); 
    
    bgBox.setFillColor(sf::Color(22, 18, 28, 230));  // Dark glass plum
    bgBox.setOutlineThickness(1.5f);
    bgBox.setOutlineColor(sf::Color(181, 58, 199, 40));

    float boxX = 990.f;  
    float boxY = 35.f;   
    float boxW = 250.f;  
    float boxH = 430.f;  
    float radius = 18.f; 

    const float pi = 3.141592654f;
    for (int i = 0; i < 10; ++i) {
        float step = i * (pi / 2.f) / 9.f;
        bgBox.setPoint(i, sf::Vector2f(
            boxX + boxW - radius + radius * std::cos(step), 
            boxY + radius - radius * std::sin(step)));
        bgBox.setPoint(10 + i, sf::Vector2f(
            boxX + radius + radius * std::cos(step + pi / 2.f), 
            boxY + radius - radius * std::sin(step + pi / 2.f)));
        bgBox.setPoint(20 + i, sf::Vector2f(
            boxX + radius + radius * std::cos(step + pi), 
            boxY + boxH - radius - radius * std::sin(step + pi)));
        bgBox.setPoint(30 + i, sf::Vector2f(
            boxX + boxW - radius + radius * std::cos(step + 3.f * pi / 2.f), 
            boxY + boxH - radius - radius * std::sin(step + 3.f * pi / 2.f)));
    }
    
    window.draw(bgBox);

    window.draw(*mInsertBtn);
    window.draw(*mDeleteBtn);
    window.draw(*mSearchBtn);
    window.draw(*mRandomBtn);
    window.draw(*mClearBtn);
    window.draw(*mPrevBtn);
    window.draw(*mNextBtn);
    window.draw(*mReturnBtn);

    sf::Text counter;
    counter.setFont(font);
    counter.setCharacterSize(13);
    counter.setFillColor(sf::Color(180, 190, 200));
    counter.setString(mController.hasSteps()
        ? std::to_string(mController.currentIndex() + 1) + " / "
          + std::to_string(mController.totalSteps())
        : "0 / 0");
    counter.setPosition(1025.f, 355.f);
    window.draw(counter);
}

void AVLScreen::drawSpeedSlider(sf::RenderWindow& window, const sf::Font& font) {
    sf::Text label;
    label.setFont(font);
    label.setCharacterSize(13);
    label.setLetterSpacing(1.1f);
    label.setFillColor(sf::Color(200, 210, 220));
    label.setString("Speed: " + std::to_string((int)mSpeedValue) + "x");
    label.setPosition(1025.f, 405.f);
    window.draw(label);

    sf::RectangleShape filledTrack({mSliderHandle.getPosition().x - mSliderTrack.getPosition().x, 8.f});
    filledTrack.setPosition(mSliderTrack.getPosition());
    filledTrack.setFillColor(sf::Color(110, 247, 242));

    window.draw(mSliderTrack);
    window.draw(filledTrack);

    sf::CircleShape shadow(10.f);
    shadow.setOrigin(10.f, 10.f);
    shadow.setPosition(mSliderHandle.getPosition() + sf::Vector2f(2.f, 3.f));
    shadow.setFillColor(sf::Color(0, 0, 0, 120));

    window.draw(shadow);
    window.draw(mSliderHandle);
}

void AVLScreen::drawInputBox(sf::RenderWindow& window, const sf::Font& font) {
    sf::RectangleShape box({150.f, 42.f});
    box.setPosition(CANVAS_X + 10.f, CANVAS_H + 10.f);
    box.setFillColor(sf::Color(22, 18, 28, 230));
    box.setOutlineThickness(1.5f);
    box.setOutlineColor(mInputActive ? sf::Color(218, 112, 214) : sf::Color(80, 60, 90));
    window.draw(box);

    // Subtle glow if active
    if (mInputActive) {
        sf::RectangleShape glowBox({150.f, 42.f});
        glowBox.setPosition(CANVAS_X + 10.f, CANVAS_H + 10.f);
        glowBox.setFillColor(sf::Color::Transparent);
        glowBox.setOutlineThickness(4.f);
        glowBox.setOutlineColor(sf::Color(218, 112, 214, 40));
        window.draw(glowBox);
    }

    sf::Text inputText;
    inputText.setFont(font);
    inputText.setString(mInputString.empty() ? "value..." : mInputString);
    inputText.setCharacterSize(16);
    inputText.setLetterSpacing(1.2f);
    inputText.setFillColor(mInputString.empty()
        ? sf::Color(100, 110, 130) : sf::Color::White);
    inputText.setPosition(CANVAS_X + 24.f, CANVAS_H + 20.f);
    window.draw(inputText);
}

void AVLScreen::drawDescription(sf::RenderWindow& window, const sf::Font& font) {
    if (!mController.hasSteps()) return;

    sf::RectangleShape shadow({CANVAS_W - 30.f, 54.f});
    shadow.setPosition(CANVAS_X + 24.f, CANVAS_H + 68.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 90));
    window.draw(shadow);

    sf::RectangleShape bar({CANVAS_W - 30.f, 54.f});
    bar.setPosition(CANVAS_X + 20.f, CANVAS_H + 64.f);
    bar.setFillColor(sf::Color(22, 18, 28, 240));
    bar.setOutlineThickness(1.5f);
    bar.setOutlineColor(sf::Color(181, 58, 199, 60));
    window.draw(bar);

    // Accent line
    sf::RectangleShape accent({4.f, 54.f});
    accent.setPosition(CANVAS_X + 20.f, CANVAS_H + 64.f);
    accent.setFillColor(sf::Color(218, 112, 214));
    window.draw(accent);

    sf::Text desc;
    desc.setFont(font);
    desc.setString(mController.currentStep()->description);
    desc.setCharacterSize(18);
    desc.setLetterSpacing(1.1f);
    desc.setFillColor(sf::Color(240, 245, 255));
    desc.setPosition(CANVAS_X + 38.f, CANVAS_H + 79.f);
    window.draw(desc);
}