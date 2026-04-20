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
    constexpr float BUTTON_WIDTH = 92.f;
    constexpr float BUTTON_HEIGHT = 46.f;
    constexpr float BUTTON_X = 24.f;
    constexpr float BUTTON_GAP_Y = 14.f;
    constexpr float BUTTON_GAP_X = 12.f;
    constexpr float BUTTON_START_Y = 108.f; 
    constexpr float BUTTON_X2 = BUTTON_X + BUTTON_WIDTH + BUTTON_GAP_X;

    // Input
    constexpr float INPUT_X = 24.f;
    constexpr float INPUT_Y = 24.f;
    constexpr float INPUT_WIDTH = 176.f;
    constexpr float INPUT_HEIGHT = 44.f;

    // Tree + Array
    constexpr std::size_t MAX_RENDERED_NODES = 16;
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
    , mSkipButton("Skip", font, {BUTTON_WIDTH, BUTTON_HEIGHT})
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
    float sliderY = BUTTON_START_Y + 5 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + 20.f;
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

    // ModernButton uses centered transforms, so convert the existing top-left layout into center positions.
    mInsertButton.setPosition({BUTTON_X + BUTTON_WIDTH / 2.f, BUTTON_START_Y + BUTTON_HEIGHT / 2.f});
    mDeleteButton.setPosition({BUTTON_X2 + BUTTON_WIDTH / 2.f, BUTTON_START_Y + BUTTON_HEIGHT / 2.f});
    mBuildButton.setPosition({BUTTON_X + BUTTON_WIDTH / 2.f, BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mClearButton.setPosition({BUTTON_X2 + BUTTON_WIDTH / 2.f, BUTTON_START_Y + (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mRandomButton.setPosition({BUTTON_X + BUTTON_WIDTH / 2.f, BUTTON_START_Y + 2 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mSkipButton.setPosition({BUTTON_X2 + BUTTON_WIDTH / 2.f, BUTTON_START_Y + 2 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mLoadButton.setPosition({BUTTON_X + (BUTTON_WIDTH * 2 + BUTTON_GAP_X) / 2.f, BUTTON_START_Y + 3 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mPrevButton.setPosition({BUTTON_X + (BUTTON_WIDTH / 4.f), BUTTON_START_Y + 4 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mPlayPauseButton.setPosition({BUTTON_X + BUTTON_WIDTH + BUTTON_GAP_X / 2.f, BUTTON_START_Y + 4 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mStepButton.setPosition({BUTTON_X + 1.75f * BUTTON_WIDTH + BUTTON_GAP_X, BUTTON_START_Y + 4 * (BUTTON_HEIGHT + BUTTON_GAP_Y) + BUTTON_HEIGHT / 2.f});
    mReturnButton.setPosition({BUTTON_X + (BUTTON_WIDTH * 2 + BUTTON_GAP_X) / 2.f, 626.f + BUTTON_HEIGHT / 2.f});
    
    setStatus("Ready.");
}

// Routes mouse and keyboard input to the correct heap action or text field update.
void HeapVisualizer::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
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
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            mIsDraggingSlider = false;
            mInputFocused = mInputBox.getGlobalBounds().contains(mouse);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        mInsertButton.update(mouse);
        mDeleteButton.update(mouse);
        mBuildButton.update(mouse);
        mClearButton.update(mouse);
        mPlayPauseButton.update(mouse);
        mStepButton.update(mouse);
        mPrevButton.update(mouse);
        mLoadButton.update(mouse);

        if (mInsertButton.isClicked(mouse, true)) {
            runInsert();
        }
        if (mDeleteButton.isClicked(mouse, true)) {
            runDeleteRoot();
        }
        if (mBuildButton.isClicked(mouse, true)) {
            runBuildHeap();
        }
        if (mClearButton.isClicked(mouse, true)) {
        if (mLoadButton.isClicked(mouse, true)) {
            setStatus("Load from file is not implemented yet.");
        }
            runClear();
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
        } else if (unicode == 13) {
            int value = 0;
            if (tryParseSingleValue(value)) {
                runInsert();
            } else {
                runBuildHeap();
            }
        } else if (unicode < 128) {
            appendCharacter(static_cast<char>(unicode));
        }
    }
}

// Refreshes hover states, visible text, and advances the animation timer when autoplay is enabled.
void HeapVisualizer::update(float deltaTime, const sf::RenderWindow& window) {
    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    mInsertButton.update(mouse);
    mDeleteButton.update(mouse);
    mBuildButton.update(mouse);
    mClearButton.update(mouse);
    mPlayPauseButton.update(mouse);
    mStepButton.update(mouse);
    mPrevButton.update(mouse);
    mLoadButton.update(mouse);
    mReturnButton.update(mouse);
    mRandomButton.update(mouse);
    mSkipButton.update(mouse);

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
    drawPanel(window);
    drawInputArea(window);
    drawButtons(window);
    drawTree(window);
    drawArray(window);
    drawLegend(window);
    drawCodeSnippet(window);
    const_cast<CodePanel&>(mCodePanel).draw(window);
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
    if (startArray.size() >= 15) {
        setStatus("Limit reached. Clear or use fewer than 16 nodes.");
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
void HeapVisualizer::runDeleteRoot() {
    const std::vector<int> startArray = mPendingActions.empty() ? mDisplayArray : mHeap.getArray();
    if (startArray.empty()) {
        setStatus("Heap is empty.");
        return;
    }
    loadHeapifyCode();
    const int rootValue = startArray.front();
    mHeap.BuildHeap(startArray);
    mHeap.flushActions();
    mHeap.Delete(0);
    queueOperation(startArray);
    setStatus("Deleted root " + std::to_string(rootValue) + ".");
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

void HeapVisualizer::runRandom() {
    clearInput();
    mPendingActions.clear();

    int n = std::rand() % 6 + 10; 
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

    mIsPlaying = false; 
    mActionTimer = 0.f;
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
        mHighlight.firstColor = sf::Color(87, 190, 255);
        mHighlight.secondColor = sf::Color(87, 190, 255);
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

    const float maxTotalWidth = 760.f;
    const float gap = 4.f;
    const float cellWidth = std::min(48.f, (maxTotalWidth - (visibleNodes * gap)) / visibleNodes);
    
    for (std::size_t i = 0; i < visibleNodes; ++i) {
        sf::RectangleShape cell({cellWidth, cellWidth});
        cell.setPosition({260.f + i * (cellWidth + gap), ARRAY_Y});
        cell.setFillColor(sf::Color(247, 250, 255, 230));
        cell.setOutlineThickness(2.f);
        cell.setOutlineColor(nodeColor(i));
        window.draw(cell);

        unsigned int fontSize = cellWidth < 35.f ? 12 : 18;
        sf::Text valueText = makeText(mFont, std::to_string(mDisplayArray[i]), fontSize, sf::Color(20, 28, 40), {0.f, 0.f});
        sf::FloatRect bounds = valueText.getLocalBounds();
        valueText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        valueText.setPosition({cell.getPosition().x + cellWidth / 2.f, cell.getPosition().y + cellWidth / 2.f - 2.f});
        window.draw(valueText);

        sf::Text indexText = makeText(mFont, std::to_string(i), 12, sf::Color(189, 198, 214), {0.f, 0.f});
        sf::FloatRect idxBounds = indexText.getLocalBounds();
        indexText.setOrigin(idxBounds.left + idxBounds.width / 2.f, 0.f);
        // Đặt ở vị trí ARRAY_Y - 15.f (phía trên ô)
        indexText.setPosition({cell.getPosition().x + cellWidth / 2.f, cell.getPosition().y - 18.f});
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
        sf::CircleShape node(NODE_RADIUS);
        node.setOrigin({NODE_RADIUS, NODE_RADIUS});
        node.setPosition(nodePosition(i));
        node.setFillColor(sf::Color(245, 249, 255));
        node.setOutlineThickness(4.f);
        node.setOutlineColor(nodeColor(i));
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
    compare.setFillColor(sf::Color(87, 190, 255));
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
    itemX += 100.f;
    sf::CircleShape focus(8.f);
    focus.setFillColor(sf::Color(248, 196, 76));
    focus.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(focus);
    window.draw(makeText(mFont, "Focused", 15, sf::Color::White, {itemX + 25.f, LEGEND_Y}));
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
    float row4Y = BUTTON_START_Y + 4 * (BUTTON_HEIGHT + BUTTON_GAP_Y);
    float playCenterX = BUTTON_X + BUTTON_WIDTH + BUTTON_GAP_X / 2.f;
    mPlayPauseButton = ModernButton(mIsPlaying ? "Pause" : "Play", mFont, {BUTTON_WIDTH, BUTTON_HEIGHT});
    mPlayPauseButton.setPosition({playCenterX, row4Y + BUTTON_HEIGHT / 2.f});
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
    const std::size_t firstIndexInLevel = (1u << level) - 1u;
    const std::size_t positionInLevel = index - firstIndexInLevel;
    const std::size_t nodesInLevel = 1u << level;
    const float horizontalGap = TREE_WIDTH / static_cast<float>(nodesInLevel);
    const float x = TREE_LEFT_X + horizontalGap * (static_cast<float>(positionInLevel) + 0.5f);
    const float y = TREE_TOP_Y + level * 60.f;
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
    // 1. Load texture
    if (!mBgTexture.loadFromFile("assets/textures/avl_background.png")) {
        std::cerr << "Failed to load background.png\n";
    }
    mBgSprite.setTexture(mBgTexture);
    
    mBgSprite.setScale(1280.f / mBgTexture.getSize().x, 720.f / mBgTexture.getSize().y);

    sf::Clock clock;

    // 2. Event Loop
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }

            const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            mReturnButton.update(mouse);
            if (event.type == sf::Event::MouseButtonReleased &&
                event.mouseButton.button == sf::Mouse::Left &&
                mReturnButton.isClicked(mouse, true)) {
                return 0;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                return 0;
            }

            handleEvent(event, window);
        }

        // 3. Update logic (animation, hover effect...)
        update(deltaTime, window);

        // 4. Render
        window.clear();
        
        window.draw(mBgSprite); // background
        render(window);         // render tree, array, buttons...
        
        window.display();
    }

    return -1;
}

