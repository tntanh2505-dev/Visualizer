#include "DSA-Visualization/ui/AVL_Screen.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"
#include "DSA-Visualization/ui/components/graphic_node.hpp"
#include <iostream>
#include <cmath>
#include <map>
#include <functional>
#include <fstream> 
#include <sstream>

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

AVLScreen::AVLScreen()
: mInputActive(false)
, mCodePanel()
, mSpeedValue(2.f)
, mSliderDragging(false)
, mHistoryIndex(0)
, m_leftWidth(35.f)
, m_rightWidth(35.f)
, m_leftExpanded(true)
, m_rightExpanded(true)
, m_selectedNodeValue(-1)
, m_isLightMode(false)
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
    mStepMode = false;
    mStepAnimatingNext = false;
    mStepAnimatingPrev = false;
}

int AVLScreen::run(sf::RenderWindow& window, sf::Font& font) {
    mBaseWidth = window.getSize().x;
    mBaseHeight = window.getSize().y;

    sf::View originalView = window.getView(); 
    sf::View dynamicView(sf::FloatRect(0, 0, mBaseWidth, mBaseHeight));
    window.setView(dynamicView);

    mWorkspaceBg.setSize({mBaseWidth, mBaseHeight});

    if (!mBgTexture.loadFromFile("assets/textures/avl_background.png"))
        std::cerr << "Failed to load background.png\n";
    mBgSprite.setTexture(mBgTexture);
    mBgSprite.setScale(mBaseWidth / mBgTexture.getSize().x, mBaseHeight / mBgTexture.getSize().y);
    mBgSprite.setColor(UITheme::Color::AVLBackground);

    mCodePanel = CodePanel(font, sf::Vector2f(0.f, 0.f), sf::Vector2f(380.f, 400.f));
    
    std::map<std::string, std::vector<std::string>> avlSnippets;
    avlSnippets["insert"] = INSERT_CODE;
    avlSnippets["remove"] = DELETE_CODE;
    avlSnippets["search"] = SEARCH_CODE;
    mCodePanel.loadSnippets(avlSnippets);
    mCodePanel.update("insert", -1);
    
    // Right Panel Tabs Setup
    mRightTabState = AVLRightTabState::CODE;
    mInfoTextDisplay.setFont(font);
    mInfoTextDisplay.setCharacterSize(15);
    mInfoTextDisplay.setString("AVL Tree\n\n- O(log N) Insert\n- O(log N) Delete\n- O(log N) Search\n\nA self-balancing binary search tree\nwhere heights of child subtrees\ndiffer by at most one.");

    mSliderTrack = sf::RectangleShape({220.f, 6.f});
    mSliderTrack.setFillColor(UITheme::Color::SliderTrack);
    mSliderTrack.setOutlineThickness(1.f);
    mSliderTrack.setOutlineColor(UITheme::Color::AVLGlow);

    mSliderHandle = sf::CircleShape(10.f);
    mSliderHandle.setOrigin(10.f, 10.f);
    mSliderHandle.setFillColor(UITheme::Color::SliderHandle); 

    mInputBox.setSize({220.f, 40.f});
    mInputBox.setOutlineThickness(1.5f);

    mCurrentNodeColor = sf::Color(245, 249, 255); 
    mThemeColors = {
        sf::Color(45, 45, 55), sf::Color(181, 58, 199),
        sf::Color(52, 152, 219), sf::Color(231, 76, 60),
        sf::Color(241, 196, 15), sf::Color(46, 204, 113)
    };

    mColorSwatches.clear();
    for (const auto& color : mThemeColors) {
        sf::RectangleShape swatch(sf::Vector2f(25.f, 25.f));
        swatch.setFillColor(color);
        swatch.setOutlineThickness(1.5f);
        swatch.setOutlineColor(sf::Color(100, 100, 100));
        mColorSwatches.push_back(swatch);
    }

    applyTheme(font);

    sf::VertexArray dotGrid(sf::Points);
    for (int x = 0; x <= 3000; x += 30) {
        for (int y = 0; y <= 3000; y += 30) {
            dotGrid.append(sf::Vertex(sf::Vector2f(x, y), UITheme::Color::AVLGlow));
        }
    }

    const float LEFT_PANEL_WIDTH = 300.f;
    const float RIGHT_PANEL_WIDTH = 450.f;
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

            if (event.type == sf::Event::Resized) {
                mBaseWidth = event.size.width;
                mBaseHeight = event.size.height;
                sf::View resizedView(sf::FloatRect(0, 0, mBaseWidth, mBaseHeight));
                window.setView(resizedView);
                
                mWorkspaceBg.setSize({mBaseWidth, mBaseHeight});
                mBgSprite.setScale(mBaseWidth / mBgTexture.getSize().x, mBaseHeight / mBgTexture.getSize().y);
            }

            bool leftPressed = (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);
            
            if (leftPressed) {
                float centerY = mBaseHeight / 2.f;
                float winW = mBaseWidth;
                
                bool mouseInLeftTab = (mouseRaw.x >= m_leftWidth - TAB_WIDTH && mouseRaw.x <= m_leftWidth &&
                                       mouseRaw.y >= centerY - TAB_HEIGHT / 2.f && mouseRaw.y <= centerY + TAB_HEIGHT / 2.f);
                if (mouseInLeftTab) { m_leftExpanded = !m_leftExpanded; continue; }

                bool mouseInRightTab = (mouseRaw.x >= winW - m_rightWidth && mouseRaw.x <= winW - m_rightWidth + TAB_WIDTH &&
                                        mouseRaw.y >= centerY - TAB_HEIGHT / 2.f && mouseRaw.y <= centerY + TAB_HEIGHT / 2.f);
                if (mouseInRightTab) { m_rightExpanded = !m_rightExpanded; continue; }
            }

            bool isClickingOnPanel = (mouseRaw.x < m_leftWidth) || (mouseRaw.x > mBaseWidth - m_rightWidth);

            if (leftPressed && isClickingOnPanel) {
                if (mReturnBtn->isClicked(mouseRaw, true)) {
                    window.setView(originalView);
                    return 0;
                }

                // --- Right Panel Click Detection ---
                if (mouseRaw.x > mBaseWidth - m_rightWidth) { 
                    if (mInfoTabBtn->isClicked(mouseRaw, true)) mRightTabState = AVLRightTabState::INFO;
                    if (mCodeTabBtn->isClicked(mouseRaw, true)) mRightTabState = AVLRightTabState::CODE;
                }

                // --- Left Panel Click Detection ---
                if (mouseRaw.x < m_leftWidth) {
                    if (mDarkThemeBtn->isClicked(mouseRaw, true)) { m_isLightMode = false; applyTheme(font); }
                    if (mLightThemeBtn->isClicked(mouseRaw, true)) { m_isLightMode = true; applyTheme(font); }

                    for (size_t i = 0; i < mColorSwatches.size(); ++i) {
                        if (mColorSwatches[i].getGlobalBounds().contains(mouseRaw)) {
                            mCurrentNodeColor = mThemeColors[i];
                        }
                    }
                }

                mInputActive = mInputBox.getGlobalBounds().contains(mouseRaw);

                if (mSliderHandle.getGlobalBounds().contains(mouseRaw) || mSliderTrack.getGlobalBounds().contains(mouseRaw))
                    mSliderDragging = true;

                if (mInsertBtn->isClicked(mouseRaw, true) && !mInputString.empty()) {
                    if (mHistoryIndex < (int)mHistory.size()) mHistory.erase(mHistory.begin() + mHistoryIndex, mHistory.end());
                    std::string processedString = mInputString;
                    std::replace(processedString.begin(), processedString.end(), ',', ' ');
                    std::stringstream ss(processedString);
                    std::string token;
                    while (ss >> token) {
                        try {
                            int val = std::stoi(token);
                            Operation op{OpType::Insert, val};
                            mHistory.push_back(op); 
                            mHistoryIndex++;
                            buildSteps(op);
                        } catch (...) { continue; }
                    }
                    mInputString.clear();
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
                    buildSteps(op); mController.skipToEnd();
                    mInputString.clear(); m_selectedNodeValue = -1;
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
                    for (int i = 0; i < count; ++i) { mHistory.push_back({OpType::Insert, rand() % 100}); mHistoryIndex++; }
                    
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
                    if (mController.currentIndex() > 0) {
                        mController.prev();
                        mStepMode = true; mStepAnimatingPrev = true; mStepAnimatingNext = false;
                    } else if (mHistoryIndex > 0) {
                        mHistoryIndex--; mTree.clear(); mController.clear();
                        for(int i = 0; i < mHistoryIndex - 1; ++i) {
                            if (mHistory[i].type == OpType::Insert)       mTree.insert(mHistory[i].value, nullptr);
                            else if (mHistory[i].type == OpType::Delete)  mTree.remove(mHistory[i].value, nullptr);
                            else if (mHistory[i].type == OpType::Clear)   mTree.clear();
                            else if (mHistory[i].type == OpType::Update) { mTree.remove(mHistory[i].oldValue, nullptr); mTree.insert(mHistory[i].value, nullptr); }
                        }
                        if (mHistoryIndex > 0) { buildSteps(mHistory[mHistoryIndex - 1]); mController.skipToEnd(); }
                        m_selectedNodeValue = -1; mStepMode = true; mStepAnimatingPrev = false; mStepAnimatingNext = false;
                    }
                }
                
                if (mNextBtn->isClicked(mouseRaw, true)) {
                    if (mController.currentIndex() < mController.totalSteps() - 1) {
                        mController.next();
                        mStepMode = true; mStepAnimatingNext = true; mStepAnimatingPrev = false;
                    } else if (mHistoryIndex < (int)mHistory.size()) {
                        Operation op = mHistory[mHistoryIndex]; mHistoryIndex++;
                        buildSteps(op);
                        m_selectedNodeValue = -1; mStepMode = true; mStepAnimatingNext = true; mStepAnimatingPrev = false;
                    }
                }

                if (mSkipAnimationBtn->isClicked(mouseRaw, true)){
                    if (mHistoryIndex > 0) { buildSteps(mHistory[mHistoryIndex - 1]); mController.skipToEnd(); }
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
                        if (mHistoryIndex > 0) { buildSteps(mHistory[mHistoryIndex - 1]); mController.skipToEnd(); }
                        mInputString.clear(); m_selectedNodeValue = -1;
                    }
                }
            }

            // --- Node click detection on the canvas ---
            if (leftPressed && !isClickingOnPanel && mController.hasSteps()) {
                float canvasCenter = m_leftWidth + (mBaseWidth - m_leftWidth - m_rightWidth) / 2.f;
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
                else 
                if (((event.text.unicode >= '0' && event.text.unicode <= '9' || event.text.unicode == '-') || 
               (event.text.unicode == ' ' || event.text.unicode == ',')) && mInputString.size() < 10)
                mInputString += static_cast<char>(event.text.unicode);
            }
        }
        
        // --- Smooth Layout Mathematics ---
        float targetLeft = m_leftExpanded ? LEFT_PANEL_WIDTH : TAB_WIDTH;
        m_leftWidth += (targetLeft - m_leftWidth) * 12.f * dt;
        
        float targetRight = m_rightExpanded ? RIGHT_PANEL_WIDTH : TAB_WIDTH;
        m_rightWidth += (targetRight - m_rightWidth) * 12.f * dt;

        float leftBaseX = m_leftWidth - LEFT_PANEL_WIDTH;
        float rightBaseX = mBaseWidth - m_rightWidth;

        // Sync UI positions dynamically to parallel Linked List Logic
        float currentY = 20.f;
        mReturnBtn->setPosition(sf::Vector2f(leftBaseX + 30.f + 40.f, currentY + 20.f)); 
        currentY += 50.f;

        mInputBox.setPosition(sf::Vector2f(leftBaseX + 30.f, currentY)); 
        mInputBox.setFillColor(UITheme::Color::CodePanelBg);
        mInputBox.setOutlineColor(mInputActive ? UITheme::Color::AVLAccent : UITheme::Color::ModernBtnBorder);
        currentY += 50.f;

        mInsertBtn->setPosition(sf::Vector2f(leftBaseX + 30.f + 52.5f, currentY + 20.f));
        mDeleteBtn->setPosition(sf::Vector2f(leftBaseX + 145.f + 52.5f, currentY + 20.f)); 
        currentY += 50.f;

        mSearchBtn->setPosition(sf::Vector2f(leftBaseX + 30.f + 52.5f, currentY + 20.f));
        mUpdateBtn->setPosition(sf::Vector2f(leftBaseX + 145.f + 52.5f, currentY + 20.f)); 
        currentY += 50.f;

        mRandomBtn->setPosition(sf::Vector2f(leftBaseX + 30.f + 52.5f, currentY + 20.f));
        mClearBtn->setPosition(sf::Vector2f(leftBaseX + 145.f + 52.5f, currentY + 20.f)); 
        currentY += 50.f;

        mLoadFileBtn->setPosition(sf::Vector2f(leftBaseX + 30.f + 110.f, currentY + 20.f)); 
        currentY += 50.f;

        mPrevBtn->setPosition(sf::Vector2f(leftBaseX + 30.f + 52.5f, currentY + 20.f));
        mNextBtn->setPosition(sf::Vector2f(leftBaseX + 145.f + 52.5f, currentY + 20.f)); 
        currentY += 50.f;

        mSkipAnimationBtn->setPosition(sf::Vector2f(leftBaseX + 30.f + 110.f, currentY + 20.f)); 
        currentY += 70.f;

        mSliderTrack.setPosition(leftBaseX + 30.f, currentY);
        if (mSliderDragging) {
            float trackLeft  = leftBaseX + 30.f;
            float trackRight = leftBaseX + 250.f;
            float ratio = (mouseRaw.x - trackLeft) / (trackRight - trackLeft);
            ratio = std::max(0.f, std::min(1.f, ratio));
            mSpeedValue = 0.5f + ratio * 7.5f; 
            mController.setSpeed(mSpeedValue);
        }
        float ratio = (mSpeedValue - 0.5f) / 7.5f;
        mSliderHandle.setPosition(leftBaseX + 30.f + ratio * 220.f, currentY + 3.f);
        
        currentY += 40.f;
        
        // --- Relocated Customization Properties to Left Panel Bottom ---
        float startX_custom = leftBaseX + 30.f;
        mColorLabelPos = {startX_custom, currentY};
        for (size_t i = 0; i < mColorSwatches.size(); ++i) {
            float x = startX_custom + (i % 3) * 35.f;
            float y = currentY + 25.f + (i / 3) * 35.f;
            mColorSwatches[i].setPosition({x, y});
        }
        
        currentY += 105.f;
        mThemeLabelPos = {startX_custom, currentY};
        mDarkThemeBtn->setPosition({startX_custom + 40.f, currentY + 45.f});
        mLightThemeBtn->setPosition({startX_custom + 130.f, currentY + 45.f});

        // --- Right Panel Layout Mathematics (Tabs & Code) ---
        mInfoTabBtn->setPosition({rightBaseX + TAB_WIDTH + 15.f + 40.f, 40.f});
        mCodeTabBtn->setPosition({rightBaseX + TAB_WIDTH + 15.f + 130.f, 40.f});

        if (mRightTabState == AVLRightTabState::CODE) {
            mCodePanel.setPosition(sf::Vector2f(rightBaseX + TAB_WIDTH + 15.f, 80.f));
        } else {
            mInfoTextDisplay.setPosition({rightBaseX + TAB_WIDTH + 15.f, 80.f});
        }

        // Update hovers
        mInsertBtn->update(mouseRaw); mDeleteBtn->update(mouseRaw); mSearchBtn->update(mouseRaw);
        mUpdateBtn->update(mouseRaw); mRandomBtn->update(mouseRaw); mClearBtn->update(mouseRaw); 
        mLoadFileBtn->update(mouseRaw); mPrevBtn->update(mouseRaw); mNextBtn->update(mouseRaw); 
        mSkipAnimationBtn->update(mouseRaw); mReturnBtn->update(mouseRaw);
        mDarkThemeBtn->update(mouseRaw); mLightThemeBtn->update(mouseRaw);
        mInfoTabBtn->update(mouseRaw); mCodeTabBtn->update(mouseRaw);

        if (!mStepMode) {
            mController.update(dt);
        } else {
            if (mStepAnimatingNext) {
                mController.update(dt);
                if (mController.t() >= 1.0f) mStepAnimatingNext = false;
            } else if (mStepAnimatingPrev) {
                mController.update(dt);
                if (mController.t() <= 0.0f) mStepAnimatingPrev = false;
            }
        }

        window.clear(UITheme::Color::AVLBackground);  
        window.draw(mWorkspaceBg);
        window.draw(mBgSprite); 
        window.draw(dotGrid); 

        // Dynamic Tree Centering
        float newCenter = m_leftWidth + (mBaseWidth - m_leftWidth - m_rightWidth) / 2.f;
        float shiftX = newCenter - 610.f; // Base 610 represents center of old layout

        drawTree(window, font, shiftX);
        drawDescription(window, font, shiftX);

        drawLeftPanel(window, font, leftBaseX);
        drawRightPanel(window, font, rightBaseX);
        
        window.display();
    }
    
    window.setView(originalView);
    return -1;
}

void AVLScreen::drawLeftPanel(sf::RenderWindow& window, const sf::Font& font, float leftBaseX) {
    sf::RectangleShape leftMenu(sf::Vector2f(m_leftWidth, mBaseHeight));
    leftMenu.setFillColor(UITheme::Color::AVLPanelBg);
    window.draw(leftMenu);

    float centerY = mBaseHeight / 2.f;
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
        window.draw(*mReturnBtn);
        window.draw(mInputBox);

        if (mInputActive) {
            sf::RectangleShape glowBox({220.f, 40.f});
            glowBox.setPosition(mInputBox.getPosition());
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
        inputText.setFillColor(mInputString.empty() ? sf::Color(100, 110, 130) : (m_isLightMode ? sf::Color::Black : sf::Color::White));
        inputText.setPosition(mInputBox.getPosition().x + 10.f, mInputBox.getPosition().y + 10.f);
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

        sf::Text speedLabel;
        speedLabel.setFont(font);
        speedLabel.setCharacterSize(13);
        speedLabel.setFillColor(UITheme::Color::AVLSpeedSliderText);
        speedLabel.setString("Speed: " + std::to_string((int)mSpeedValue) + "x");
        speedLabel.setPosition(mSliderTrack.getPosition().x, mSliderTrack.getPosition().y - 20.f);
        window.draw(speedLabel);

        sf::RectangleShape filledTrack({mSliderHandle.getPosition().x - mSliderTrack.getPosition().x, 6.f});
        filledTrack.setPosition(mSliderTrack.getPosition());
        filledTrack.setFillColor(UITheme::Color::AVLSliderFill);

        window.draw(mSliderTrack);
        window.draw(filledTrack);
        window.draw(mSliderHandle);
        
        // --- Draw Color Customization UI on Left Panel ---
        sf::Text colorLabel("Node Fill Color", font, 14);
        colorLabel.setFillColor(m_isLightMode ? sf::Color(40, 40, 50) : sf::Color(200, 200, 210));
        colorLabel.setPosition(mColorLabelPos);
        window.draw(colorLabel);

        for (size_t i = 0; i < mColorSwatches.size(); ++i) {
            if (mThemeColors[i] == mCurrentNodeColor) {
                mColorSwatches[i].setOutlineColor(sf::Color::Yellow);
                mColorSwatches[i].setOutlineThickness(2.5f);
            } else {
                mColorSwatches[i].setOutlineColor(sf::Color(100, 100, 100));
                mColorSwatches[i].setOutlineThickness(1.5f);
            }
            window.draw(mColorSwatches[i]);
        }
        
        sf::Text tLabel("UI Theme", font, 14);
        tLabel.setFillColor(m_isLightMode ? sf::Color(40, 40, 50) : sf::Color(200, 200, 210));
        tLabel.setPosition(mThemeLabelPos);
        window.draw(tLabel);

        window.draw(*mDarkThemeBtn);
        window.draw(*mLightThemeBtn);
    }
}

void AVLScreen::drawRightPanel(sf::RenderWindow& window, const sf::Font& font, float rightBaseX) {
    float winW = mBaseWidth;
    float TAB_WIDTH = 35.f;
    
    sf::RectangleShape rightMenu(sf::Vector2f(m_rightWidth, mBaseHeight));
    rightMenu.setFillColor(UITheme::Color::AVLPanelBg);
    rightMenu.setPosition(winW - m_rightWidth, 0);
    window.draw(rightMenu);

    float centerY = mBaseHeight / 2.f;
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

    // --- Draw Interactive Tabs ---
    if (m_rightWidth > 180.f) {
        window.draw(*mInfoTabBtn);
        window.draw(*mCodeTabBtn);

        sf::RectangleShape underline({80.f, 2.f});
        underline.setFillColor(sf::Color::Cyan);
        underline.setOrigin(40.f, 1.f);
        if (mRightTabState == AVLRightTabState::INFO) {
            underline.setPosition({mInfoTabBtn->getPosition().x, mInfoTabBtn->getPosition().y + 18.f});
            window.draw(underline);
            window.draw(mInfoTextDisplay);
        } else {
            underline.setPosition({mCodeTabBtn->getPosition().x, mCodeTabBtn->getPosition().y + 18.f});
            window.draw(underline);
            mCodePanel.highlight(mController.hasSteps() ? mController.currentStep()->codeLineIndex : -1);
            const_cast<CodePanel&>(mCodePanel).draw(window);
        }
    }
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
    
    // Override the animation frame's fill color with user-selected color
    node.setFillColor(mCurrentNodeColor);
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
    float boxW = 700.f; 
    float boxX = 640.f + shiftX - boxW/2.f; 
    float boxY = mBaseHeight - 140.f; 

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
    desc.setFillColor(m_isLightMode ? sf::Color(30, 30, 35) : sf::Color(240, 245, 255));
    desc.setPosition(boxX + 18.f, boxY + 20.f);
    window.draw(desc);
}

void AVLScreen::applyTheme(sf::Font& font) {
    if (m_isLightMode) {
        // Pleasant, soft light mode palette
        UITheme::Color::AVLBackground        = sf::Color(245, 246, 250); 
        UITheme::Color::AVLPanelBg           = sf::Color(230, 232, 240, 240);
        UITheme::Color::AVLSpeedSliderText   = sf::Color(40, 40, 50);
        
        // Soft white buttons with subtle contrast
        UITheme::Color::ModernBtnTop         = sf::Color(250, 250, 255);
        UITheme::Color::ModernBtnBottom      = sf::Color(235, 235, 240);
        UITheme::Color::ModernBtnHoverT      = sf::Color(240, 240, 245);
        UITheme::Color::ModernBtnHoverB      = sf::Color(220, 220, 230);
        UITheme::Color::ModernBtnBorder      = sf::Color(180, 180, 195);
        
        mBgSprite.setColor(sf::Color(255, 255, 255, 180)); // Lighten background grid
    } else {
        // Restore Amethyst Dark Mode palette
        UITheme::Color::AVLBackground        = UITheme::Color::GlobalBg;
        UITheme::Color::AVLPanelBg           = UITheme::Color::GlobalPanelBg;
        UITheme::Color::AVLSpeedSliderText   = UITheme::Color::GlobalTextPrimary;
        
        UITheme::Color::ModernBtnTop         = UITheme::Color::GlobalButtonFill;
        UITheme::Color::ModernBtnBottom      = sf::Color(20, 16, 27);
        UITheme::Color::ModernBtnHoverT      = UITheme::Color::GlobalButtonHover;
        UITheme::Color::ModernBtnHoverB      = sf::Color(37, 24, 56);
        UITheme::Color::ModernBtnBorder      = UITheme::Color::GlobalButtonBorder;

        mBgSprite.setColor(UITheme::Color::AVLBackground);
    }

    mInsertBtn.emplace("Insert",  font, sf::Vector2f(105.f, 40.f));
    mDeleteBtn.emplace("Delete",  font, sf::Vector2f(105.f, 40.f));
    mSearchBtn.emplace("Search",  font, sf::Vector2f(105.f, 40.f));
    mUpdateBtn.emplace("Update",  font, sf::Vector2f(105.f, 40.f));
    mRandomBtn.emplace("Random",  font, sf::Vector2f(105.f, 40.f));
    mClearBtn .emplace("Clear",   font, sf::Vector2f(105.f, 40.f));
    mLoadFileBtn.emplace("Load File", font, sf::Vector2f(220.f, 40.f));
    mPrevBtn  .emplace("< Prev",  font, sf::Vector2f(105.f,  40.f));
    mNextBtn  .emplace("Next >",  font, sf::Vector2f(105.f,  40.f));
    mSkipAnimationBtn.emplace("Skip Animation", font, sf::Vector2f(220.f, 40.f));
    mReturnBtn.emplace("Return",  font, sf::Vector2f(80.f, 40.f));
    
    // Tab and Customization buttons
    mDarkThemeBtn.emplace("Dark", font, sf::Vector2f(80.f, 30.f));
    mLightThemeBtn.emplace("Light", font, sf::Vector2f(80.f, 30.f));
    mInfoTabBtn.emplace("INFO", font, sf::Vector2f(80.f, 30.f));
    mCodeTabBtn.emplace("CODE", font, sf::Vector2f(80.f, 30.f));

    if(m_isLightMode) {
        mDarkThemeBtn->setColors(UITheme::Color::GlobalButtonFill, UITheme::Color::GlobalButtonBot, UITheme::Color::ButtonInactiveBorder, UITheme::Color::GlobalTextPrimary);
        mLightThemeBtn->setColors(UITheme::Color::ModernBtnTop, UITheme::Color::ModernBtnBottom, sf::Color::Cyan, sf::Color(40,40,50));
        
        mInfoTabBtn->setColors(UITheme::Color::ModernBtnTop, UITheme::Color::ModernBtnBottom, UITheme::Color::ModernBtnBorder, sf::Color(40,40,50));
        mCodeTabBtn->setColors(UITheme::Color::ModernBtnTop, UITheme::Color::ModernBtnBottom, UITheme::Color::ModernBtnBorder, sf::Color(40,40,50));
        mInfoTextDisplay.setFillColor(sf::Color::Black);
    } else {
        mDarkThemeBtn->setColors(UITheme::Color::ModernBtnTop, UITheme::Color::ModernBtnBottom, sf::Color::Cyan, sf::Color::White);
        mLightThemeBtn->setColors(UITheme::Color::LightButtonFill, UITheme::Color::LightButtonBot, UITheme::Color::ButtonInactiveBorder, UITheme::Color::LightTextPrimary);

        mInfoTabBtn->setColors(UITheme::Color::ModernBtnTop, UITheme::Color::ModernBtnBottom, UITheme::Color::ModernBtnBorder, sf::Color::White);
        mCodeTabBtn->setColors(UITheme::Color::ModernBtnTop, UITheme::Color::ModernBtnBottom, UITheme::Color::ModernBtnBorder, sf::Color::White);
        mInfoTextDisplay.setFillColor(sf::Color(245, 245, 250));
    }
}