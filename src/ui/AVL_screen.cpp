#include "DSA-Visualization/ui/AVL_Screen.hpp"
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

void AVLScreen::buildInsertSteps(int value) {
    std::vector<AnimationStep> steps;
    mTree.insert(value, &steps);
    mController.loadSteps(steps);
}

int AVLScreen::run(sf::RenderWindow& window, sf::Font& font) {
    if (!mBgTexture.loadFromFile("assets/textures/background.png"))
        std::cerr << "Failed to load background.png\n";
    sf::Vector2u sz = mBgTexture.getSize();
    mBgSprite.setTexture(mBgTexture);
    mBgSprite.setScale(1280.f / sz.x, 720.f / sz.y);

    mCodePanel = CodePanel(font, sf::Vector2f(16.f, 16.f), sf::Vector2f(200.f, 220.f));
    mCodePanel.setCode(INSERT_CODE);

    mInsertBtn.emplace("Insert",  font, sf::Vector2f(1010.f,  80.f), sf::Vector2f(120.f, 40.f));
    mPrevBtn  .emplace("< Prev",  font, sf::Vector2f(1010.f, 140.f), sf::Vector2f(55.f,  40.f));
    mNextBtn  .emplace("Next >",  font, sf::Vector2f(1075.f, 140.f), sf::Vector2f(55.f,  40.f));
    mReturnBtn.emplace("Return",  font, sf::Vector2f(1010.f, 660.f), sf::Vector2f(120.f, 40.f));

    // Speed slider track
    mSliderTrack = sf::RectangleShape({120.f, 6.f});
    mSliderTrack.setPosition(1010.f, 220.f);
    mSliderTrack.setFillColor(sf::Color(80, 80, 100));

    mSliderHandle = sf::CircleShape(8.f);
    mSliderHandle.setOrigin(8.f, 8.f);
    mSliderHandle.setFillColor(sf::Color(70, 130, 180));
    updateSliderHandle();

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }

            if (mReturnBtn->isClicked(event, window)) return 0;

            // Input box click
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse = window.mapPixelToCoords(
                    sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                sf::FloatRect inputBox(CANVAS_X + 10.f, CANVAS_H + 10.f, 120.f, 36.f);
                mInputActive = inputBox.contains(mouse);

                // Slider drag start
                if (mSliderHandle.getGlobalBounds().contains(mouse))
                    mSliderDragging = true;
            }

            if (event.type == sf::Event::MouseButtonReleased)
                mSliderDragging = false;

            if (mSliderDragging && event.type == sf::Event::MouseMoved) {
                sf::Vector2f mouse = window.mapPixelToCoords(
                    sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                float trackLeft  = 1010.f;
                float trackRight = 1130.f;
                float ratio = (mouse.x - trackLeft) / (trackRight - trackLeft);
                ratio = std::max(0.f, std::min(1.f, ratio));
                mSpeedValue = 0.5f + ratio * 4.5f; // range 0.5x to 5x
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

            if (mInsertBtn->isClicked(event, window) && !mInputString.empty()) {
                int val = std::stoi(mInputString);
                
                if (mHistoryIndex < (int)mInsertionHistory.size()) {
                    mInsertionHistory.erase(mInsertionHistory.begin() + mHistoryIndex, mInsertionHistory.end());
                }
                mInsertionHistory.push_back(val);
                mHistoryIndex++;
                
                buildInsertSteps(val);
                mInputString.clear();
            }

            if (mPrevBtn->isClicked(event, window)) {
                if (mHistoryIndex > 0) {
                    mHistoryIndex--;
                    mTree.clear();
                    mController.clear();
                    for(int i = 0; i < mHistoryIndex - 1; ++i) {
                        mTree.insert(mInsertionHistory[i], nullptr);
                    }
                    if (mHistoryIndex > 0) {
                        buildInsertSteps(mInsertionHistory[mHistoryIndex - 1]);
                        mController.skipToEnd();
                    }
                }
            }
            if (mNextBtn->isClicked(event, window)) {
                if (mHistoryIndex < (int)mInsertionHistory.size()) {
                    int val = mInsertionHistory[mHistoryIndex];
                    mHistoryIndex++;
                    buildInsertSteps(val);
                }
            }
        }

        mController.update(dt);

        window.clear();
        window.draw(mBgSprite);
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
    float trackLeft  = 1010.f;
    float trackRight = 1130.f;
    float ratio = (mSpeedValue - 0.5f) / 4.5f;
    mSliderHandle.setPosition(trackLeft + ratio * (trackRight - trackLeft), 223.f);
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

    sf::CircleShape circle(NODE_RADIUS);
    circle.setOrigin(NODE_RADIUS, NODE_RADIUS);
    circle.setPosition(pos);
    circle.setFillColor(ns.fillColor);
    circle.setOutlineThickness(2.f);
    circle.setOutlineColor(ns.outlineColor);
    window.draw(circle);

    sf::Text valText;
    valText.setFont(font);
    valText.setString(std::to_string(ns.value));
    valText.setCharacterSize(16);
    valText.setFillColor(sf::Color::White);
    sf::FloatRect vb = valText.getLocalBounds();
    valText.setOrigin(vb.left + vb.width / 2.f, vb.top + vb.height / 2.f);
    valText.setPosition(pos);
    window.draw(valText);
}

void AVLScreen::drawEdges(sf::RenderWindow& window,
                           const std::vector<NodeState>& nodes, float t)
{
    // Build position map from the step's own node data
    std::map<int, sf::Vector2f> posMap;
    for (const auto& ns : nodes)
        posMap[ns.value] = ns.startPos + (ns.targetPos - ns.startPos) * t;

    // Draw edges using leftChild/rightChild stored in each NodeState
    for (const auto& ns : nodes) {
        sf::Vector2f parentPos = posMap[ns.value];
        if (ns.leftChild != -1 && posMap.count(ns.leftChild)) {
            sf::Vertex line[] = {
                sf::Vertex(parentPos,              sf::Color(200, 200, 200)),
                sf::Vertex(posMap[ns.leftChild],   sf::Color(200, 200, 200))
            };
            window.draw(line, 2, sf::Lines);
        }
        if (ns.rightChild != -1 && posMap.count(ns.rightChild)) {
            sf::Vertex line[] = {
                sf::Vertex(parentPos,              sf::Color(200, 200, 200)),
                sf::Vertex(posMap[ns.rightChild],  sf::Color(200, 200, 200))
            };
            window.draw(line, 2, sf::Lines);
        }
    }
}

void AVLScreen::drawControls(sf::RenderWindow& window, const sf::Font& font) {
    mInsertBtn->draw(window);
    mPrevBtn->draw(window);
    mNextBtn->draw(window);
    mReturnBtn->draw(window);

    sf::Text counter;
    counter.setFont(font);
    counter.setCharacterSize(13);
    counter.setFillColor(sf::Color::White);
    counter.setString(mController.hasSteps()
        ? std::to_string(mController.currentIndex() + 1) + " / "
          + std::to_string(mController.totalSteps())
        : "0 / 0");
    counter.setPosition(1010.f, 192.f);
    window.draw(counter);
}

void AVLScreen::drawSpeedSlider(sf::RenderWindow& window, const sf::Font& font) {
    sf::Text label;
    label.setFont(font);
    label.setCharacterSize(12);
    label.setFillColor(sf::Color::White);
    label.setString("Speed: " + std::to_string((int)mSpeedValue) + "x");
    label.setPosition(1010.f, 235.f);
    window.draw(label);

    window.draw(mSliderTrack);
    window.draw(mSliderHandle);
}

void AVLScreen::drawInputBox(sf::RenderWindow& window, const sf::Font& font) {
    sf::RectangleShape box({120.f, 36.f});
    box.setPosition(CANVAS_X + 10.f, CANVAS_H + 10.f);
    box.setFillColor(sf::Color(30, 30, 40));
    box.setOutlineThickness(mInputActive ? 2.f : 1.f);
    box.setOutlineColor(mInputActive ? sf::Color::Cyan : sf::Color(100, 100, 120));
    window.draw(box);

    sf::Text inputText;
    inputText.setFont(font);
    inputText.setString(mInputString.empty() ? "value..." : mInputString);
    inputText.setCharacterSize(14);
    inputText.setFillColor(mInputString.empty()
        ? sf::Color(100, 100, 120) : sf::Color::White);
    inputText.setPosition(CANVAS_X + 16.f, CANVAS_H + 16.f);
    window.draw(inputText);
}

void AVLScreen::drawDescription(sf::RenderWindow& window, const sf::Font& font) {
    if (!mController.hasSteps()) return;

    sf::RectangleShape bar({CANVAS_W, 36.f});
    bar.setPosition(CANVAS_X, CANVAS_H + 60.f);
    bar.setFillColor(sf::Color(30, 30, 40, 200));
    window.draw(bar);

    sf::Text desc;
    desc.setFont(font);
    desc.setString(mController.currentStep()->description);
    desc.setCharacterSize(14);
    desc.setFillColor(sf::Color::White);
    desc.setPosition(CANVAS_X + 10.f, CANVAS_H + 68.f);
    window.draw(desc);
}