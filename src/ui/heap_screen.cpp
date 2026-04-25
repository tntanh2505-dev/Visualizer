#include "DSA-Visualization/ui/heap_screen.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace {
    constexpr std::size_t MAX_RENDERED_NODES = 32;
    constexpr float TREE_TOP_Y = 150.f;
    constexpr float ARRAY_Y = 40.f;

    sf::Text makeText(const sf::Font& font,
                    const std::string& value,
                    unsigned int size,
                    sf::Color color,
                    sf::Vector2f position) {
        sf::Text text;
        text.setFont(font);
        text.setString(value);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setPosition(position);
        return text;
    }
}

HeapVisualizer::HeapVisualizer(const sf::Font& font)
    : mFont(font)
    // Texts
    , mSpeedLabel(makeText(font, "Speed: 0.60s", 14, sf::Color::White, {0.f, 0.f}))
    , mPlaceholderText(makeText(font, "Enter value...", 18, sf::Color(120, 112, 138, 190), {0.f, 0.f}))
    , mInputText(makeText(font, "", 22, sf::Color::White, {0.f, 0.f}))
    , mHintText(makeText(font, "Build format: 1, 2, 3...", 14, sf::Color(150, 150, 150), {0.f, 0.f}))
    , mStatusText(makeText(font, "", 16, sf::Color(251, 209, 101), {0.f, 0.f}))
    , mNodeRadius(24.f)
    // Sized natively to parallel Linked List
    , mInsertButton("INS", font, {105.f, 40.f})
    , mDeleteButton("DEL", font, {105.f, 40.f})
    , mBuildButton("BUILD", font, {105.f, 40.f})
    , mClearButton("CLEAR", font, {105.f, 40.f})
    , mUpdateButton("UPDATE", font, {105.f, 40.f})
    , mRandomButton("RANDOM", font, {105.f, 40.f})
    , mLoadButton("LOAD FILE", font, {220.f, 40.f})
    , mSkipButton("SKIP", font, {220.f, 40.f})
    , mPrevButton("PREV", font, {70.f, 40.f})
    , mPlayPauseButton("PAUSE", font, {70.f, 40.f})
    , mStepButton("NEXT", font, {70.f, 40.f})
    , mReturnButton("MENU", font, {80.f, 40.f})
    // Customization & Theme Buttons
    , mDarkThemeBtn("Dark", font, {80.f, 30.f})
    , mLightThemeBtn("Light", font, {80.f, 30.f})
    , mBgDefaultBtn("PNG", font, {70.f, 30.f})
    , mBgWhiteBtn("White", font, {70.f, 30.f})
    , mBgBlackBtn("Black", font, {70.f, 30.f})
    // Tab Buttons
    , mInfoTabBtn("INFO", font, {80.f, 30.f})
    , mCodeTabBtn("CODE", font, {80.f, 30.f})
{
    // Right Panel Tabs Setup
    mRightTabState = HeapRightTabState::INFO;
    
    // Parse the provided HEAP.txt structured information
    std::vector<std::string> infoLines = {
        "HEAP",
        "[WHAT IS IT]",
        "It's complete binary tree in a container",
        "which usually an array.",
        "We demonstrate both its array form",
        "and binary tree form.",
        "",
        "[FUNCTION TIME COMPLEXITY]",
        "Insert/Delete       O(logN)",
        "Search              O(logN)",
        "CLEAR ALL           O(1)",
        "",
        "[CONTROL BUTTON]",
        "Play/Pause: Auto-play animation",
        "Prev/Next: Watch step by step",
        "Skip: Watch run at once"
    };

    for (const auto& line : infoLines) {
        sf::Text t(line, font, 14);
        if (line == "HEAP") {
            t.setCharacterSize(20);
            t.setStyle(sf::Text::Bold);
        } else if (!line.empty() && line.front() == '[' && line.back() == ']') {
            t.setCharacterSize(16);
            t.setStyle(sf::Text::Bold);
        }
        mInfoTexts.push_back(t);
    }

    mWorkspaceBg.setSize({mBaseWidth, mBaseHeight});
    
    // Code Panel Dimension update to fit right panel dynamically
    mCodePanel = CodePanel(font, {0.f, 0.f}, {380.f, 400.f});
    loadHeapifyCode();

    mSliderTrack.setSize({220.f, 6.f});
    mSliderTrack.setFillColor(sf::Color(60, 60, 80));
    mSliderTrack.setOutlineThickness(1.f);
    mSliderTrack.setOutlineColor(sf::Color(100, 100, 150));
    mSliderKnob.setRadius(10.f);
    mSliderKnob.setFillColor(sf::Color(181, 58, 199));
    mSliderKnob.setOrigin(10.f, 10.f);
    float initialT = 1.0f - (mActionInterval - 0.1f) / (2.0f - 0.1f);
    mSliderKnob.setPosition({0.f + initialT * 220.f, 0.f});

    // Input area
    mInputBox.setSize({220.f, 40.f});
    mInputBox.setFillColor(sf::Color(32, 26, 43));
    mInputBox.setOutlineThickness(2.f);
    mInputBox.setOutlineColor(sf::Color(181, 58, 199, 120));
    
    // Customization Init
    mCurrentNodeColor = sf::Color(245, 249, 255); 
    mThemeColors = {
        sf::Color(245, 249, 255), sf::Color(181, 58, 199),
        sf::Color(52, 152, 219), sf::Color(231, 76, 60),
        sf::Color(241, 196, 15), sf::Color(46, 204, 113)
    };

    for (const auto& color : mThemeColors) {
        sf::RectangleShape swatch(sf::Vector2f(25.f, 25.f));
        swatch.setFillColor(color);
        swatch.setOutlineThickness(1.5f);
        swatch.setOutlineColor(sf::Color(100, 100, 100));
        mColorSwatches.push_back(swatch);
    }

    mSizeLabel = makeText(font, "Node Size: 24", 14, sf::Color::White, {0.f, 0.f});
    mSizeSliderTrack.setSize({220.f, 6.f});
    mSizeSliderTrack.setFillColor(sf::Color(60, 60, 80));
    mSizeSliderKnob.setRadius(8.f);
    mSizeSliderKnob.setOrigin(8.f, 8.f);
    mSizeSliderKnob.setFillColor(sf::Color(181, 58, 199));

    setStatus("Ready.");
    applyTheme();
}

void HeapVisualizer::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        // --- Native Tab Slide Interaction (Synced with SLL) ---
        float centerY = mBaseHeight / 2.f;
        float winW = mBaseWidth;
        
        bool mouseInLeftTab = (mouse.x >= mLeftWidth - TAB_WIDTH && mouse.x <= mLeftWidth &&
                               mouse.y >= centerY - 25.f && mouse.y <= centerY + 25.f);
        if (mouseInLeftTab) { mLeftExpanded = !mLeftExpanded; return; }

        bool mouseInRightTab = (mouse.x >= winW - mRightWidth && mouse.x <= winW - mRightWidth + TAB_WIDTH &&
                                mouse.y >= centerY - 25.f && mouse.y <= centerY + 25.f);
        if (mouseInRightTab) { mRightExpanded = !mRightExpanded; return; }

        bool isClickingOnPanel = (mouse.x < mLeftWidth) || (mouse.x > mBaseWidth - mRightWidth);

        if (!isClickingOnPanel && mPendingActions.empty()) {
            const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
            for (std::size_t i = 0; i < visibleNodes; ++i) {
                float currentR = (i >= 15) ? mNodeRadius * 0.8f : mNodeRadius;
                sf::Vector2f pos = nodePosition(i);
                float dx = mouse.x - pos.x;
                float dy = mouse.y - pos.y;
                if (dx * dx + dy * dy <= currentR * currentR) {
                    mSelectedIndex = static_cast<int>(i);
                    mInputBuffer.clear();
                    mInputText.setString("");
                    setStatus("Node " + std::to_string(i) + " (value " + std::to_string(mDisplayArray[i]) + ") selected.");
                    return;
                }
            }
        }

        sf::FloatRect knobBounds = mSliderKnob.getGlobalBounds();
        if (knobBounds.contains(mouse) || mSliderTrack.getGlobalBounds().contains(mouse)) mIsDraggingSlider = true;
        
        if (mLeftExpanded && mouse.x < mLeftWidth) {
            for (size_t i = 0; i < mColorSwatches.size(); ++i) {
                if (mColorSwatches[i].getGlobalBounds().contains(mouse)) {
                    mCurrentNodeColor = mThemeColors[i];
                    setStatus("Node color updated.");
                    return; 
                }
            }
        }

        sf::FloatRect sizeKnobBounds = mSizeSliderKnob.getGlobalBounds();
        if (sizeKnobBounds.contains(mouse) || mSizeSliderTrack.getGlobalBounds().contains(mouse)) mIsDraggingSizeSlider = true;
    }

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        mIsDraggingSlider = false;
        mIsDraggingSizeSlider = false;

        bool isClickingOnPanel = (mouse.x < mLeftWidth) || (mouse.x > mBaseWidth - mRightWidth);

        if (!isClickingOnPanel) {
            bool mouseOverNode = false;
            const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
            for (std::size_t i = 0; i < visibleNodes; ++i) {
                float currentR = (i >= 15) ? mNodeRadius * 0.8f : mNodeRadius;
                sf::Vector2f pos = nodePosition(i);
                float dx = mouse.x - pos.x;
                float dy = mouse.y - pos.y;
                if (dx * dx + dy * dy <= currentR * currentR) { mouseOverNode = true; break; }
            }
            if (!mouseOverNode) mSelectedIndex = -1;
            mInputFocused = false;
        } else if (mInputBox.getGlobalBounds().contains(mouse)) {
            mInputFocused = true;
        }

        if (mInfoTabBtn.isClicked(mouse, true)) mRightTabState = HeapRightTabState::INFO;
        if (mCodeTabBtn.isClicked(mouse, true)) mRightTabState = HeapRightTabState::CODE;

        if (mLoadButton.isClicked(mouse, true)) runLoadFile();
        if (mInsertButton.isClicked(mouse, true)) runInsert();
        if (mDeleteButton.isClicked(mouse, true)) runDeleteSelected();
        if (mBuildButton.isClicked(mouse, true)) runBuildHeap();
        if (mClearButton.isClicked(mouse, true)) runClear();
        if (mUpdateButton.isClicked(mouse, true)) runUpdate();
        if (mRandomButton.isClicked(mouse, true)) runRandom();
        if (mSkipButton.isClicked(mouse, true)) runSkip();
        if (mPlayPauseButton.isClicked(mouse, true)) togglePlayback();
        
        if (mDarkThemeBtn.isClicked(mouse, true)) { mIsDarkMode = true; applyTheme(); }
        if (mLightThemeBtn.isClicked(mouse, true)) { mIsDarkMode = false; applyTheme(); }
        
        if (mBgDefaultBtn.isClicked(mouse, true)) { mBgType = BackgroundType::Default; setStatus("Background set to PNG."); }
        if (mBgWhiteBtn.isClicked(mouse, true)) { mBgType = BackgroundType::White; setStatus("Background set to White."); }
        if (mBgBlackBtn.isClicked(mouse, true)) { mBgType = BackgroundType::Black; setStatus("Background set to Black."); }
        
        if (mStepButton.isClicked(mouse, true)) {
            mIsPlaying = false;
            if (!mPendingActions.empty()) processNextAction();
            else setStatus("No pending animation steps.");
        }
        if (mPrevButton.isClicked(mouse, true)) {
            mIsPlaying = false;
            if (!mHistory.empty()) { processPreviousAction(); setStatus("Stepped back."); } 
            else setStatus("Already at the start of operation.");
        }
    }

    if (event.type == sf::Event::TextEntered && mInputFocused) {
        const auto unicode = event.text.unicode;
        if (unicode == 8) backspaceInput();
        else if (unicode < 128) appendCharacter(static_cast<char>(unicode));
    }
}

void HeapVisualizer::update(float deltaTime, const sf::RenderWindow& window) {
    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    float targetLeft = mLeftExpanded ? SIDEBAR_MAX_WIDTH : TAB_WIDTH;
    float targetRight = mRightExpanded ? CODE_PANEL_MAX_WIDTH : TAB_WIDTH;
    mLeftWidth += (targetLeft - mLeftWidth) * 12.f * deltaTime;
    mRightWidth += (targetRight - mRightWidth) * 12.f * deltaTime;
    mWorkspaceCenterX = mLeftWidth + (mBaseWidth - mLeftWidth - mRightWidth) / 2.f;
    
    // --- Smooth Left Panel Layout Mathematics ---
    float leftBaseX = mLeftWidth - SIDEBAR_MAX_WIDTH;
    float currentY = 20.f;

    mReturnButton.setPosition({leftBaseX + 30.f + 40.f, currentY + 20.f}); currentY += 50.f;
    mInputBox.setPosition({leftBaseX + 30.f, currentY}); 
    mPlaceholderText.setPosition({leftBaseX + 30.f + 14.f, currentY + 8.f});
    mInputText.setPosition({leftBaseX + 30.f + 14.f, currentY + 8.f}); currentY += 50.f;
    mHintText.setPosition({leftBaseX + 30.f, currentY}); currentY += 30.f;

    // Grid System Synced with LinkedList Scene
    mInsertButton.setPosition({leftBaseX + 30.f + 52.5f, currentY + 20.f});
    mDeleteButton.setPosition({leftBaseX + 145.f + 52.5f, currentY + 20.f}); currentY += 50.f;
    
    mBuildButton.setPosition({leftBaseX + 30.f + 52.5f, currentY + 20.f});
    mClearButton.setPosition({leftBaseX + 145.f + 52.5f, currentY + 20.f}); currentY += 50.f;
    
    mUpdateButton.setPosition({leftBaseX + 30.f + 52.5f, currentY + 20.f});
    mRandomButton.setPosition({leftBaseX + 145.f + 52.5f, currentY + 20.f}); currentY += 50.f;

    mLoadButton.setPosition({leftBaseX + 30.f + 110.f, currentY + 20.f}); currentY += 70.f;

    // Controls
    mPrevButton.setPosition({leftBaseX + 30.f + 35.f, currentY + 20.f});
    mPlayPauseButton.setPosition({leftBaseX + 105.f + 35.f, currentY + 20.f});
    mStepButton.setPosition({leftBaseX + 180.f + 35.f, currentY + 20.f}); currentY += 50.f;

    mSkipButton.setPosition({leftBaseX + 30.f + 110.f, currentY + 20.f}); currentY += 60.f;

    mSpeedLabel.setPosition({leftBaseX + 30.f, currentY - 20.f});
    mSliderTrack.setPosition({leftBaseX + 30.f, currentY}); currentY += 30.f;

    // Customization anchors
    float startX_custom = leftBaseX + 30.f;
    mColorLabelPos = {startX_custom, currentY};
    for (size_t i = 0; i < mColorSwatches.size(); ++i) {
        float x = startX_custom + (i % 3) * 35.f;
        float y = currentY + 25.f + (i / 3) * 35.f;
        mColorSwatches[i].setPosition({x, y});
    }
    
    currentY += 100.f;
    mThemeLabelPos = {startX_custom, currentY};
    mDarkThemeBtn.setPosition({startX_custom + 40.f, currentY + 45.f});
    mLightThemeBtn.setPosition({startX_custom + 130.f, currentY + 45.f});

    currentY += 105.f;
    mSizeLabel.setPosition({startX_custom, currentY});
    mSizeSliderTrack.setPosition({startX_custom, currentY + 25.f});

    currentY += 75.f;
    mBgDefaultBtn.setPosition({startX_custom + 35.f, currentY + 45.f});
    mBgWhiteBtn.setPosition({startX_custom + 110.f, currentY + 45.f});
    mBgBlackBtn.setPosition({startX_custom + 185.f, currentY + 45.f});

    // --- Smooth Right Panel Layout Mathematics ---
    float rightBaseX = mBaseWidth - mRightWidth;
    mInfoTabBtn.setPosition({rightBaseX + TAB_WIDTH + 15.f + 40.f, 40.f});
    mCodeTabBtn.setPosition({rightBaseX + TAB_WIDTH + 15.f + 130.f, 40.f});

    if (mRightTabState == HeapRightTabState::CODE) {
        mCodePanel.setPosition({rightBaseX + TAB_WIDTH + 15.f, 80.f});
    } else {
        float startX = rightBaseX + TAB_WIDTH + 15.f;
        float textY = 80.f;
        for (auto& text : mInfoTexts) {
            text.setPosition({startX, textY});
            textY += text.getCharacterSize() + (text.getString().isEmpty() ? 5.f : 8.f);
        }
    }

    // Dynamically Center Status Text
    mStatusText.setPosition({mWorkspaceCenterX - mStatusText.getLocalBounds().width / 2.f, mBaseHeight - 80.f});

    // Updating hovers
    mDarkThemeBtn.update(mouse);
    mLightThemeBtn.update(mouse);
    mInsertButton.update(mouse);
    mDeleteButton.update(mouse);
    mBuildButton.update(mouse);
    mClearButton.update(mouse);
    mUpdateButton.update(mouse);
    mRandomButton.update(mouse);
    mSkipButton.update(mouse);
    mPlayPauseButton.update(mouse);
    mStepButton.update(mouse);
    mPrevButton.update(mouse);
    mLoadButton.update(mouse);
    mReturnButton.update(mouse);
    mBgDefaultBtn.update(mouse);
    mBgWhiteBtn.update(mouse);
    mBgBlackBtn.update(mouse);
    mInfoTabBtn.update(mouse);
    mCodeTabBtn.update(mouse);

    mInputBox.setOutlineColor(mInputFocused ? sf::Color(181, 58, 199, 200) : sf::Color(181, 58, 199, 120));
    mInputText.setString(mInputBuffer + (mInputFocused ? "|" : ""));
    mRootText.setString(mDisplayArray.empty() ? "Root: --" : "Root: " + std::to_string(mDisplayArray.front()));

    if (mIsDraggingSlider) {
        const float left = mSliderTrack.getPosition().x;
        const float width = mSliderTrack.getSize().x;
        const float newX = std::max(left, std::min(mouse.x, left + width));
        mSliderKnob.setPosition(newX, mSliderKnob.getPosition().y);

        const float t = (newX - left) / width;
        mActionInterval = MAX_INTERVAL - t * (MAX_INTERVAL - MIN_INTERVAL);

        std::stringstream ss;
        ss << "Speed: " << std::fixed << std::setprecision(2) << mActionInterval << "s";
        mSpeedLabel.setString(ss.str());
    } else {
        float t = 1.0f - (mActionInterval - MIN_INTERVAL) / (MAX_INTERVAL - MIN_INTERVAL);
        mSliderKnob.setPosition(mSliderTrack.getPosition().x + t * mSliderTrack.getSize().x, mSliderTrack.getPosition().y + 3.f);
    }

    if (mIsDraggingSizeSlider) {
        float trackX = mSizeSliderTrack.getPosition().x;
        float trackWidth = mSizeSliderTrack.getSize().x;
        float newX = std::max(trackX, std::min(mouse.x, trackX + trackWidth));
        float t = (newX - trackX) / trackWidth;
        mNodeRadius = 15.f + t * 15.f;
        mSizeSliderKnob.setPosition(newX, mSizeSliderTrack.getPosition().y + 3.f);
        mSizeLabel.setString("Node Size: " + std::to_string((int)mNodeRadius));
    } else {
        float t = (mNodeRadius - 15.f) / 15.f;
        mSizeSliderKnob.setPosition(mSizeSliderTrack.getPosition().x + t * mSizeSliderTrack.getSize().x, mSizeSliderTrack.getPosition().y + 3.f);
    }

    if (mInputFocused) {
        mInputBox.setOutlineColor(sf::Color(181, 58, 199, 120));
        mInputBox.setOutlineThickness(3.f);
    } else {
        mInputBox.setOutlineColor(sf::Color(100, 100, 100));
        mInputBox.setOutlineThickness(1.f);
    }

    if (!mIsPlaying || mPendingActions.empty()) return;

    mActionTimer += deltaTime;
    if (mActionTimer >= mActionInterval) {
        mActionTimer = 0.f;
        processNextAction();
    }
}

void HeapVisualizer::render(sf::RenderWindow& window) const {
    sf::RectangleShape solidBg({mBaseWidth, mBaseHeight});
    if (mBgType == BackgroundType::Black) {
        solidBg.setFillColor(sf::Color::Black);
        window.draw(solidBg);
    }
    else {
        
        if (mBgType == BackgroundType::White) 
        {
            solidBg.setFillColor(sf::Color::White);
            window.draw(solidBg);
        }
        else window.draw(mBgSprite);
        
    }
    
    drawTree(window);
    drawArray(window);
    drawLegend(window);
    window.draw(mStatusText);

    sf::Color panelColor = mIsDarkMode ? sf::Color(25, 25, 35, 150) : sf::Color(220, 220, 230, 150);
    
    // Draw Left Panel Architecture
    sf::RectangleShape leftMenu({mLeftWidth, mBaseHeight});
    leftMenu.setFillColor(panelColor);
    window.draw(leftMenu);

    sf::RectangleShape leftTab({TAB_WIDTH, 50.f});
    leftTab.setFillColor(sf::Color(45, 45, 50));
    leftTab.setPosition(mLeftWidth - TAB_WIDTH, mBaseHeight / 2.f - 25.f);
    window.draw(leftTab);

    sf::Text lIcon(mLeftExpanded ? "<<" : ">>", mFont, 18);
    sf::FloatRect lb = lIcon.getLocalBounds();
    lIcon.setOrigin(lb.left + lb.width/2.f, lb.top + lb.height/2.f);
    lIcon.setPosition(mLeftWidth - TAB_WIDTH/2.f, mBaseHeight / 2.f - 2.f);
    lIcon.setFillColor(sf::Color(110, 247, 242));
    window.draw(lIcon);

    // Draw Right Panel Architecture
    sf::RectangleShape rightMenu({mRightWidth, mBaseHeight});
    rightMenu.setPosition(mBaseWidth - mRightWidth, 0.f);
    rightMenu.setFillColor(panelColor);
    window.draw(rightMenu);

    sf::RectangleShape rightTab({TAB_WIDTH, 50.f});
    rightTab.setFillColor(sf::Color(45, 45, 50));
    rightTab.setPosition(mBaseWidth - mRightWidth, mBaseHeight / 2.f - 25.f);
    window.draw(rightTab);

    sf::Text rIcon(mRightExpanded ? ">>" : "<<", mFont, 18);
    sf::FloatRect rb = rIcon.getLocalBounds();
    rIcon.setOrigin(rb.left + rb.width/2.f, rb.top + rb.height/2.f);
    rIcon.setPosition(mBaseWidth - mRightWidth + TAB_WIDTH/2.f, mBaseHeight / 2.f - 2.f);
    rIcon.setFillColor(sf::Color(110, 247, 242));
    window.draw(rIcon);

    if (mLeftWidth > 180.f) {
        drawInputArea(window);
        drawButtons(window);
        drawColorPicker(window);
        drawThemeToggle(window);
        window.draw(mSizeLabel);
        window.draw(mSizeSliderTrack);
        window.draw(mSizeSliderKnob);
        drawBackgroundToggle(window);
    }

    if (mRightWidth > 180.f) {
        window.draw(mInfoTabBtn);
        window.draw(mCodeTabBtn);

        sf::RectangleShape underline({80.f, 2.f});
        underline.setFillColor(sf::Color::Cyan);
        underline.setOrigin(40.f, 1.f);
        if (mRightTabState == HeapRightTabState::INFO) {
            underline.setPosition({mInfoTabBtn.getPosition().x, mInfoTabBtn.getPosition().y + 18.f});
            window.draw(underline);
            for (const auto& text : mInfoTexts) {
                window.draw(text);
            }
        } else {
            underline.setPosition({mCodeTabBtn.getPosition().x, mCodeTabBtn.getPosition().y + 18.f});
            window.draw(underline);
            const_cast<CodePanel&>(mCodePanel).draw(window);
        }
    }
}

void HeapVisualizer::reset() {
    mInputFocused = false;
    mInputBuffer.clear();
    mInputText.setString("");
    mPendingActions.clear();
    mHistory.clear();
    mHighlight = {};
    mActionTimer = 0.f;
    mSelectedIndex = -1;
    mIsPlaying = false;
}

void HeapVisualizer::runLoadFile() {
    std::ifstream file("data/heap.txt");
    if (!file.is_open()) {
        setStatus("Could not open data/heap.txt");
        return;
    }

    std::vector<int> values;
    int val;
    while (file >> val) { values.push_back(val); }
    file.close();

    if (values.empty()) { setStatus("File is empty or invalid format."); return; }

    if (values.size() > MAX_RENDERED_NODES) {
        values.resize(MAX_RENDERED_NODES);
        setStatus("Loaded " + std::to_string(MAX_RENDERED_NODES) + " nodes (trimmed).");
    } else {
        setStatus("Loaded " + std::to_string(values.size()) + " nodes from file.");
    }

    loadHeapifyCode();
    mPendingActions.clear();
    mHistory.clear();
    mHighlight = {};
    mActionTimer = 0.f;
    clearInput();

    mHeap.BuildHeap(values);
    mDisplayArray = values;
    
    const std::vector<Action> actions = mHeap.flushActions();
    for (const Action& action : actions) {
        mPendingActions.push_back(action);
    }
    
    mIsPlaying = true;
}

void HeapVisualizer::runInsert() {
    int value = 0;
    if (!tryParseSingleValue(value)) {
        setStatus("Insert expects a single integer.");
        return;
    }
    loadInsertCode();
    const std::vector<int> startArray = mPendingActions.empty() ? mDisplayArray : mHeap.getArray();
    if (startArray.size() >= MAX_RENDERED_NODES - 1) {
        setStatus("Limit reached. Clear or use fewer than " + std::to_string(MAX_RENDERED_NODES) + " nodes.");
        return;
    }

    mHeap.BuildHeap(startArray);
    mHeap.flushActions();
    mHeap.Insert(value);
    queueOperation(startArray);
    clearInput();
    setStatus("Inserted " + std::to_string(value) + ".");
}

void HeapVisualizer::runDeleteSelected() {
    if (mSelectedIndex == -1) { setStatus("Please select a node to delete."); return; }

    const std::vector<int> startArray = mPendingActions.empty() ? mDisplayArray : mHeap.getArray();
    if (startArray.empty()) { setStatus("Heap is empty."); return; }

    loadHeapifyCode();
    int targetIdx = mSelectedIndex;
    int targetValue = startArray[targetIdx];
    mHeap.BuildHeap(startArray);

    mHeap.flushActions();
    mHeap.Delete(targetIdx);
    queueOperation(startArray);

    setStatus("Deleted node " + std::to_string(targetIdx) + " (Value: " + std::to_string(targetValue) + ").");
    mSelectedIndex = -1;
    mInputFocused = false;
    clearInput();
}

void HeapVisualizer::runBuildHeap() {
    bool ok = false;
    std::vector<int> values = parseSequence(ok);
    if (!ok) { setStatus("Build Heap expects integers separated by spaces or commas."); return; }
    loadHeapifyCode();
    if (values.size() > MAX_RENDERED_NODES) {
        values.resize(MAX_RENDERED_NODES);
        setStatus("Build input trimmed to 16 nodes for rendering.");
    } else if (values.empty()) {
        setStatus("Heap cleared.");
    } else {
        setStatus("Built heap from " + std::to_string(values.size()) + " values.");
    }

    mHeap.BuildHeap(values);
    mDisplayArray = values;
    mPendingActions.clear();
    mHighlight = {};
    mActionTimer = 0.f;
    clearInput();

    const std::vector<Action> actions = mHeap.flushActions();
    for (const Action& action : actions) { mPendingActions.push_back(action); }
}

void HeapVisualizer::runClear() {
    mHeap.BuildHeap({});
    mHeap.flushActions();
    mDisplayArray.clear();
    mPendingActions.clear();
    mHighlight = {};
    mInputBuffer.clear();
    mActionTimer = 0.f;
    setStatus("Heap cleared.");
}

void HeapVisualizer::runUpdate() {
    if (mSelectedIndex == -1) { setStatus("Please select a node first."); return; }
    if (mInputBuffer.empty()) { setStatus("Enter a new value in the input bar."); return; }

    int newValue;
    if (tryParseSingleValue(newValue)) {
        const std::vector<int> startArray = mDisplayArray;
        mHeap.BuildHeap(startArray);
        mHeap.flushActions();

        mHeap.Update(mSelectedIndex, newValue);
        queueOperation(startArray);

        if (mPendingActions.empty()) mIsPlaying = false;

        setStatus("Updated node " + std::to_string(mSelectedIndex) + " to " + std::to_string(newValue));
        mSelectedIndex = -1; 
        clearInput();
    }
}

void HeapVisualizer::runRandom() {
    clearInput();
    mPendingActions.clear();

    int n = std::rand() % (MAX_RENDERED_NODES - 16) + 16; 
    std::vector<int> randomValues;
    for (int i = 0; i < n; ++i) { randomValues.push_back(std::rand() % 100); }

    mDisplayArray = randomValues; 
    mHeap.BuildHeap(randomValues);
    queueOperation(randomValues); 
    
    mIsPlaying = true;
    loadHeapifyCode();
    setStatus("Randomized " + std::to_string(n) + " nodes. Starting Build...");
}

void HeapVisualizer::runSkip() {
    if (mPendingActions.empty()) { setStatus("Nothing to skip."); return; }
    while (!mPendingActions.empty()) { processNextAction(); }
    setStatus("Skipped to final state.");
}

void HeapVisualizer::togglePlayback() {
    mIsPlaying = !mIsPlaying;
    setStatus(mIsPlaying ? "Animation resumed." : "Animation paused.");
}

void HeapVisualizer::queueOperation(const std::vector<int>& startArray) {
    mDisplayArray = startArray;
    mPendingActions.clear();
    mHistory.clear();
    mHighlight = {};
    mActionTimer = 0.f;

    const std::vector<Action> actions = mHeap.flushActions();
    for (const Action& action : actions) { mPendingActions.push_back(action); }

    if (mPendingActions.empty()) { mDisplayArray = mHeap.getArray(); } 
    else if (!mIsPlaying) { processNextAction(); }
}

void HeapVisualizer::processNextAction() {
    if (mPendingActions.empty()) {
        mActiveLine = -1;
        mHighlight = {};
        mCodePanel.highlight(-1);
        return;
    }

    const Action action = mPendingActions.front();
    mPendingActions.pop_front();
    mHistory.push_back(action);

    mHighlight = {};
    mActiveLine = action.lineIdx;
    mCodePanel.highlight(action.lineIdx);

    switch (action.type) {
        case ActionType::INSERT:
            if (action.index1 >= 0 && static_cast<std::size_t>(action.index1) <= mDisplayArray.size()) {
                mDisplayArray.insert(mDisplayArray.begin() + action.index1, action.index2);
                mHighlight.first = action.index1;
                mHighlight.firstColor = sf::Color(248, 196, 76);
                mHighlight.label = "Inserted " + std::to_string(action.index2);
            }
            break;

        case ActionType::COMPARE:
            mHighlight.first = action.index1;
            mHighlight.second = action.index2;
            mHighlight.firstColor = sf::Color(0, 255, 255);
            mHighlight.secondColor = sf::Color(0, 255, 255);
            mHighlight.label = "Compare";
            break;

        case ActionType::SWAP:
            if (action.index1 >= 0 && action.index2 >= 0 &&
                static_cast<std::size_t>(action.index1) < mDisplayArray.size() &&
                static_cast<std::size_t>(action.index2) < mDisplayArray.size()) {
                
                std::swap(mDisplayArray[action.index1], mDisplayArray[action.index2]);
                mHighlight.first = action.index1;
                mHighlight.second = action.index2;
                mHighlight.firstColor = sf::Color(255, 124, 124);
                mHighlight.secondColor = sf::Color(255, 124, 124);
                mHighlight.label = "Swap";
            }
            break;

        case ActionType::HIGHLIGHT:
            mHighlight.first = action.index1;
            mHighlight.firstColor = sf::Color(248, 196, 76);
            mHighlight.label = "Focus";
            break;

        case ActionType::REMOVE:
            if (action.index1 >= 0 && static_cast<std::size_t>(action.index1) < mDisplayArray.size()) {
                mDisplayArray.erase(mDisplayArray.begin() + action.index1);
                mHighlight = {};
                mHighlight.label = "Removed " + std::to_string(action.index2);
            }
            break;
        
        case ActionType::CHANGE_VALUE:
            if (action.index1 >= 0 && static_cast<std::size_t>(action.index1) < mDisplayArray.size()) {
                mDisplayArray[action.index1] = action.index2;
                mHighlight = {action.index1, -1, sf::Color(248, 196, 76), sf::Color::Transparent, "Changing value"};
            }
            break;
    }
}

void HeapVisualizer::processPreviousAction() {
    if (mHistory.empty()) {
        setStatus("At the beginning of operation.");
        mCodePanel.highlight(-1);
        return;
    }

    Action action = mHistory.back();
    mHistory.pop_back();

    mHighlight = {};

    switch (action.type) {
        case ActionType::INSERT:
            if (action.index1 >= 0 && static_cast<std::size_t>(action.index1) < mDisplayArray.size()) {
                mDisplayArray.erase(mDisplayArray.begin() + action.index1);
                mHighlight.label = "Undo Insert";
            }
            break;

        case ActionType::SWAP:
            if (action.index1 >= 0 && action.index2 >= 0) {
                std::swap(mDisplayArray[action.index1], mDisplayArray[action.index2]);
                mHighlight.first = action.index1;
                mHighlight.second = action.index2;
                mHighlight.firstColor = sf::Color(255, 124, 124);
                mHighlight.secondColor = sf::Color(255, 124, 124);
                mHighlight.label = "Undo Swap";
            }
            break;

        case ActionType::COMPARE:
            mHighlight.firstColor = sf::Color(0, 255, 255);
            mHighlight.secondColor = sf::Color(0, 255, 255);
            break;

        case ActionType::HIGHLIGHT:
            mHighlight.label = "Undo " + std::string(action.type == ActionType::COMPARE ? "Compare" : "Focus");
            break;

        case ActionType::REMOVE:
            if (action.index1 >= 0 && static_cast<std::size_t>(action.index1) <= mDisplayArray.size()) {
                mDisplayArray.insert(mDisplayArray.begin() + action.index1, action.index2);
                mHighlight.first = action.index1;
                mHighlight.label = "Undo Remove";
            }
            break;

        case ActionType::CHANGE_VALUE:
            if (action.index1 >= 0 && static_cast<std::size_t>(action.index1) < mDisplayArray.size()) {
                mDisplayArray[action.index1] = action.index2;
                mHighlight = {action.index1, -1, sf::Color(248, 196, 76), sf::Color::Transparent, "Undo changes"};
            }
            break;
    }

    mCodePanel.highlight(action.lineIdx);

    if (mHistory.empty()) {
        mActiveLine = -1;
    } else {
        mActiveLine = mHistory.back().lineIdx;
    }

    mPendingActions.push_front(action);
}

void HeapVisualizer::loadHeapifyCode() {
    std::vector<std::string> code = {
        "heapify(i):",
        "  l = 2i + 1, r = 2i + 2",
        "  lar = i",
        "  if l < sz && A[l] > A[i]:",
        "    lar = l",
        "  if r < sz && A[r] > A[lar]:",
        "    lar = r",
        "  if lar != i:",
        "    swap(A[i], A[lar])",
        "    heapify(lar)"
    };
    mCodePanel.setCode(code);
}

void HeapVisualizer::loadInsertCode() {
    std::vector<std::string> code = {
        "insert(val):",
        "  A.push(val)",
        "  i = last_idx",
        "  while i > 0 && A[p] < A[i]:",
        "    swap(A[i], A[p])",
        "    i = parent(i)"
    };
    mCodePanel.setCode(code);
}

void HeapVisualizer::drawInputArea(sf::RenderWindow& window) const {
    window.draw(mInputBox);
    if (mInputBuffer.empty()) {
        window.draw(mPlaceholderText);
    } else {
        window.draw(mInputText);
    }
    window.draw(mHintText);
}

void HeapVisualizer::drawButtons(sf::RenderWindow& window) const {
    window.draw(mInsertButton);
    window.draw(mDeleteButton); 
    window.draw(mBuildButton);
    window.draw(mClearButton);
    window.draw(mUpdateButton);
    window.draw(mRandomButton);
    window.draw(mSkipButton);
    window.draw(mLoadButton);
    window.draw(mReturnButton);
    window.draw(mPlayPauseButton);
    window.draw(mStepButton);
    window.draw(mPrevButton);
    window.draw(mSpeedLabel);
    window.draw(mSliderTrack);
    window.draw(mSliderKnob);
}

void HeapVisualizer::drawArray(sf::RenderWindow& window) const {
    const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
    if (visibleNodes == 0) return;

    const float availableWorkspaceWidth = (mBaseWidth - mLeftWidth - mRightWidth) * 0.95f;
    const float gap = 4.f;

    const float cellWidth = std::min(48.f, (availableWorkspaceWidth - (visibleNodes * gap)) / visibleNodes);
    const float totalArrayWidth = (visibleNodes * cellWidth) + ((visibleNodes - 1) * gap);

    const float startX = mWorkspaceCenterX - (totalArrayWidth / 2.f);
    sf::Color indexColor = (mBgType != BackgroundType::Black) ? sf::Color(50, 50, 60) : sf::Color(200, 200, 210);

    for (std::size_t i = 0; i < visibleNodes; ++i) {
        float currentX = startX + i * (cellWidth + gap);
        
        sf::RectangleShape cell({cellWidth, cellWidth});
        cell.setPosition({currentX, ARRAY_Y});
        cell.setFillColor(mCurrentNodeColor);
        cell.setOutlineThickness(3.f);

        sf::Color currentOutlineColor = nodeColor(i);
        if (mSelectedIndex == static_cast<int>(i)) {
            currentOutlineColor = sf::Color(34, 139, 34);
            cell.setOutlineThickness(3.f);
        }
        cell.setOutlineColor(currentOutlineColor);
        window.draw(cell);

        unsigned int fontSize = cellWidth < 35.f ? 12 : 18;
        sf::Text valueText = makeText(mFont, std::to_string(mDisplayArray[i]), fontSize, sf::Color(20, 28, 40), {0.f, 0.f});
        
        sf::FloatRect bounds = valueText.getLocalBounds();
        valueText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);

        valueText.setPosition({currentX + cellWidth / 2.f, ARRAY_Y + cellWidth / 2.f - 2.f});
        window.draw(valueText);

        sf::Text indexText = makeText(mFont, std::to_string(i), 12, indexColor, {0.f, 0.f});
        sf::FloatRect idxBounds = indexText.getLocalBounds();
        indexText.setOrigin(idxBounds.left + idxBounds.width / 2.f, 0.f);
        indexText.setPosition({currentX + cellWidth / 2.f, ARRAY_Y - 18.f});
        window.draw(indexText);
    }
}

void HeapVisualizer::drawTree(sf::RenderWindow& window) const {
    const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);

    sf::Color edgeColor;
    if (mBgType == BackgroundType::Default || mBgType == BackgroundType::White) {
        edgeColor = sf::Color::Black;
    } else {
        edgeColor = sf::Color(136, 155, 184);
    }

    for (std::size_t i = 1; i < visibleNodes; ++i) {
        const std::size_t parent = (i - 1) / 2;
        sf::Vector2f start = nodePosition(parent);
        sf::Vector2f end = nodePosition(i);
        sf::Vector2f direction = end - start;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159f;
        sf::RectangleShape line({length, 3.0f});
        line.setOrigin({0.f, 1.5f});
        line.setPosition(start);
        line.setRotation(angle);
        line.setFillColor(edgeColor);

        window.draw(line);
    }

    for (std::size_t i = 0; i < visibleNodes; ++i) {
        float currentRadius = (i >= 15) ? mNodeRadius * 0.8f : mNodeRadius;
        
        sf::CircleShape node(currentRadius);
        node.setOrigin({currentRadius, currentRadius});
        node.setPosition(nodePosition(i));
        node.setFillColor(mCurrentNodeColor);
        node.setOutlineThickness(4.f);

        sf::Color currentOutlineColor = nodeColor(i);
        if (mSelectedIndex == static_cast<int>(i)) {
            currentOutlineColor = sf::Color(34, 139, 34);
            node.setOutlineThickness(currentRadius * 0.25f);
        }
        node.setOutlineColor(currentOutlineColor);
        window.draw(node);

        unsigned int dynamicFontSize = static_cast<unsigned int>(currentRadius * 0.75f);
        sf::Text valueText = makeText(mFont, std::to_string(mDisplayArray[i]), dynamicFontSize, sf::Color(20, 28, 40), {0.f, 0.f});
        
        sf::FloatRect bounds = valueText.getLocalBounds();
        valueText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        valueText.setPosition({node.getPosition().x, node.getPosition().y - 1.f});
        window.draw(valueText);
    }
}

void HeapVisualizer::drawLegend(sf::RenderWindow& window) const {
    sf::Color legendTextColor = (mBgType == BackgroundType::Black) ? sf::Color::White : sf::Color(20, 28, 40);
    float LEGEND_Y = mBaseHeight - 150.f;

    if (!mHighlight.label.empty()) {
        sf::Text stepText = makeText(mFont, "Step: " + mHighlight.label, 16, sf::Color(251, 209, 101), {0.f, 0.f});
        stepText.setPosition({mWorkspaceCenterX - stepText.getLocalBounds().width / 2.f, LEGEND_Y - 40.f});
        window.draw(stepText);
    }

    float itemX = mWorkspaceCenterX - 180.f; 

    sf::CircleShape compare(8.f);
    compare.setFillColor(sf::Color(0, 255, 255));
    compare.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(compare);
    window.draw(makeText(mFont, "Compare", 15, legendTextColor, {itemX + 25.f, LEGEND_Y}));

    itemX += 120.f;
    sf::CircleShape swap(8.f);
    swap.setFillColor(sf::Color(255, 124, 124));
    swap.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(swap);
    window.draw(makeText(mFont, "Swap", 15, legendTextColor, {itemX + 25.f, LEGEND_Y}));

    itemX += 90.f;
    sf::CircleShape focus(8.f);
    focus.setFillColor(sf::Color(248, 196, 76));
    focus.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(focus);
    window.draw(makeText(mFont, "Focused", 15, legendTextColor, {itemX + 25.f, LEGEND_Y}));

    itemX += 110.f;
    sf::CircleShape selected(8.f);
    selected.setFillColor(sf::Color(34, 139, 34));
    selected.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(selected);
    window.draw(makeText(mFont, "Selected", 15, legendTextColor, {itemX + 25.f, LEGEND_Y}));
}

void HeapVisualizer::drawColorPicker(sf::RenderWindow& window) const {
    sf::Text label = makeText(mFont, "Node Fill Color", 14, mIsDarkMode ? sf::Color(200, 200, 210) : sf::Color(50, 50, 60), mColorLabelPos);
    window.draw(label);

    for (size_t i = 0; i < mColorSwatches.size(); ++i) {
        auto& swatch = const_cast<sf::RectangleShape&>(mColorSwatches[i]);
        if (mThemeColors[i] == mCurrentNodeColor) {
            swatch.setOutlineColor(sf::Color::Yellow);
            swatch.setOutlineThickness(2.5f);
        } else {
            swatch.setOutlineColor(sf::Color(100, 100, 100));
            swatch.setOutlineThickness(1.5f);
        }
        window.draw(swatch);
    }
}

void HeapVisualizer::drawBackgroundToggle(sf::RenderWindow& window) const {
    float startX = mThemeLabelPos.x;
    float startY = mBgDefaultBtn.getPosition().y - 45.f; 

    sf::Text label = makeText(mFont, "Background Style", 14, 
        mIsDarkMode ? sf::Color(200, 200, 210) : sf::Color(50, 50, 60), {startX, startY});
    window.draw(label);

    auto& btnD = const_cast<ModernButton&>(mBgDefaultBtn);
    auto& btnW = const_cast<ModernButton&>(mBgWhiteBtn);
    auto& btnB = const_cast<ModernButton&>(mBgBlackBtn);

    sf::Color currentTop, currentBot, inactiveBorder, textColor;
    if (mIsDarkMode) {
        currentTop = sf::Color(32, 26, 43);
        currentBot = sf::Color(20, 16, 27);
        inactiveBorder = sf::Color(100, 100, 100, 150);
        textColor = sf::Color::White;
    } else {
        currentTop = sf::Color(230, 230, 240);
        currentBot = sf::Color(200, 200, 215);
        inactiveBorder = sf::Color(150, 150, 160);
        textColor = sf::Color(40, 40, 50); 
    }

    sf::Color activeBorder = sf::Color::Cyan;

    btnD.setColors(currentTop, currentBot, mBgType == BackgroundType::Default ? activeBorder : inactiveBorder, textColor);
    btnW.setColors(currentTop, currentBot, mBgType == BackgroundType::White ? activeBorder : inactiveBorder, textColor);
    btnB.setColors(currentTop, currentBot, mBgType == BackgroundType::Black ? activeBorder : inactiveBorder, textColor);

    window.draw(btnD);
    window.draw(btnW);
    window.draw(btnB);
}

void HeapVisualizer::drawThemeToggle(sf::RenderWindow& window) const {
    sf::Text label = makeText(mFont, "UI Theme", 14, 
        mIsDarkMode ? sf::Color(200, 200, 210) : sf::Color(50, 50, 60), mThemeLabelPos);
    window.draw(label);

    auto& btnD = const_cast<ModernButton&>(mDarkThemeBtn);
    auto& btnL = const_cast<ModernButton&>(mLightThemeBtn);

    sf::Color darkTop(32, 26, 43);
    sf::Color darkBot(20, 16, 27);
    sf::Color lightTop(230, 230, 240);
    sf::Color lightBot(200, 200, 215);
    sf::Color activeBorder = sf::Color::Cyan;
    sf::Color inactiveBorder = sf::Color(100, 100, 100, 150);

    btnD.setColors(darkTop, darkBot, mIsDarkMode ? activeBorder : inactiveBorder);
    btnL.setColors(lightTop, lightBot, !mIsDarkMode ? activeBorder : inactiveBorder);
    window.draw(btnD);
    window.draw(btnL);
}

void HeapVisualizer::applyTheme() {
    if (mIsDarkMode) {
        mInputBox.setFillColor(sf::Color(32, 26, 43));
        mInputText.setFillColor(sf::Color::White);
        mSliderTrack.setFillColor(sf::Color(60, 60, 80));
        mSliderTrack.setOutlineColor(sf::Color(100, 100, 150));
        mSliderKnob.setFillColor(sf::Color(181, 58, 199));
        mSpeedLabel.setFillColor(sf::Color::White);
        mSizeSliderKnob.setFillColor(sf::Color(181, 58, 199));
        mSizeLabel.setFillColor(sf::Color::White);
        mSizeSliderTrack.setFillColor(sf::Color(60, 60, 80));
        
        mInsertButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mDeleteButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mBuildButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mClearButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mReturnButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mPlayPauseButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mStepButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mLoadButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mRandomButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mSkipButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mUpdateButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mPrevButton.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        
        mBgDefaultBtn.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mBgWhiteBtn.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));
        mBgBlackBtn.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120));

        mInfoTabBtn.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120), sf::Color::White);
        mCodeTabBtn.setColors(sf::Color(32, 26, 43), sf::Color(20, 16, 27), sf::Color(181, 58, 199, 120), sf::Color::White);
    } 
    else {
        mInputBox.setFillColor(sf::Color(255, 255, 255));
        mInputText.setFillColor(sf::Color(20, 28, 40));
        mHintText.setFillColor(sf::Color(60, 60, 70));
        mSliderTrack.setFillColor(sf::Color(200, 200, 210));
        mSliderTrack.setOutlineColor(sf::Color(160, 160, 170));
        mSliderKnob.setFillColor(sf::Color(52, 152, 219));
        mSpeedLabel.setFillColor(sf::Color::Black);
        mSizeSliderKnob.setFillColor(sf::Color(52, 152, 219));
        mSizeLabel.setFillColor(sf::Color(50, 50, 60));
        mSizeSliderTrack.setFillColor(sf::Color(180, 180, 190));
        
        mInsertButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mDeleteButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mBuildButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mClearButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mReturnButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mRandomButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mPlayPauseButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mStepButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mPrevButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mLoadButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mSkipButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mUpdateButton.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        
        mBgDefaultBtn.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mBgWhiteBtn.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mBgBlackBtn.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));

        mInfoTabBtn.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
        mCodeTabBtn.setColors(sf::Color(230, 230, 240), sf::Color(200, 200, 215), sf::Color(150, 150, 160), sf::Color(40, 40, 50));
    }
    
    // Update Info Texts Color safely
    for (auto& text : mInfoTexts) {
        std::string s = text.getString();
        if (s == "HEAP") {
            text.setFillColor(mIsDarkMode ? sf::Color(251, 209, 101) : sf::Color(200, 150, 0));
        } else if (!s.empty() && s.front() == '[' && s.back() == ']') {
            text.setFillColor(mIsDarkMode ? sf::Color(110, 247, 242) : sf::Color(0, 150, 200));
        } else {
            text.setFillColor(mIsDarkMode ? sf::Color(245, 245, 250) : sf::Color::Black);
        }
    }
}

void HeapVisualizer::appendDigit(char digit) {
    if (mInputBuffer.size() < 60) {
        mInputBuffer.push_back(digit);
    }
}

void HeapVisualizer::appendCharacter(char character) {
    if ((character >= '0' && character <= '9') || character == '-' || character == ',' || character == ' ') {
        appendDigit(character);
    }
}

void HeapVisualizer::backspaceInput() {
    if (!mInputBuffer.empty()) {
        mInputBuffer.pop_back();
        mInputText.setString(mInputBuffer);
    }
}

void HeapVisualizer::setStatus(const std::string& status) {
    mStatusMessage = status;
    mStatusText.setString(status);
    mPlayPauseButton.setText(mIsPlaying ? "PAUSE" : "PLAY");
}

bool HeapVisualizer::tryParseSingleValue(int& value) const {
    std::stringstream stream(mInputBuffer);
    stream >> value;
    return !stream.fail() && stream.eof();
}

std::vector<int> HeapVisualizer::parseSequence(bool& ok) const {
    std::vector<int> values;
    ok = true;

    std::string normalized = mInputBuffer;
    std::replace(normalized.begin(), normalized.end(), ',', ' ');

    std::stringstream stream(normalized);
    int value = 0;
    while (stream >> value) { values.push_back(value); }
    if (!stream.eof()) { ok = false; }

    return values;
}

sf::Vector2f HeapVisualizer::nodePosition(std::size_t index) const {
    const int level = static_cast<int>(std::floor(std::log2(static_cast<float>(index + 1))));
    const std::size_t nodesInLevel = 1u << level;

    float availableWidth = (mBaseWidth - mLeftWidth - mRightWidth) - 60.f;
    const float horizontalGap = availableWidth / static_cast<float>(nodesInLevel);

    const std::size_t firstIndexInLevel = (1u << level) - 1u;
    const std::size_t positionInLevel = index - firstIndexInLevel;

    float levelWidth = horizontalGap * nodesInLevel;
    float xStartOfLevel = mWorkspaceCenterX - (levelWidth / 2.f);

    const float x = xStartOfLevel + horizontalGap * (static_cast<float>(positionInLevel) + 0.5f);
    const float y = TREE_TOP_Y + level * 80.f;

    return {x, y};
}

sf::Color HeapVisualizer::nodeColor(std::size_t index) const {
    if (static_cast<int>(index) == mHighlight.first) { return mHighlight.firstColor; }
    if (static_cast<int>(index) == mHighlight.second) { return mHighlight.secondColor; }
    return sf::Color(106, 133, 176);
}

void HeapVisualizer::clearInput() {
    mInputBuffer.clear(); 
    mInputText.setString(""); 
}

int HeapVisualizer::run(sf::RenderWindow& window, sf::Font& font) {
    reset();
    
    // Get actual window sizes right away
    mBaseWidth = window.getSize().x;
    mBaseHeight = window.getSize().y;

    sf::View originalView = window.getView(); 
    sf::View dynamicView(sf::FloatRect(0, 0, mBaseWidth, mBaseHeight));
    window.setView(dynamicView);
    
    mWorkspaceBg.setSize({mBaseWidth, mBaseHeight});

    if (notLoaded) {
        if (!mBgTexture.loadFromFile("assets/textures/heap_background.png")) {
            std::cerr << "Failed to load background.png\n";
        }
        notLoaded = false;
        mBgSprite.setTexture(mBgTexture);
        mBgSprite.setScale(mBaseWidth / mBgTexture.getSize().x, mBaseHeight / mBgTexture.getSize().y);
    }

    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }

            if (event.type == sf::Event::Resized) {
                // dynamically update the base width and height to 1:1 window ratio
                mBaseWidth = event.size.width;
                mBaseHeight = event.size.height;

                sf::View resizedView(sf::FloatRect(0, 0, mBaseWidth, mBaseHeight));
                window.setView(resizedView);
                
                mWorkspaceBg.setSize({mBaseWidth, mBaseHeight});
                if (!notLoaded) {
                    mBgSprite.setScale(mBaseWidth / mBgTexture.getSize().x, mBaseHeight / mBgTexture.getSize().y);
                }
            }

            const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (event.type == sf::Event::MouseButtonReleased &&
                event.mouseButton.button == sf::Mouse::Left &&
                mReturnButton.isClicked(mouse, true)) {
                    runSkip();
                    window.setView(originalView);
                    return 0;
                }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.setView(originalView);
                return 0;
            }
            handleEvent(event, window);
        }

        update(deltaTime, window);

        window.clear();
        render(window); 
        window.display();
    }

    window.setView(originalView);
    return -1;
}