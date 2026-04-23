#include "DSA-Visualization/ui/heap_screen.hpp"
#include <iostream>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace {
    // Panel
    constexpr float PANEL_WIDTH = 1280.f;
    constexpr float PANEL_HEIGHT = 720.f;

    // Button
    constexpr float BUTTON_WIDTH = 82.f;
    constexpr float BUTTON_HEIGHT = 40.f;
    constexpr float BUTTON_X = 24.f;
    constexpr float BUTTON_GAP_Y = 14.f;
    constexpr float BUTTON_START_Y = 108.f; 
    constexpr float BUTTON_GAP_X = 10.f;
    constexpr float BUTTON_X2 = BUTTON_X + BUTTON_WIDTH + BUTTON_GAP_X;

    // Input
    constexpr float INPUT_X = 24.f;
    constexpr float INPUT_Y = 24.f;
    constexpr float INPUT_WIDTH = 174.f;
    constexpr float INPUT_HEIGHT = 44.f;

    // Tree + Array
    constexpr std::size_t MAX_RENDERED_NODES = 32;
    constexpr float NODE_RADIUS = 24.f;
    constexpr float TREE_WIDTH = 760.f;
    constexpr float TREE_LEFT_X = 260.f;
    constexpr float TREE_TOP_Y = 150.f;
    constexpr float ARRAY_Y = 40.f;

    // Helper
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
    // Input Area
    , mSpeedLabel(makeText(font, "Speed: 0.60s", 14, sf::Color::White, {BUTTON_X, BUTTON_START_Y + 4 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + 12.f}))
    , mPlaceholderText(makeText(font, "Enter value...", 18, sf::Color(120, 112, 138, 190), {INPUT_X + 14.f, INPUT_Y + 8.f}))
    , mInputText(makeText(font, "", 22, sf::Color::White, {INPUT_X + 14.f, INPUT_Y + 8.f}))
    , mHintText(makeText(font, "Build format: 1, 2, 3...", 14, sf::Color(150, 150, 150), {INPUT_X, INPUT_Y + 55.f}))
    , mStatusText(makeText(font, "", 16, sf::Color(251, 209, 101), {420.f, 640.f}))
    
    // Button
    , mInsertButton("Insert", font, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mDeleteButton("Delete", font, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mBuildButton("Build", font, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mClearButton("Clear", font, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mPrevButton("<", font, {BUTTON_WIDTH / 2.f, BUTTON_HEIGHT})
    , mPlayPauseButton("Pause", font, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mStepButton(">", font, {BUTTON_WIDTH / 2.f, BUTTON_HEIGHT})
    , mLoadButton("Load File", font, {BUTTON_WIDTH * 2 + BUTTON_GAP_X, BUTTON_HEIGHT})
    , mReturnButton("Return", font, {BUTTON_WIDTH * 2 + BUTTON_GAP_X, BUTTON_HEIGHT})
    , mRandomButton("Random", font, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mSkipButton("Skip Animation", font, {BUTTON_WIDTH * 2 + BUTTON_GAP_X, BUTTON_HEIGHT})
    , mUpdateButton("Update", font, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mLeftCollapseBtn(">>", font, {30.f, 60.f}, 5.f)
    , mRightCollapseBtn("<<", font, {30.f, 60.f}, 5.f)
{
    // Panel
    float cpX = 1058.f;
    float cpY = 24.f;
    float cpWidth = 204.f;
    float cpHeight = 318.f;
    mPanel.setSize({PANEL_WIDTH, PANEL_HEIGHT});
    mPanel.setFillColor(sf::Color(10, 10, 15));
    mCodePanel = CodePanel(font, {cpX, cpY}, {cpWidth, cpHeight});
    loadHeapifyCode();
    mControlPanelBg.setSize({240.f, 720.f});
    mControlPanelBg.setPosition({1040.f, 0.f});
    mControlPanelBg.setFillColor(sf::Color(25, 25, 35, 200));
    mControlPanelBg.setOutlineThickness(1.f);
    mControlPanelBg.setOutlineColor(sf::Color(80, 80, 100));

    mCodeBox.setSize({240.f, 720.f});
    mCodeBox.setPosition({0.f, 0.f});
    mCodeBox.setFillColor(sf::Color(25, 25, 35, 200));
    mCodeBox.setOutlineThickness(1.f);
    mCodeBox.setOutlineColor(sf::Color(80, 80, 100));

    // Speed Slider
    float sliderY = BUTTON_START_Y + 6 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + 20.f;
    float sliderWidth = BUTTON_WIDTH * 2 + BUTTON_GAP_X;
    mSliderTrack.setSize({sliderWidth, 6.f});
    mSliderTrack.setPosition({BUTTON_X, sliderY});
    mSliderTrack.setFillColor(sf::Color(60, 60, 80));
    mSliderTrack.setOutlineThickness(1.f);
    mSliderTrack.setOutlineColor(sf::Color(100, 100, 150));
    mSliderKnob.setRadius(10.f);
    mSliderKnob.setFillColor(sf::Color(181, 58, 199));
    mSliderKnob.setOrigin(10.f, 10.f);
    float initialT = 1.0f - (mActionInterval - 0.1f) / (2.0f - 0.1f);
    mSliderKnob.setPosition({BUTTON_X + initialT * sliderWidth, sliderY + 3.f});
    mSpeedLabel.setPosition({BUTTON_X, sliderY - 22.f});

    //Input area
    mInputBox.setPosition({INPUT_X, INPUT_Y});
    mInputBox.setSize({INPUT_WIDTH, INPUT_HEIGHT});
    mInputBox.setFillColor(sf::Color(32, 26, 43));
    mInputBox.setOutlineThickness(2.f);
    mInputBox.setOutlineColor(sf::Color(181, 58, 199, 120));

    // Button
    mInsertButton.setPosition({BUTTON_X + BUTTON_WIDTH / 2.f, BUTTON_START_Y + BUTTON_HEIGHT / 2.f});
    mDeleteButton.setPosition({BUTTON_X2 + BUTTON_WIDTH / 2.f, BUTTON_START_Y + BUTTON_HEIGHT / 2.f});
    mBuildButton.setPosition({BUTTON_X + BUTTON_WIDTH / 2.f, BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mClearButton.setPosition({BUTTON_X2 + BUTTON_WIDTH / 2.f, BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mUpdateButton.setPosition({BUTTON_X + BUTTON_WIDTH / 2.f, BUTTON_START_Y + 2 *(BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mRandomButton.setPosition({BUTTON_X2 + BUTTON_WIDTH / 2.f, BUTTON_START_Y + 2 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mSkipButton.setPosition({BUTTON_X + (BUTTON_WIDTH * 2 + BUTTON_GAP_X) / 2.f, BUTTON_START_Y + 3 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mLoadButton.setPosition({BUTTON_X + (BUTTON_WIDTH * 2 + BUTTON_GAP_X) / 2.f, BUTTON_START_Y + 4 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mPrevButton.setPosition({BUTTON_X + (BUTTON_WIDTH / 4.f), BUTTON_START_Y + 5 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mPlayPauseButton.setPosition({BUTTON_X + BUTTON_WIDTH + BUTTON_GAP_X / 2.f, BUTTON_START_Y + 5 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mStepButton.setPosition({BUTTON_X + 1.75f * BUTTON_WIDTH + BUTTON_GAP_X, BUTTON_START_Y + 5 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mReturnButton.setPosition({BUTTON_X + (BUTTON_WIDTH * 2 + BUTTON_GAP_X) / 2.f, 626.f + BUTTON_HEIGHT / 2.f});
    
    //Customization
    mCurrentNodeColor = sf::Color(245, 249, 255); 

    mThemeColors = {
        sf::Color(245, 249, 255),
        sf::Color(181, 58, 199),
        sf::Color(52, 152, 219),
        sf::Color(231, 76, 60),
        sf::Color(241, 196, 15),
        sf::Color(46, 204, 113)
    };

    for (const auto& color : mThemeColors) {
        sf::RectangleShape swatch(sf::Vector2f(25.f, 25.f));
        swatch.setFillColor(color);
        swatch.setOutlineThickness(1.5f);
        swatch.setOutlineColor(sf::Color(100, 100, 100));
        mColorSwatches.push_back(swatch);
    }

    setStatus("Ready.");
}

// Routes mouse and keyboard input to the correct heap action or text field update.
void HeapVisualizer::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    //Mouse position
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool mouseOverButtons = mDeleteButton.getGlobalBounds().contains(mouse) || mUpdateButton.getGlobalBounds().contains(mouse);
    bool mouseOverInput = mInputBox.getGlobalBounds().contains(mouse);
    bool mouseOverSlider = mSliderTrack.getGlobalBounds().contains(mouse) || mSliderKnob.getGlobalBounds().contains(mouse);

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        //Node selection
        if (mPendingActions.empty()) {
            const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
            for (std::size_t i = 0; i < visibleNodes; ++i) {
                sf::FloatRect nodeBounds(nodePosition(i).x - NODE_RADIUS, nodePosition(i).y - NODE_RADIUS, NODE_RADIUS * 2, NODE_RADIUS * 2);
                if (nodeBounds.contains(mouse)) {
                    mSelectedIndex = static_cast<int>(i);
                    mInputFocused = true;
                    mInputBuffer.clear();
                    mInputText.setString("");
                    setStatus("Node " + std::to_string(i) + " (value " + std::to_string(mDisplayArray[i]) + ") selected.");
                    return;
                }
            }
        }

        if (mouseOverButtons) {
            return;
        }

        //Slider Drag
        sf::FloatRect knobBounds = mSliderKnob.getGlobalBounds();
        knobBounds.left -= 10.f; knobBounds.width += 20.f;
        knobBounds.top -= 10.f;  knobBounds.height += 20.f;

        if (knobBounds.contains(mouse) || mSliderTrack.getGlobalBounds().contains(mouse)) {
            mIsDraggingSlider = true;

            const float left = mSliderTrack.getPosition().x;
            const float width = mSliderTrack.getSize().x;
            const float newX = std::max(left, std::min(mouse.x, left + width));
            mSliderKnob.setPosition(newX, mSliderKnob.getPosition().y);

            const float t = (newX - left) / width;
            mActionInterval = MAX_INTERVAL - t * (MAX_INTERVAL - MIN_INTERVAL);

            std::stringstream ss;
            ss << "Speed: " << std::fixed << std::setprecision(2) << mActionInterval << "s";
            mSpeedLabel.setString(ss.str());
        }
        
        //Color selection
        if (mRightExpanded && mouse.x > 1280.f - mRightWidth) {
            for (size_t i = 0; i < mColorSwatches.size(); ++i) {
                if (mColorSwatches[i].getGlobalBounds().contains(mouse)) {
                    mCurrentNodeColor = mThemeColors[i];
                    setStatus("Node color updated.");
                    return; 
                }
                }
        }
    }

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        mIsDraggingSlider = false;

        if (!mouseOverButtons && !mouseOverSlider) {
            bool mouseOverNode = false;
            const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
            for (std::size_t i = 0; i < visibleNodes; ++i) {
                sf::FloatRect nodeBounds(nodePosition(i).x - NODE_RADIUS, nodePosition(i).y - NODE_RADIUS, NODE_RADIUS * 2, NODE_RADIUS * 2);
                if (nodeBounds.contains(mouse)) {
                    mouseOverNode = true;
                    break;
                }
            }

            if (!mouseOverNode && !mouseOverInput) {
                mSelectedIndex = -1;
                mInputFocused = false;
            }
            
            if (mouseOverInput) {
                mInputFocused = true;
            }
        }
    
        if (mInsertButton.isClicked(mouse, true)) {
            runInsert();
        }
        if (mDeleteButton.isClicked(mouse, true)) {
            runDeleteSelected();
        }
        if (mBuildButton.isClicked(mouse, true)) {
            runBuildHeap();
        }
        if (mClearButton.isClicked(mouse, true)) {
            runClear();
        }
        if (mUpdateButton.isClicked(mouse, true)) {
            runUpdate();
        }
        if (mLoadButton.isClicked(mouse, true)) {
            setStatus("Load from file is not implemented yet.");
        }
        if (mRandomButton.isClicked(mouse, true)) {
            runRandom();
        }
        if (mSkipButton.isClicked(mouse, true)) {
            runSkip();
        }
        if (mPlayPauseButton.isClicked(mouse, true)) {
            togglePlayback();
        }
        if (mLeftCollapseBtn.isClicked(mouse, true)) {
            mLeftExpanded = !mLeftExpanded;
        }
        if (mRightCollapseBtn.isClicked(mouse, true)) {
            mRightExpanded = !mRightExpanded;
        }
        if (mStepButton.isClicked(mouse, true)) {
            mIsPlaying = false;
            if (!mPendingActions.empty()) {
                processNextAction();
            } else {
                setStatus("No pending animation steps.");
            }
        }
        if (mPrevButton.isClicked(mouse, true)) {
            mIsPlaying = false;
            if (!mHistory.empty()) {
                processPreviousAction();
                setStatus("Stepped back.");
            } else {
                setStatus("Already at the start of operation.");
            }
        }
    }

    if (event.type == sf::Event::TextEntered && mInputFocused) {
        const auto unicode = event.text.unicode;
        if (unicode == 8) {
            backspaceInput();
        } 
        else if (unicode < 128) {
            appendCharacter(static_cast<char>(unicode));
        }
    }
}

// Refreshes hover states, visible text, and advances the animation timer when autoplay is enabled.
void HeapVisualizer::update(float deltaTime, const sf::RenderWindow& window) {
    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    float targetLeft = mLeftExpanded ? SIDEBAR_MAX_WIDTH : TAB_WIDTH;
    float targetRight = mRightExpanded ? CODE_PANEL_MAX_WIDTH : TAB_WIDTH;

    mLeftWidth += (targetLeft - mLeftWidth) * 12.f * deltaTime;
    mRightWidth += (targetRight - mRightWidth) * 12.f * deltaTime;

    mWorkspaceCenterX = mLeftWidth + (1280.f - mLeftWidth - mRightWidth) / 2.f;

    float cpWidth = 204.f;
    mCodePanel.setPosition({ 1280.f - mRightWidth + (mRightWidth - cpWidth) / 2.f, 24.f });

    mLeftCollapseBtn.setPosition({ mLeftWidth - 15.f, 360.f });
    mRightCollapseBtn.setPosition({ 1280.f - mRightWidth + 15.f, 360.f });
    mLeftCollapseBtn.setText(mLeftExpanded ? "<<" : ">>");
    mRightCollapseBtn.setText(mRightExpanded ? ">>" : "<<");

    mLeftCollapseBtn.update(mouse);
    mRightCollapseBtn.update(mouse);

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
    }

    if (mInputFocused) {
        mInputBox.setOutlineColor(sf::Color(181, 58, 199));
        mInputBox.setOutlineThickness(3.f);
    } else {
        mInputBox.setOutlineColor(sf::Color(100, 100, 100));
        mInputBox.setOutlineThickness(1.f);
    }

    if (!mIsPlaying || mPendingActions.empty()) {
        return;
    }

    mActionTimer += deltaTime;
    if (mActionTimer >= mActionInterval) {
        mActionTimer = 0.f;
        processNextAction();
    }
}

// Draws the heap screen in layers so the panel, controls, and visualization stay separated.
void HeapVisualizer::render(sf::RenderWindow& window) const {
    window.draw(mBgSprite);

    sf::RectangleShape sidebarBg({mLeftWidth, 720.f});
    sidebarBg.setFillColor(sf::Color(25, 25, 35));
    window.draw(sidebarBg);

    sf::RectangleShape codePanelBg({mRightWidth, 720.f});
    codePanelBg.setPosition(1280.f - mRightWidth, 0.f);
    codePanelBg.setFillColor(sf::Color(25, 25, 35));
    window.draw(codePanelBg);

    if (mLeftWidth > 180.f) {
        drawInputArea(window);
        drawButtons(window);
        drawLegend(window);
        window.draw(mStatusText);
    }

    if (mRightWidth > 180.f) {
        const_cast<CodePanel&>(mCodePanel).draw(window);
        drawColorPicker(window);
    }

    drawTree(window);
    drawArray(window);
    window.draw(mLeftCollapseBtn);
    window.draw(mRightCollapseBtn);
}

// Clears temporary UI state when the user leaves and re-enters the heap screen.
void HeapVisualizer::reset() {
    mInputFocused = false;
    mPendingActions.clear();
    mHighlight = {};
    mActionTimer = 0.f;
}

// Inserts a single value into the current heap and converts the heap's action log into animation steps.
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

// Removes the root element, which matches the main delete operation exposed by the visualizer UI.
void HeapVisualizer::runDeleteSelected() {
    if (mSelectedIndex == -1) {
        setStatus("Please select a node to delete.");
        return;
    }

    const std::vector<int> startArray = mPendingActions.empty() ? mDisplayArray : mHeap.getArray();
    if (startArray.empty()) {
        setStatus("Heap is empty.");
        return;
    }

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

// Builds a heap from a typed sequence and keeps the original order on screen so heapify can animate into place.
void HeapVisualizer::runBuildHeap() {
    bool ok = false;
    std::vector<int> values = parseSequence(ok);
    if (!ok) {
        setStatus("Build Heap expects integers separated by spaces or commas.");
        return;
    }
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
    for (const Action& action : actions) {
        mPendingActions.push_back(action);
    }
}

// Resets both the heap model and the visual state to an empty screen.
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
    if (mSelectedIndex == -1) {
        setStatus("Please select a node first.");
        return;
    }

    if (mInputBuffer.empty()) {
        setStatus("Enter a new value in the input bar.");
        return;
    }

    int newValue;
    if (tryParseSingleValue(newValue)) {
        const std::vector<int> startArray = mDisplayArray;
        mHeap.BuildHeap(startArray);
        mHeap.flushActions();

        mHeap.Update(mSelectedIndex, newValue);
        queueOperation(startArray);

        if (mPendingActions.empty()) {
            mIsPlaying = false;
        }

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
    for (int i = 0; i < n; ++i) {
        randomValues.push_back(std::rand() % 100);
    }

    mDisplayArray = randomValues; 
    mHeap.BuildHeap(randomValues);
    queueOperation(randomValues); 
    
    mIsPlaying = true;
    loadHeapifyCode();
    setStatus("Randomized " + std::to_string(n) + " nodes. Starting Build...");
}

void HeapVisualizer::runSkip() {
    if (mPendingActions.empty()) {
        setStatus("Nothing to skip.");
        return;
    }

    while (!mPendingActions.empty()) {
        processNextAction();
    }

    setStatus("Skipped to final state.");
}

// Switches between automatic playback and manual stepping.
void HeapVisualizer::togglePlayback() {
    mIsPlaying = !mIsPlaying;
    setStatus(mIsPlaying ? "Animation resumed." : "Animation paused.");
}

// Starts a new animation by resetting the displayed heap to its pre-operation state.
void HeapVisualizer::queueOperation(const std::vector<int>& startArray) {
    mDisplayArray = startArray;
    mPendingActions.clear();
    mHistory.clear();
    mHighlight = {};
    mActionTimer = 0.f;

    // The heap model records actions while mutating; the UI replays that log against mDisplayArray.
    const std::vector<Action> actions = mHeap.flushActions();
    for (const Action& action : actions) {
        mPendingActions.push_back(action);
    }

    if (mPendingActions.empty()) {
        mDisplayArray = mHeap.getArray();
    } else if (!mIsPlaying) {
        processNextAction();
    }
}

// Applies one recorded heap action to the currently displayed array and updates highlight colors.
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

// Draws the full-screen translucent backdrop and the status texts pinned to it.
void HeapVisualizer::drawPanel(sf::RenderWindow& window) const {
    window.draw(mPanel);
    window.draw(mCodeBox);
    window.draw(mControlPanelBg);
    window.draw(mStatusText);
}

// Draws the input label, box, current text, and usage hint.
void HeapVisualizer::drawInputArea(sf::RenderWindow& window) const {
    window.draw(mInputBox);
    if (mInputBuffer.empty()) {
        window.draw(mPlaceholderText);
    } else {
        window.draw(mInputText);
    }
    window.draw(mHintText);
}

// Draws the button cluster used to control heap operations and animation playback.
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

// Draws the array representation under the tree, shrinking cells when many nodes are present.
void HeapVisualizer::drawArray(sf::RenderWindow& window) const {
    const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
    if (visibleNodes == 0) return;

    const float windowWidth = 1280.f; 
    const float availableWorkspaceWidth = (windowWidth - mLeftWidth - mRightWidth) * 0.95f;
    const float gap = 4.f;

    const float cellWidth = std::min(48.f, (availableWorkspaceWidth - (visibleNodes * gap)) / visibleNodes);
    const float totalArrayWidth = (visibleNodes * cellWidth) + ((visibleNodes - 1) * gap);

    const float startX = mWorkspaceCenterX - (totalArrayWidth / 2.f);

    for (std::size_t i = 0; i < visibleNodes; ++i) {
        float currentX = startX + i * (cellWidth + gap);
        
        sf::RectangleShape cell({cellWidth, cellWidth});
        cell.setPosition({currentX, ARRAY_Y});
        cell.setFillColor(mCurrentNodeColor);
        cell.setOutlineThickness(2.f);

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

        sf::Text indexText = makeText(mFont, std::to_string(i), 12, sf::Color(189, 198, 214), {0.f, 0.f});
        sf::FloatRect idxBounds = indexText.getLocalBounds();
        indexText.setOrigin(idxBounds.left + idxBounds.width / 2.f, 0.f);
        indexText.setPosition({currentX + cellWidth / 2.f, ARRAY_Y - 18.f});
        window.draw(indexText);
    }
}

// Draws the node-link tree representation using heap indices to derive parent-child edges.
void HeapVisualizer::drawTree(sf::RenderWindow& window) const {
    const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
    for (std::size_t i = 1; i < visibleNodes; ++i) {
        const std::size_t parent = (i - 1) / 2;
        const sf::Vector2f start = nodePosition(parent);
        const sf::Vector2f end = nodePosition(i);
        sf::Vertex line[] = {
            sf::Vertex(start, sf::Color(136, 155, 184)),
            sf::Vertex(end, sf::Color(136, 155, 184))
        };
        window.draw(line, 2, sf::Lines);
    }

    for (std::size_t i = 0; i < visibleNodes; ++i) {
        float Radius = (i >= 15) ? NODE_RADIUS - 5.0f : NODE_RADIUS;
        sf::CircleShape node(Radius);
        node.setOrigin({Radius, Radius});
        node.setPosition(nodePosition(i));
        node.setFillColor(mCurrentNodeColor);
        node.setOutlineThickness(4.f);

        sf::Color currentOutlineColor = nodeColor(i);
        if (mSelectedIndex == static_cast<int>(i)) {
            currentOutlineColor = sf::Color(34, 139, 34);
            node.setOutlineThickness(6.f);
        }
        node.setOutlineColor(currentOutlineColor);

        window.draw(node);

        sf::Text valueText = makeText(mFont, std::to_string(mDisplayArray[i]), 18, sf::Color(20, 28, 40), {0.f, 0.f});
        sf::FloatRect bounds = valueText.getLocalBounds();
        valueText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        valueText.setPosition({node.getPosition().x, node.getPosition().y - 1.f});
        window.draw(valueText);
    }
}

// Draws the animation status and the color legend for compare, swap, and focus states.
void HeapVisualizer::drawLegend(sf::RenderWindow& window) const {
    // Step
    constexpr float LEGEND_Y = 570.f;
    if (!mHighlight.label.empty()) {
        sf::Text stepText = makeText(mFont, "Step: " + mHighlight.label, 16, sf::Color(251, 209, 101), {40.f, LEGEND_Y});
        window.draw(stepText);
    }

    float itemX = 600.f; 

    // Compare
    sf::CircleShape compare(8.f);
    compare.setFillColor(sf::Color(0, 255, 255));
    compare.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(compare);
    window.draw(makeText(mFont, "Compare", 15, sf::Color::White, {itemX + 25.f, LEGEND_Y}));

    // Swap
    itemX += 120.f;
    sf::CircleShape swap(8.f);
    swap.setFillColor(sf::Color(255, 124, 124));
    swap.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(swap);
    window.draw(makeText(mFont, "Swap", 15, sf::Color::White, {itemX + 25.f, LEGEND_Y}));

    // Focused
    itemX += 90.f;
    sf::CircleShape focus(8.f);
    focus.setFillColor(sf::Color(248, 196, 76));
    focus.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(focus);
    window.draw(makeText(mFont, "Focused", 15, sf::Color::White, {itemX + 25.f, LEGEND_Y}));

    //Selected
    itemX += 110.f;
    sf::CircleShape selected(8.f);
    selected.setFillColor(sf::Color(34, 139, 34));
    selected.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(selected);
    window.draw(makeText(mFont, "Selected", 15, sf::Color::White, {itemX + 25.f, LEGEND_Y}));
}

void HeapVisualizer::drawCodeSnippet(sf::RenderWindow& window) const {
    if (mCurrentCode.empty()) return;

    float boxX = 1048.f; 
    float boxWidth = 224.f; 

    sf::RectangleShape codeBg({boxWidth, 280.f});
    codeBg.setPosition({boxX, 100.f});
    codeBg.setFillColor(sf::Color(15, 15, 25, 230)); 
    codeBg.setOutlineThickness(1.2f);
    codeBg.setOutlineColor(sf::Color(100, 100, 255, 150));
    window.draw(codeBg);

    for (int i = 0; i < (int)mCurrentCode.size(); ++i) {
        sf::Text text = makeText(mFont, mCurrentCode[i], 12, sf::Color(190, 190, 200), {boxX + 15.f, 120.f + i * 22.f});
        
        if (i == mActiveLine) {
            text.setFillColor(sf::Color(255, 105, 180));
            
            sf::RectangleShape bar({3.f, 16.f});
            bar.setPosition({boxX + 5.f, 124.f + i * 22.f});
            bar.setFillColor(sf::Color(255, 105, 180));
            window.draw(bar);
        }
        window.draw(text);
    }
}

void HeapVisualizer::drawColorPicker(sf::RenderWindow& window) const {
    float rightBaseX = 1280.f - mRightWidth;
    float startX = rightBaseX + 50.f;
    float startY = 380.f;

    sf::Text label = makeText(mFont, "Node Fill Color", 14, sf::Color(200, 200, 210), {startX, startY});
    window.draw(label);

    for (size_t i = 0; i < mColorSwatches.size(); ++i) {
        float x = startX + (i % 3) * 35.f;
        float y = startY + 25.f + (i / 3) * 35.f;
        
        auto& swatch = const_cast<sf::RectangleShape&>(mColorSwatches[i]);
        swatch.setPosition(x, y);

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

// Adds a character to the input buffer while keeping the field length bounded.
void HeapVisualizer::appendDigit(char digit) {
    if (mInputBuffer.size() < 60) {
        mInputBuffer.push_back(digit);
    }
}

// Only accepts characters that make sense for integer and integer-list input.
void HeapVisualizer::appendCharacter(char character) {
    if ((character >= '0' && character <= '9') || character == '-' || character == ',' || character == ' ') {
        appendDigit(character);
    }
}

// Removes the last typed character from the input field.
void HeapVisualizer::backspaceInput() {
    if (!mInputBuffer.empty()) {
        mInputBuffer.pop_back();
        mInputText.setString(mInputBuffer);
    }
}

// Updates the status line and rebuilds the play/pause button label to match the current mode.
void HeapVisualizer::setStatus(const std::string& status) {
    mStatusMessage = status;
    mStatusText.setString(status);
    float row5Y = BUTTON_START_Y + 5 * (BUTTON_HEIGHT + BUTTON_GAP_Y);
    float playCenterX = BUTTON_X + BUTTON_WIDTH + BUTTON_GAP_X / 2.f;
    mPlayPauseButton = ModernButton(mIsPlaying ? "Pause" : "Play", mFont, {BUTTON_WIDTH, BUTTON_HEIGHT});
    mPlayPauseButton.setPosition({playCenterX, row5Y + BUTTON_HEIGHT / 2.f});
}

// Parses the input as exactly one integer, used by the Insert action.
bool HeapVisualizer::tryParseSingleValue(int& value) const {
    std::stringstream stream(mInputBuffer);
    stream >> value;
    return !stream.fail() && stream.eof();
}

// Parses a list of integers for heap construction, accepting both spaces and commas as separators.
std::vector<int> HeapVisualizer::parseSequence(bool& ok) const {
    std::vector<int> values;
    ok = true;

    std::string normalized = mInputBuffer;
    std::replace(normalized.begin(), normalized.end(), ',', ' ');

    std::stringstream stream(normalized);
    int value = 0;
    while (stream >> value) {
        values.push_back(value);
    }

    if (!stream.eof()) {
        ok = false;
    }

    return values;
}

// Converts an array index into a tree position by grouping nodes by heap level.
sf::Vector2f HeapVisualizer::nodePosition(std::size_t index) const {
    const int level = static_cast<int>(std::floor(std::log2(static_cast<float>(index + 1))));
    const std::size_t nodesInLevel = 1u << level;

    float availableWidth = (1280.f - mLeftWidth - mRightWidth) - 60.f;
    const float horizontalGap = availableWidth / static_cast<float>(nodesInLevel);

    const std::size_t firstIndexInLevel = (1u << level) - 1u;
    const std::size_t positionInLevel = index - firstIndexInLevel;

    float levelWidth = horizontalGap * nodesInLevel;
    float xStartOfLevel = mWorkspaceCenterX - (levelWidth / 2.f);

    const float x = xStartOfLevel + horizontalGap * (static_cast<float>(positionInLevel) + 0.5f);
    const float y = TREE_TOP_Y + level * 80.f;

    return {x, y};
}

// Resolves the outline color for a node based on the active animation highlight state.
sf::Color HeapVisualizer::nodeColor(std::size_t index) const {
    if (static_cast<int>(index) == mHighlight.first) {
        return mHighlight.firstColor;
    }
    if (static_cast<int>(index) == mHighlight.second) {
        return mHighlight.secondColor;
    }
    return sf::Color(106, 133, 176);
}

void HeapVisualizer::clearInput() {
    mInputBuffer.clear(); 
    mInputText.setString(""); 
}

int HeapVisualizer::run(sf::RenderWindow& window, sf::Font& font) {
    sf::View originalView = window.getView(); 
    sf::View heapView;
    heapView.setSize(1280.f, 720.f); 
    heapView.setCenter(640.f, 360.f);
    window.setView(heapView);

    if (!mBgTexture.loadFromFile("assets/textures/avl_background.png")) {
        std::cerr << "Failed to load background.png\n";
    }
    mBgSprite.setTexture(mBgTexture);
    mBgSprite.setScale(1280.f / mBgTexture.getSize().x, 720.f / mBgTexture.getSize().y);

    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }

            const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (event.type == sf::Event::MouseButtonReleased &&
                event.mouseButton.button == sf::Mouse::Left &&
                mReturnButton.isClicked(mouse, true)) {
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
        window.draw(mBgSprite);
        render(window); 
        window.display();
    }

    window.setView(originalView);
    return -1;
}