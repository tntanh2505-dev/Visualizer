#include "DSA-Visualization/ui/AVL_Screen.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"
#include "DSA-Visualization/ui/components/graphic_node.hpp"
#include <iostream>
#include <cmath>
#include <map>
#include <functional>
#include <fstream> 

const float fset = 40.f;//family offset #1
const float fset2 = 40.f; //family offset #2

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

static const float NODE_RADIUS   = UITheme::Size::NodeRadius;
static const float CANVAS_H      = 560.f; // Base height reference for description box

AVLScreen::AVLScreen()
: mInputActive(false)
, mCodePanel()
, mSpeedValue(2.f)
, mSliderDragging(false)
, mHistoryIndex(0)
, m_leftWidth(35.f)
, m_rightWidth(35.f)
, m_leftExpanded(false)
, m_rightExpanded(false)
, m_selectedNodeValue(-1)
{}

void AVLScreen::buildSteps(Operation op) {
    std::vector<AnimationStep> steps;
    if (op.type == OpType::Insert) {
        mCodePanel.update("insert", -1);
        mTree.insert(op.value, &steps);
    } else if (op.type == OpType::Delete) {
        mCodePanel.update("remove", -1);
        mTree.remove(op.value, &steps);
    } else if (op.type == OpType::Search) {
        mCodePanel.update("search", -1);
        mTree.search(op.value, &steps);
    } else if (op.type == OpType::Clear) {
        mCodePanel.update("", -1);
        mTree.clear();
        AnimationStep step;
        step.description = "Tree cleared.";
        step.codeLineIndex = -1;
        steps.push_back(step);
    } else if (op.type == OpType::Update) {
        mCodePanel.update("insert", -1);
        mTree.remove(op.oldValue, nullptr);  // silent delete
        mTree.insert(op.value, &steps);       // insert with animation
    }
    mController.loadSteps(steps);
}

int AVLScreen::run(sf::RenderWindow& window, sf::Font& font) {
    if (!mBgTexture.loadFromFile("assets/textures/avl_background.png"))
        std::cerr << "Failed to load background.png\n";
    sf::Vector2u sz = mBgTexture.getSize();
    mBgSprite.setTexture(mBgTexture);
    mBgSprite.setScale(static_cast<float>(window.getSize().x) / sz.x, 
                       static_cast<float>(window.getSize().y) / sz.y);
    
    mBgSprite.setColor(UITheme::Color::AVLBackground);

    // Initialize UI Components dynamically
    mCodePanel = CodePanel(font, sf::Vector2f(0.f, 0.f), sf::Vector2f(330.f, 350.f));
    
    std::map<std::string, std::vector<std::string>> avlSnippets;
    avlSnippets["insert"] = INSERT_CODE;
    avlSnippets["remove"] = DELETE_CODE;
    avlSnippets["search"] = SEARCH_CODE;
    mCodePanel.loadSnippets(avlSnippets);
    mCodePanel.update("insert", -1);
    
    mInsertBtn.emplace("Insert",  font, sf::Vector2f(100.f, 40.f));
    mDeleteBtn.emplace("Delete",  font, sf::Vector2f(100.f, 40.f));
    mSearchBtn.emplace("Search",  font, sf::Vector2f(100.f, 40.f));
    mUpdateBtn.emplace("Update",  font, sf::Vector2f(100.f,40.f));

    mRandomBtn.emplace("Random",  font, sf::Vector2f(100.f, 40.f));
    mClearBtn .emplace("Clear",   font, sf::Vector2f(100.f, 40.f));
    mLoadFileBtn.emplace("Load File", font, sf::Vector2f(210.f, 40.f));

    mPrevBtn  .emplace("< Prev",  font, sf::Vector2f(100.f,  40.f));
    mNextBtn  .emplace("Next >",  font, sf::Vector2f(100.f,  40.f));
    mSkipAnimationBtn.emplace("Skip Animation", font, sf::Vector2f(210.f, 40.f));

    mReturnBtn.emplace("Return",  font, sf::Vector2f(210.f, 40.f));
    
    mSliderTrack = sf::RectangleShape({220.f, 6.f});
    mSliderTrack.setFillColor(UITheme::Color::SliderTrack);
    mSliderTrack.setOutlineThickness(1.f);
    mSliderTrack.setOutlineColor(UITheme::Color::AVLGlow);

    mSliderHandle = sf::CircleShape(10.f);
    mSliderHandle.setOrigin(10.f, 10.f);
    mSliderHandle.setFillColor(UITheme::Color::SliderHandle); 

    sf::VertexArray dotGrid(sf::Points);
    for (int x = 0; x <= window.getSize().x; x += 30) {
        for (int y = 0; y <= window.getSize().y; y += 30) {
            dotGrid.append(sf::Vertex(sf::Vector2f(x, y), UITheme::Color::AVLGlow));
        }
    }

    const float LEFT_PANEL_WIDTH = 280.f;
    const float RIGHT_PANEL_WIDTH = 380.f;
    const float TAB_WIDTH = 35.f;
    const float TAB_HEIGHT = 50.f;

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Vector2f mouseRaw = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }

            bool leftPressed = (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);
            
            if (leftPressed) {
                float centerY = window.getSize().y / 2.f;
                float winW = window.getSize().x;
                
                bool mouseInLeftTab = (mouseRaw.x >= m_leftWidth - TAB_WIDTH && mouseRaw.x <= m_leftWidth &&
                                       mouseRaw.y >= centerY - TAB_HEIGHT / 2.f && mouseRaw.y <= centerY + TAB_HEIGHT / 2.f);
                if (mouseInLeftTab) { m_leftExpanded = !m_leftExpanded; continue; }

                bool mouseInRightTab = (mouseRaw.x >= winW - m_rightWidth && mouseRaw.x <= winW - m_rightWidth + TAB_WIDTH &&
                                        mouseRaw.y >= centerY - TAB_HEIGHT / 2.f && mouseRaw.y <= centerY + TAB_HEIGHT / 2.f);
                if (mouseInRightTab) { m_rightExpanded = !m_rightExpanded; continue; }
            }

            float rightPanelHeight = std::max(window.getSize().y / 2.f + 25.f, 20.f + fset + 350.f + 20.f);
            bool isClickingOnPanel = (mouseRaw.x < m_leftWidth) || 
                                     (mouseRaw.x > window.getSize().x - m_rightWidth && mouseRaw.y < rightPanelHeight);

            if (leftPressed && isClickingOnPanel) {
                if (mReturnBtn->isClicked(mouseRaw, true)) return 0;

                float leftBaseX = m_leftWidth - LEFT_PANEL_WIDTH;
                sf::FloatRect inputBox(leftBaseX + 30.f, 30.f, 220.f, 42.f);
                mInputActive = inputBox.contains(mouseRaw);

                if (mSliderHandle.getGlobalBounds().contains(mouseRaw) || mSliderTrack.getGlobalBounds().contains(mouseRaw))
                    mSliderDragging = true;

                if (mInsertBtn->isClicked(mouseRaw, true) && !mInputString.empty()) {
                    int val = std::stoi(mInputString);
                    if (mHistoryIndex < (int)mHistory.size()) mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    Operation op{OpType::Insert, val};
                    mHistory.push_back(op); mHistoryIndex++;
                    buildSteps(op); mInputString.clear();
                    m_selectedNodeValue = -1;
                }

                if (mDeleteBtn->isClicked(mouseRaw, true) && !mInputString.empty()) {
                    int val = std::stoi(mInputString);
                    if (mHistoryIndex < (int)mHistory.size()) mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    Operation op{OpType::Delete, val};
                    mHistory.push_back(op); mHistoryIndex++;
                    buildSteps(op); mInputString.clear();
                    m_selectedNodeValue = -1;
                }

                if (mSearchBtn->isClicked(mouseRaw, true) && !mInputString.empty()) {
                    int val = std::stoi(mInputString);
                    if (mHistoryIndex < (int)mHistory.size()) mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    Operation op{OpType::Search, val};
                    mHistory.push_back(op); mHistoryIndex++;
                    buildSteps(op); mInputString.clear();
                    m_selectedNodeValue = -1;
                }

                if (mUpdateBtn->isClicked(mouseRaw, true) && !mInputString.empty() && m_selectedNodeValue != -1) {
                    int newVal = std::stoi(mInputString);
                    int oldVal = m_selectedNodeValue;
                    if (mHistoryIndex < (int)mHistory.size()) mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    Operation op{OpType::Update, newVal, oldVal};
                    mHistory.push_back(op); mHistoryIndex++;
                    buildSteps(op);
                    mController.skipToEnd();
                    mInputString.clear();
                    m_selectedNodeValue = -1;
                }

                if (mClearBtn->isClicked(mouseRaw, true)) {
                    if (mHistoryIndex < (int)mHistory.size()) mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    Operation op{OpType::Clear, 0};
                    mHistory.push_back(op); mHistoryIndex++;
                    buildSteps(op); mInputString.clear();
                    m_selectedNodeValue = -1;
                }

                if (mRandomBtn->isClicked(mouseRaw, true)) {
                    if (mHistoryIndex < (int)mHistory.size()) mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    mHistory.push_back({OpType::Clear, 0}); mHistoryIndex++;
                    
                    int count = 5 + (rand() % 6);
                    for (int i = 0; i < count; ++i) {
                        mHistory.push_back({OpType::Insert, rand() % 100}); mHistoryIndex++;
                    }
                    
                    mTree.clear(); mController.clear();
                    for(int i = 0; i < mHistoryIndex - 1; ++i) {
                        if (mHistory[i].type == OpType::Insert)       mTree.insert(mHistory[i].value, nullptr);
                        else if (mHistory[i].type == OpType::Delete)  mTree.remove(mHistory[i].value, nullptr);
                        else if (mHistory[i].type == OpType::Clear)   mTree.clear();
                        else if (mHistory[i].type == OpType::Update) { mTree.remove(mHistory[i].oldValue, nullptr); mTree.insert(mHistory[i].value, nullptr); }
                    }
                    buildSteps(mHistory[mHistoryIndex - 1]);
                    mController.skipToEnd(); mInputString.clear();
                    m_selectedNodeValue = -1;
                }

                if (mPrevBtn->isClicked(mouseRaw, true)) {
                    if (mHistoryIndex > 0) {
                        mHistoryIndex--;
                        mTree.clear(); mController.clear();
                        for(int i = 0; i < mHistoryIndex - 1; ++i) {
                            if (mHistory[i].type == OpType::Insert)       mTree.insert(mHistory[i].value, nullptr);
                            else if (mHistory[i].type == OpType::Delete)  mTree.remove(mHistory[i].value, nullptr);
                            else if (mHistory[i].type == OpType::Clear)   mTree.clear();
                            else if (mHistory[i].type == OpType::Update) { mTree.remove(mHistory[i].oldValue, nullptr); mTree.insert(mHistory[i].value, nullptr); }
                        }
                        if (mHistoryIndex > 0) {
                            buildSteps(mHistory[mHistoryIndex - 1]); mController.skipToEnd();
                        }
                        m_selectedNodeValue = -1;
                    }
                }
                
                if (mNextBtn->isClicked(mouseRaw, true)) {
                    if (mHistoryIndex < (int)mHistory.size()) {
                        Operation op = mHistory[mHistoryIndex]; mHistoryIndex++;
                        buildSteps(op);
                        m_selectedNodeValue = -1;
                    }
                }

                if (mSkipAnimationBtn->isClicked(mouseRaw, true)){
                    if (mHistoryIndex > 0) {
                        buildSteps(mHistory[mHistoryIndex - 1]); mController.skipToEnd();
                    }
                }

                if (mLoadFileBtn->isClicked(mouseRaw, true)) {
                    std::ifstream file("data/AVL_tree.txt");
                    if (file.is_open()) {
                        if (mHistoryIndex < (int)mHistory.size()) mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                        mHistory.push_back({OpType::Clear, 0}); mHistoryIndex++;
                        int val;
                        while (file >> val) { mHistory.push_back({OpType::Insert, val}); mHistoryIndex++; }
                        
                        mTree.clear(); mController.clear();
                        for(int i = 0; i < mHistoryIndex - 1; ++i) {
                            if (mHistory[i].type == OpType::Insert)       mTree.insert(mHistory[i].value, nullptr);
                            else if (mHistory[i].type == OpType::Delete)  mTree.remove(mHistory[i].value, nullptr);
                            else if (mHistory[i].type == OpType::Clear)   mTree.clear();
                            else if (mHistory[i].type == OpType::Update) { mTree.remove(mHistory[i].oldValue, nullptr); mTree.insert(mHistory[i].value, nullptr); }
                        }
                        if (mHistoryIndex > 0) {
                            buildSteps(mHistory[mHistoryIndex - 1]); mController.skipToEnd();
                        }
                        mInputString.clear();
                        m_selectedNodeValue = -1;
                    }
                }
            }

            // --- Node click detection on the canvas ---
            if (leftPressed && !isClickingOnPanel && mController.hasSteps()) {
                float canvasCenter = m_leftWidth + (window.getSize().x - m_leftWidth - m_rightWidth) / 2.f;
                float hitShiftX = canvasCenter - 610.f;
                float curT = mController.t();
                const auto* step = mController.currentStep();

                int hitNode = -1;
                for (const auto& ns : step->nodes) {
                    sf::Vector2f pos = ns.startPos + (ns.targetPos - ns.startPos) * curT;
                    pos.x += hitShiftX;
                    float dx = mouseRaw.x - pos.x;
                    float dy = mouseRaw.y - pos.y;
                    if (dx * dx + dy * dy <= NODE_RADIUS * NODE_RADIUS) {
                        hitNode = ns.value;
                        break;
                    }
                }
                m_selectedNodeValue = hitNode;
            }
            
            if (event.type == sf::Event::MouseButtonReleased) mSliderDragging = false;

            if (mInputActive && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8 && !mInputString.empty())
                    mInputString.pop_back();
                else if ((event.text.unicode >= '0' && event.text.unicode <= '9' || event.text.unicode == '-') && mInputString.size() < 4)
                    mInputString += static_cast<char>(event.text.unicode);
            }
        }
        
        // --- Smooth Layout Mathematics ---
        float targetLeft = m_leftExpanded ? LEFT_PANEL_WIDTH : TAB_WIDTH;
        m_leftWidth += (targetLeft - m_leftWidth) * 12.f * dt;
        
        float targetRight = m_rightExpanded ? RIGHT_PANEL_WIDTH : TAB_WIDTH;
        m_rightWidth += (targetRight - m_rightWidth) * 12.f * dt;

        float leftBaseX = m_leftWidth - LEFT_PANEL_WIDTH;
        float rightBaseX = window.getSize().x - m_rightWidth;

        // Sync UI positions dynamically 
        mInsertBtn->setPosition(sf::Vector2f(leftBaseX + 85.f, 100.f));
        mDeleteBtn->setPosition(sf::Vector2f(leftBaseX + 195.f, 100.f));
        mSearchBtn->setPosition(sf::Vector2f(leftBaseX + 85.f, 150.f));
        mUpdateBtn->setPosition(sf::Vector2f(leftBaseX + 195.f,150.f));

        //family offset
        mRandomBtn->setPosition(sf::Vector2f(leftBaseX + 85.f, 210.f + fset));
        mClearBtn ->setPosition(sf::Vector2f(leftBaseX + 195.f, 210.f + fset));
        mLoadFileBtn->setPosition(sf::Vector2f(leftBaseX + 140.f, 260.f + fset));

        mPrevBtn  ->setPosition(sf::Vector2f(leftBaseX + 85.f, 320.f + fset));
        mNextBtn  ->setPosition(sf::Vector2f(leftBaseX + 195.f, 320.f + fset));
        mSkipAnimationBtn->setPosition(sf::Vector2f(leftBaseX + 140.f, 370.f + fset));

        mSliderTrack.setPosition(leftBaseX + 30.f, 430.f + fset);
        
        // Slider Logic updates seamlessly during drag
        if (mSliderDragging) {
            float trackLeft  = leftBaseX + 30.f;
            float trackRight = leftBaseX + 250.f;
            float ratio = (mouseRaw.x - trackLeft) / (trackRight - trackLeft);
            ratio = std::max(0.f, std::min(1.f, ratio));
            mSpeedValue = 0.5f + ratio * 7.5f; 
            mController.setSpeed(mSpeedValue);
        }
        float ratio = (mSpeedValue - 0.5f) / 7.5f;
        mSliderHandle.setPosition(leftBaseX + 30.f + ratio * 220.f, 433.f + fset);

        mReturnBtn->setPosition(sf::Vector2f(leftBaseX + 140.f, window.getSize().y - 40.f));
        mCodePanel.setPosition(sf::Vector2f(rightBaseX + TAB_WIDTH + 10.f, 20.f + fset));

        // Update hovers
        mInsertBtn->update(mouseRaw); mDeleteBtn->update(mouseRaw); mSearchBtn->update(mouseRaw);
        mUpdateBtn->update(mouseRaw);

        mRandomBtn->update(mouseRaw); mClearBtn->update(mouseRaw); mLoadFileBtn->update(mouseRaw);
        mPrevBtn->update(mouseRaw); mNextBtn->update(mouseRaw); mSkipAnimationBtn->update(mouseRaw);
        mReturnBtn->update(mouseRaw);

        mController.update(dt);

        window.clear(UITheme::Color::AVLBackground); 
        window.draw(mBgSprite); 
        window.draw(dotGrid); 

        // Dynamic Tree Centering
        float newCenter = m_leftWidth + (window.getSize().x - m_leftWidth - m_rightWidth) / 2.f;
        float shiftX = newCenter - 610.f; // Base 610 represents center of old 760w layout

        drawTree(window, font, shiftX);
        drawDescription(window, font, shiftX);

        drawLeftPanel(window, font, leftBaseX);
        drawRightPanel(window, font, rightBaseX);
        
        window.display();
    }
    return -1;
}

void AVLScreen::drawLeftPanel(sf::RenderWindow& window, const sf::Font& font, float leftBaseX) {
    sf::RectangleShape leftMenu(sf::Vector2f(m_leftWidth, window.getSize().y));
    leftMenu.setFillColor(UITheme::Color::AVLPanelBg);
    window.draw(leftMenu);

    float centerY = window.getSize().y / 2.f;
    sf::RectangleShape leftTab(sf::Vector2f(35.f, 50.f));
    leftTab.setFillColor(UITheme::Color::GraphTabBg);
    leftTab.setPosition(m_leftWidth - 35.f, centerY - 25.f);
    window.draw(leftTab);

    sf::Text lIcon(m_leftExpanded ? "<<" : ">>", font, 18);
    sf::FloatRect lb = lIcon.getLocalBounds();
    lIcon.setOrigin(lb.left + lb.width/2.f, lb.top + lb.height/2.f);
    lIcon.setPosition(m_leftWidth - 17.5f, centerY - 2.f);
    lIcon.setFillColor(UITheme::Color::NodeOutlineColor);
    window.draw(lIcon);

    if (m_leftWidth > 50.f) {
        // Dynamic Input Box inside panel
        sf::RectangleShape box({220.f, 42.f});
        box.setPosition(leftBaseX + 30.f, 30.f);
        box.setFillColor(UITheme::Color::CodePanelBg);
        box.setOutlineThickness(1.5f);
        box.setOutlineColor(mInputActive ? UITheme::Color::AVLAccent : UITheme::Color::ModernBtnBorder);
        window.draw(box);

        if (mInputActive) {
            sf::RectangleShape glowBox({220.f, 42.f});
            glowBox.setPosition(leftBaseX + 30.f, 30.f);
            glowBox.setFillColor(sf::Color::Transparent);
            glowBox.setOutlineThickness(3.f);
            glowBox.setOutlineColor(UITheme::Color::AVLGlow);
            window.draw(glowBox);
        }

        sf::Text inputText;
        inputText.setFont(font);
        inputText.setString(mInputString.empty() ? "value..." : mInputString + (mInputActive ? "|" : ""));
        inputText.setCharacterSize(16);
        inputText.setLetterSpacing(1.2f);
        inputText.setFillColor(mInputString.empty() ? sf::Color(100, 110, 130) : sf::Color::White);
        inputText.setPosition(leftBaseX + 40.f, 40.f);
        window.draw(inputText);

        window.draw(*mInsertBtn);
        window.draw(*mDeleteBtn);
        window.draw(*mSearchBtn);
        window.draw(*mUpdateBtn);

        window.draw(*mRandomBtn);
        window.draw(*mClearBtn);
        window.draw(*mLoadFileBtn);

        window.draw(*mPrevBtn);
        window.draw(*mNextBtn);
        window.draw(*mSkipAnimationBtn);
        
        window.draw(*mReturnBtn);

        sf::Text speedLabel;
        speedLabel.setFont(font);
        speedLabel.setCharacterSize(13);
        speedLabel.setFillColor(UITheme::Color::AVLSpeedSliderText);
        speedLabel.setString("Speed: " + std::to_string((int)mSpeedValue) + "x");
        speedLabel.setPosition(leftBaseX + 30.f, 405.f + fset);
        window.draw(speedLabel);

        sf::RectangleShape filledTrack({mSliderHandle.getPosition().x - mSliderTrack.getPosition().x, 6.f});
        filledTrack.setPosition(mSliderTrack.getPosition());
        filledTrack.setFillColor(UITheme::Color::AVLSliderFill);

        window.draw(mSliderTrack);
        window.draw(filledTrack);
        window.draw(mSliderHandle);

        sf::Text counter;
        counter.setFont(font);
        counter.setCharacterSize(13);
        counter.setFillColor(sf::Color(180, 190, 200));
        counter.setString(mController.hasSteps()
            ? "Step: " + std::to_string(mController.currentIndex() + 1) + " / " + std::to_string(mController.totalSteps())
            : "Step: 0 / 0");
        counter.setPosition(leftBaseX + 30.f, 455.f + fset);
        window.draw(counter);
    }
}

void AVLScreen::drawRightPanel(sf::RenderWindow& window, const sf::Font& font, float rightBaseX) {
    float winW = window.getSize().x;
    
    float centerY = window.getSize().y / 2.f;
    float panelHeight = std::max(centerY + 25.f, 20.f + fset + 350.f + 20.f);
    
    sf::RectangleShape rightMenu(sf::Vector2f(m_rightWidth, panelHeight));
    rightMenu.setFillColor(UITheme::Color::AVLPanelBg);
    rightMenu.setPosition(winW - m_rightWidth, 0);
    window.draw(rightMenu);

    sf::RectangleShape rightTab(sf::Vector2f(35.f, 50.f));
    rightTab.setFillColor(UITheme::Color::GraphTabBg);
    rightTab.setPosition(winW - m_rightWidth, centerY - 25.f);
    window.draw(rightTab);

    sf::Text rIcon(m_rightExpanded ? ">>" : "<<", font, 18);
    sf::FloatRect rb = rIcon.getLocalBounds();
    rIcon.setOrigin(rb.left + rb.width/2.f, rb.top + rb.height/2.f);
    rIcon.setPosition(winW - m_rightWidth + 17.5f, centerY - 2.f);
    rIcon.setFillColor(UITheme::Color::NodeHighlightColor);
    window.draw(rIcon);

    mCodePanel.highlight(mController.hasSteps() ? mController.currentStep()->codeLineIndex : -1);
    const_cast<CodePanel&>(mCodePanel).draw(window);
}

void AVLScreen::drawTree(sf::RenderWindow& window, const sf::Font& font, float shiftX) {
    if (!mController.hasSteps()) return;
    const AnimationStep* step = mController.currentStep();
    float t = mController.t();
    drawEdges(window, step->nodes, t, shiftX);
    for (const auto& ns : step->nodes)
        drawNode(window, font, ns, t, shiftX);
}

void AVLScreen::drawNode(sf::RenderWindow& window, const sf::Font& font,
                          const NodeState& ns, float t, float shiftX)
{
    sf::Vector2f pos = ns.startPos + (ns.targetPos - ns.startPos) * t;
    pos.x += shiftX;

    // Faint glow behind the selected node
    if (ns.value == m_selectedNodeValue) {
        sf::CircleShape glow(NODE_RADIUS + 8.f);
        glow.setOrigin(NODE_RADIUS + 8.f, NODE_RADIUS + 8.f);
        glow.setPosition(pos);
        glow.setFillColor(sf::Color(181, 58, 199, 55));
        window.draw(glow);

        sf::CircleShape glowInner(NODE_RADIUS + 4.f);
        glowInner.setOrigin(NODE_RADIUS + 4.f, NODE_RADIUS + 4.f);
        glowInner.setPosition(pos);
        glowInner.setFillColor(sf::Color(181, 58, 199, 35));
        window.draw(glowInner);
    }

    GraphicNode node(NODE_RADIUS, std::to_string(ns.value), font);
    node.setPosition(pos);
    node.setFillColor(ns.fillColor);
    node.setOutlineColor(ns.outlineColor);
    window.draw(node);
}

void AVLScreen::drawEdges(sf::RenderWindow& window,
                           const std::vector<NodeState>& nodes, float t, float shiftX)
{
    std::map<int, sf::Vector2f> posMap;
    for (const auto& ns : nodes) {
        sf::Vector2f pos = ns.startPos + (ns.targetPos - ns.startPos) * t;
        pos.x += shiftX;
        posMap[ns.value] = pos;
    }

    auto drawThickLine = [&](sf::Vector2f p1, sf::Vector2f p2) {
        sf::Vector2f dir = p2 - p1;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len < 0.1f) return;
        float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

        sf::RectangleShape glow(sf::Vector2f(len, 7.f));
        glow.setOrigin(0, 3.5f);
        glow.setPosition(p1);
        glow.setFillColor(UITheme::Color::AVLGlow);
        glow.setRotation(angle);
        window.draw(glow);

        sf::RectangleShape line(sf::Vector2f(len, 2.5f));
        line.setOrigin(0, 1.25f);
        line.setPosition(p1);
        line.setFillColor(UITheme::Color::AVLAccent); 
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

void AVLScreen::drawDescription(sf::RenderWindow& window, const sf::Font& font, float shiftX) {
    // Determine description text
    std::string descText;
    if (m_selectedNodeValue != -1) {
        descText = "Node " + std::to_string(m_selectedNodeValue) + " selected";
    } else if (mController.hasSteps()) {
        descText = mController.currentStep()->description;
    } else {
        return;
    }

    float boxH = 70.f;
    float go_up = 70.f;

    float boxW = 700.f; 
    float boxX = 640.f + shiftX - boxW/2.f; 
    float boxY = CANVAS_H + 64.f - go_up;

    sf::RectangleShape shadow({boxW, boxH});
    shadow.setPosition(boxX + 4.f, boxY + 4.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 90));
    window.draw(shadow);

    sf::RectangleShape bar({boxW, boxH});
    bar.setPosition(boxX, boxY);
    bar.setFillColor(UITheme::Color::AVLPanelBg);
    bar.setOutlineThickness(1.5f);
    bar.setOutlineColor(m_selectedNodeValue != -1 ? UITheme::Color::AVLAccent : UITheme::Color::AVLGlowStrong);
    window.draw(bar);

    sf::RectangleShape accent({4.f, boxH});
    accent.setPosition(boxX, boxY);
    accent.setFillColor(UITheme::Color::AVLAccent);
    window.draw(accent);

    sf::Text desc;
    desc.setFont(font);
    desc.setString(descText);
    desc.setCharacterSize(24);
    desc.setLetterSpacing(1.1f);
    desc.setFillColor(sf::Color(240, 245, 255));
    desc.setPosition(boxX + 18.f, boxY + 20.f);
    window.draw(desc);
}