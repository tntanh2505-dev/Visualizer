#include "DSA-Visualization/ui/heap_screen.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace {
    // --- Dynamic Sliding Panel Constants ---
    const float LEFT_PANEL_WIDTH = 280.f;
    const float RIGHT_PANEL_WIDTH = 380.f;
    const float TAB_WIDTH = 35.f;
    const float TAB_HEIGHT = 50.f;

    // --- Dynamic Layout Variables ---
    float g_leftWidth = TAB_WIDTH;
    float g_rightWidth = TAB_WIDTH;
    bool g_leftExpanded = false;
    bool g_rightExpanded = false;
    
    float g_TreeWidth = 900.f;
    float g_TreeLeftX = 40.f;
    
    const float ARRAY_Y = 60.f;     // Leave 3 lines at the top
    const float TREE_TOP_Y = 180.f; // Tree drops below the array

    constexpr float BUTTON_WIDTH = 100.f;
    constexpr float BUTTON_HEIGHT = 40.f;
    constexpr float INPUT_WIDTH = 220.f;
    constexpr float INPUT_HEIGHT = 40.f;

    constexpr std::size_t MAX_RENDERED_NODES = 31;
    const float NODE_RADIUS = UITheme::Size::NodeRadius;

    constexpr float MIN_INTERVAL = 0.1f;
    constexpr float MAX_INTERVAL = 2.0f;

    // Helper
    sf::Text makeText(const sf::Font& font, const std::string& value, unsigned int size, sf::Color color, sf::Vector2f position) {
        sf::Text text; text.setFont(font); text.setString(value); text.setCharacterSize(size); text.setFillColor(color); text.setPosition(position); return text;
    }
}

HeapVisualizer::HeapVisualizer(const sf::Font& font)
    : mFont(font)
    , mSpeedLabel(makeText(font, "Speed: 0.60s", 14, UITheme::Color::TextWhite, {0, 0}))
    , mPlaceholderText(makeText(font, "Enter value...", 14, UITheme::Color::TextMuted, {0, 0}))
    , mInputText(makeText(font, "", 16, UITheme::Color::TextDark, {0, 0}))
    , mHintText(makeText(font, "Format: 1, 2, 3...", 12, UITheme::Color::TextMuted, {0, 0}))
    , mStatusText(makeText(font, "", 18, UITheme::Color::HeapTextHighlight, {0, 0}))

    // Buttons will be dynamically positioned in update()
    , mInsertButton("Insert", font, {0, 0}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mDeleteButton("Delete", font, {0, 0}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mBuildButton("Build", font, {0, 0}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mClearButton("Clear", font, {0, 0}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mPrevButton("<", font, {0, 0}, {BUTTON_WIDTH / 2.f, BUTTON_HEIGHT})
    , mPlayPauseButton("Pause", font, {0, 0}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mStepButton(">", font, {0, 0}, {BUTTON_WIDTH / 2.f, BUTTON_HEIGHT})
    , mReturnButton("Return", font, {0, 0}, {BUTTON_WIDTH * 2 + 10.f, BUTTON_HEIGHT})
{
    g_leftExpanded = false;
    g_rightExpanded = false;
    g_leftWidth = TAB_WIDTH;
    g_rightWidth = TAB_WIDTH;

    mPanel.setFillColor(UITheme::Color::HeapBackground);
    
    // Setup Code Panel
    mCodePanel = CodePanel(font, {0, 0}, {340.f, 280.f});
    loadHeapifyCode();

    // Speed Slider setup
    mSliderTrack.setSize({220.f, 6.f});
    mSliderTrack.setFillColor(UITheme::Color::SliderTrack);
    mSliderTrack.setOutlineThickness(1.f);
    mSliderKnob.setRadius(10.f);
    mSliderKnob.setFillColor(UITheme::Color::HeapAccent);
    mSliderKnob.setOrigin(10.f, 10.f);
    mIsDraggingSlider = false;

    // Input area setup
    mInputBox.setSize({INPUT_WIDTH, INPUT_HEIGHT});
    mInputBox.setFillColor(UITheme::Color::TextBoxBg);
    mInputBox.setOutlineThickness(2.f);
    mInputBox.setOutlineColor(UITheme::Color::HeapAccent);

    setStatus("Ready.");
}

void HeapVisualizer::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // --- Tab Slide Interaction ---
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        float centerY = window.getSize().y / 2.f;
        float winW = window.getSize().x;
        
        bool mouseInLeftTab = (mouse.x >= g_leftWidth - TAB_WIDTH && mouse.x <= g_leftWidth &&
                               mouse.y >= centerY - TAB_HEIGHT / 2.f && mouse.y <= centerY + TAB_HEIGHT / 2.f);
        if (mouseInLeftTab) { g_leftExpanded = !g_leftExpanded; return; }

        bool mouseInRightTab = (mouse.x >= winW - g_rightWidth && mouse.x <= winW - g_rightWidth + TAB_WIDTH &&
                                mouse.y >= centerY - TAB_HEIGHT / 2.f && mouse.y <= centerY + TAB_HEIGHT / 2.f);
        if (mouseInRightTab) { g_rightExpanded = !g_rightExpanded; return; }
    }

    bool isClickingOnPanel = (mouse.x < g_leftWidth) || (mouse.x > window.getSize().x - g_rightWidth);

    if (isClickingOnPanel && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::FloatRect knobBounds = mSliderKnob.getGlobalBounds();
        knobBounds.left -= 10.f; knobBounds.width += 20.f; knobBounds.top -= 10.f; knobBounds.height += 20.f;

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

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        mIsDraggingSlider = false;
        mInputFocused = mInputBox.getGlobalBounds().contains(mouse);
    }

    if (mInsertButton.isClicked(event, window)) runInsert();
    if (mDeleteButton.isClicked(event, window)) runDeleteRoot();
    if (mBuildButton.isClicked(event, window)) runBuildHeap();
    if (mClearButton.isClicked(event, window)) runClear();
    if (mPlayPauseButton.isClicked(event, window)) togglePlayback();
    
    if (mStepButton.isClicked(event, window)) {
        mIsPlaying = false;
        if (!mPendingActions.empty()) processNextAction();
        else setStatus("No pending animation steps.");
    }
    
    if (mPrevButton.isClicked(event, window)) {
        mIsPlaying = false;
        if (!mHistory.empty()) {
            processPreviousAction();
            setStatus("Stepped back."); 
        } else {
            setStatus("Already at the start of operation.");
        }
    }

    if (event.type == sf::Event::TextEntered && mInputFocused) {
        const auto unicode = event.text.unicode;
        if (unicode == 8) backspaceInput();
        else if (unicode == 13) {
            int value = 0;
            if (tryParseSingleValue(value)) runInsert();
            else runBuildHeap();
        } else if (unicode < 128) {
            appendCharacter(static_cast<char>(unicode));
        }
    }
}

void HeapVisualizer::update(float deltaTime, const sf::RenderWindow& window) {
    // --- Smooth Panel Transitions ---
    float targetLeft = g_leftExpanded ? LEFT_PANEL_WIDTH : TAB_WIDTH;
    g_leftWidth += (targetLeft - g_leftWidth) * 12.f * deltaTime;
    
    float targetRight = g_rightExpanded ? RIGHT_PANEL_WIDTH : TAB_WIDTH;
    g_rightWidth += (targetRight - g_rightWidth) * 12.f * deltaTime;

    // --- Left Panel Layout ---
    float leftBaseX = g_leftWidth - LEFT_PANEL_WIDTH; 
    float currentY = 40.f;
    
    mInputBox.setPosition({leftBaseX + 20.f, currentY});
    mPlaceholderText.setPosition({leftBaseX + 26.f, currentY + 10.f});
    mInputText.setPosition({leftBaseX + 26.f, currentY + 8.f});
    mHintText.setPosition({leftBaseX + 20.f, currentY + 45.f});

    currentY += 80.f;
    mInsertButton.setPosition({leftBaseX + 20.f, currentY});
    mDeleteButton.setPosition({leftBaseX + 130.f, currentY});

    currentY += 60.f;
    mBuildButton.setPosition({leftBaseX + 20.f, currentY});
    mClearButton.setPosition({leftBaseX + 130.f, currentY});

    currentY += 60.f;
    mPrevButton.setPosition({leftBaseX + 20.f, currentY});
    mPlayPauseButton.setPosition({leftBaseX + 75.f, currentY});
    mStepButton.setPosition({leftBaseX + 180.f, currentY}); 

    currentY += 70.f;
    mSpeedLabel.setPosition({leftBaseX + 20.f, currentY - 20.f});
    mSliderTrack.setPosition({leftBaseX + 20.f, currentY});
    mPanel.setSize({(float)window.getSize().x, (float)window.getSize().y});

    float t = 1.0f - (mActionInterval - MIN_INTERVAL) / (MAX_INTERVAL - MIN_INTERVAL);
    if (!mIsDraggingSlider) {
        mSliderKnob.setPosition({leftBaseX + 20.f + t * mSliderTrack.getSize().x, currentY + 3.f});
    } else {
        const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        const float left = mSliderTrack.getPosition().x;
        const float width = mSliderTrack.getSize().x;
        const float newX = std::max(left, std::min(mouse.x, left + width));
        mSliderKnob.setPosition(newX, currentY + 3.f);

        const float newT = (newX - left) / width;
        mActionInterval = MAX_INTERVAL - newT * (MAX_INTERVAL - MIN_INTERVAL);
        std::stringstream ss;
        ss << "Speed: " << std::fixed << std::setprecision(2) << mActionInterval << "s";
        mSpeedLabel.setString(ss.str());
    }

    currentY += 60.f;
    mReturnButton.setPosition({leftBaseX + 20.f, currentY});

    // --- Right Panel Layout ---
    float rightBaseX = window.getSize().x - g_rightWidth;
    mCodePanel.setPosition({rightBaseX + TAB_WIDTH + 10.f, 20.f});

    // --- Dynamic Resizing for Tree and Array ---
    g_TreeLeftX = g_leftWidth + 20.f;
    g_TreeWidth = window.getSize().x - g_leftWidth - g_rightWidth - 40.f;
    mStatusText.setPosition({g_TreeLeftX + (g_TreeWidth / 2.f) - 100.f, window.getSize().y - 120.f});

    // --- Hover Logic ---
    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    mInsertButton.setHighlight(mInsertButton.getGlobalBounds().contains(mouse));
    mDeleteButton.setHighlight(mDeleteButton.getGlobalBounds().contains(mouse));
    mBuildButton.setHighlight(mBuildButton.getGlobalBounds().contains(mouse));
    mClearButton.setHighlight(mClearButton.getGlobalBounds().contains(mouse));
    mPlayPauseButton.setHighlight(mPlayPauseButton.getGlobalBounds().contains(mouse));
    mStepButton.setHighlight(mStepButton.getGlobalBounds().contains(mouse));
    mPrevButton.setHighlight(mPrevButton.getGlobalBounds().contains(mouse));
    
    mInputBox.setOutlineColor(mInputFocused ? UITheme::Color::HeapFocus : UITheme::Color::HeapNodeOutline);
    mInputText.setString(mInputBuffer + (mInputFocused ? "|" : ""));

    if (!mIsPlaying || mPendingActions.empty()) return;

    mActionTimer += deltaTime;
    if (mActionTimer >= mActionInterval) {
        mActionTimer = 0.f;
        processNextAction();
    }
}

void HeapVisualizer::render(sf::RenderWindow& window) const {
    window.draw(mPanel);

    drawArray(window);
    drawTree(window);
    drawLegend(window);
    window.draw(mStatusText);
    
    // --- Draw Left Panel ---
    sf::RectangleShape leftMenu(sf::Vector2f(g_leftWidth, window.getSize().y));
    leftMenu.setFillColor(UITheme::Color::GraphPanelBg);
    window.draw(leftMenu);

    sf::RectangleShape leftTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
    leftTab.setFillColor(UITheme::Color::GraphTabBg);
    leftTab.setPosition(g_leftWidth - TAB_WIDTH, window.getSize().y / 2.f - TAB_HEIGHT / 2.f);
    window.draw(leftTab);

    sf::Text lIcon(g_leftExpanded ? "<<" : ">>", mFont, 18);
    sf::FloatRect lb = lIcon.getLocalBounds();
    lIcon.setOrigin(lb.left + lb.width/2.f, lb.top + lb.height/2.f);
    lIcon.setPosition(g_leftWidth - TAB_WIDTH/2.f, window.getSize().y / 2.f - 2.f);
    lIcon.setFillColor(UITheme::Color::NodeOutlineColor);
    window.draw(lIcon);

    // Left Content
    drawInputArea(window);
    drawButtons(window);

    // --- Draw Right Panel ---
    float winW = window.getSize().x;
    sf::RectangleShape rightMenu(sf::Vector2f(g_rightWidth, window.getSize().y));
    rightMenu.setFillColor(UITheme::Color::GraphPanelBg);
    rightMenu.setPosition(winW - g_rightWidth, 0);
    window.draw(rightMenu);

    sf::RectangleShape rightTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
    rightTab.setFillColor(UITheme::Color::GraphTabBg);
    rightTab.setPosition(winW - g_rightWidth, window.getSize().y / 2.f - TAB_HEIGHT / 2.f);
    window.draw(rightTab);

    sf::Text rIcon(g_rightExpanded ? ">>" : "<<", mFont, 18);
    sf::FloatRect rb = rIcon.getLocalBounds();
    rIcon.setOrigin(rb.left + rb.width/2.f, rb.top + rb.height/2.f);
    rIcon.setPosition(winW - g_rightWidth + TAB_WIDTH/2.f, window.getSize().y / 2.f - 2.f);
    rIcon.setFillColor(UITheme::Color::NodeHighlightColor);
    window.draw(rIcon);

    // Right Content
    const_cast<CodePanel&>(mCodePanel).draw(window);
}

void HeapVisualizer::reset() {
    mInputFocused = false;
    mPendingActions.clear();
    mHighlight = {};
    mActionTimer = 0.f;
}

// ... [runInsert, runDeleteRoot, runBuildHeap, runClear, togglePlayback, queueOperation, processNextAction, processPreviousAction remain unchanged logically, identical to your previous implementation] ...

void HeapVisualizer::runInsert() {
    int value = 0;
    if (!tryParseSingleValue(value)) {
        setStatus("Insert expects a single integer.");
        return;
    }
    loadInsertCode();
    const std::vector<int> startArray = mPendingActions.empty() ? mDisplayArray : mHeap.getArray();
    if (startArray.size() >= MAX_RENDERED_NODES) {
        setStatus("Limit reached. Clear or use fewer than 31 nodes.");
        return;
    }

    mHeap.BuildHeap(startArray);
    mHeap.flushActions();
    mHeap.Insert(value);
    queueOperation(startArray);
    clearInput();
    setStatus("Inserted " + std::to_string(value) + ".");
}

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

void HeapVisualizer::runBuildHeap() {
    bool ok = false;
    std::vector<int> values = parseSequence(ok);
    if (!ok) {
        setStatus("Build expects integers separated by spaces or commas.");
        return;
    }
    loadHeapifyCode();
    if (values.size() > MAX_RENDERED_NODES) {
        values.resize(MAX_RENDERED_NODES);
        setStatus("Build input trimmed to 31 nodes for rendering.");
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
    for (const Action& action : actions) mPendingActions.push_back(action);
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
    for (const Action& action : actions) mPendingActions.push_back(action);

    if (mPendingActions.empty()) mDisplayArray = mHeap.getArray();
    else if (!mIsPlaying) processNextAction();
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
            mHighlight.firstColor = UITheme::Color::HeapFocus;
            mHighlight.label = "Inserted " + std::to_string(action.index2);
        }
        break;
    case ActionType::COMPARE:
        mHighlight.first = action.index1;
        mHighlight.second = action.index2;
        mHighlight.firstColor = UITheme::Color::HeapCompare;
        mHighlight.secondColor = UITheme::Color::HeapCompare;
        mHighlight.label = "Compare";
        break;
    case ActionType::SWAP:
        if (action.index1 >= 0 && action.index2 >= 0 &&
            static_cast<std::size_t>(action.index1) < mDisplayArray.size() &&
            static_cast<std::size_t>(action.index2) < mDisplayArray.size()) {
            std::swap(mDisplayArray[action.index1], mDisplayArray[action.index2]);
            mHighlight.first = action.index1;
            mHighlight.second = action.index2;
            mHighlight.firstColor = UITheme::Color::HeapSwap;
            mHighlight.secondColor = UITheme::Color::HeapSwap;
            mHighlight.label = "Swap";
        }
        break;
    case ActionType::HIGHLIGHT:
        mHighlight.first = action.index1;
        mHighlight.firstColor = UITheme::Color::HeapFocus;
        mHighlight.label = "Focus";
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
            mHighlight.firstColor = UITheme::Color::HeapSwap;
            mHighlight.secondColor = UITheme::Color::HeapSwap;
            mHighlight.label = "Undo Swap";
        }
        break;
    case ActionType::COMPARE:
    case ActionType::HIGHLIGHT:
        mHighlight.label = "Undo " + std::string(action.type == ActionType::COMPARE ? "Compare" : "Focus");
        break;
    }

    mCodePanel.highlight(action.lineIdx);
    mActiveLine = mHistory.empty() ? -1 : mHistory.back().lineIdx;
    mPendingActions.push_front(action);
}

void HeapVisualizer::loadHeapifyCode() {
    std::map<std::string, std::vector<std::string>> snippets;
    snippets["heapify"] = {
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
    mCodePanel.loadSnippets(snippets);
    mCodePanel.update("heapify", -1);
}

void HeapVisualizer::loadInsertCode() {
    std::map<std::string, std::vector<std::string>> snippets;
    snippets["insert"] = {
        "insert(val):",
        "  A.push(val)",
        "  i = last_idx",
        "  while i > 0 && A[p] < A[i]:",
        "    swap(A[i], A[p])",
        "    i = parent(i)"
    };
    mCodePanel.loadSnippets(snippets);
    mCodePanel.update("insert", -1);
}

void HeapVisualizer::drawPanel(sf::RenderWindow& window) const {}

void HeapVisualizer::drawInputArea(sf::RenderWindow& window) const {
    window.draw(mInputBox);
    if (mInputBuffer.empty()) window.draw(mPlaceholderText);
    else window.draw(mInputText);
    window.draw(mHintText);
}

void HeapVisualizer::drawButtons(sf::RenderWindow& window) const {
    mInsertButton.draw(window);
    mDeleteButton.draw(window); 
    mBuildButton.draw(window);
    mClearButton.draw(window);
    mReturnButton.draw(window);
    mPlayPauseButton.draw(window);
    mStepButton.draw(window);
    mPrevButton.draw(window);
    window.draw(mSpeedLabel);
    window.draw(mSliderTrack);
    window.draw(mSliderKnob);
}

void HeapVisualizer::drawArray(sf::RenderWindow& window) const {
    const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
    if (visibleNodes == 0) return;

    const float gap = 4.f;
    const float cellWidth = std::min(48.f, (g_TreeWidth - (visibleNodes * gap)) / visibleNodes);
    const float startX = g_TreeLeftX + (g_TreeWidth - (visibleNodes * (cellWidth + gap))) / 2.f; 
    
    for (std::size_t i = 0; i < visibleNodes; ++i) {
        sf::RectangleShape cell({cellWidth, cellWidth});
        cell.setPosition({startX + i * (cellWidth + gap), ARRAY_Y});
        cell.setFillColor(UITheme::Color::HeapNodeFill);
        cell.setOutlineThickness(2.f);
        cell.setOutlineColor(nodeColor(i));
        window.draw(cell);

        unsigned int fontSize = cellWidth < 35.f ? 12 : 18;
        sf::Text valueText = makeText(mFont, std::to_string(mDisplayArray[i]), fontSize, UITheme::Color::TextDark, {0.f, 0.f});
        sf::FloatRect bounds = valueText.getLocalBounds();
        valueText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        valueText.setPosition({cell.getPosition().x + cellWidth / 2.f, cell.getPosition().y + cellWidth / 2.f - 2.f});
        window.draw(valueText);

        sf::Text indexText = makeText(mFont, std::to_string(i), 12, UITheme::Color::TextMuted, {0.f, 0.f});
        sf::FloatRect idxBounds = indexText.getLocalBounds();
        indexText.setOrigin(idxBounds.left + idxBounds.width / 2.f, 0.f);
        indexText.setPosition({cell.getPosition().x + cellWidth / 2.f, cell.getPosition().y - 18.f});
        window.draw(indexText);
    }
}

void HeapVisualizer::drawTree(sf::RenderWindow& window) const {
    const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
    for (std::size_t i = 1; i < visibleNodes; ++i) {
        const std::size_t parent = (i - 1) / 2;
        const sf::Vector2f start = nodePosition(parent);
        const sf::Vector2f end = nodePosition(i);
        sf::Vertex line[] = {
            sf::Vertex(start, UITheme::Color::HeapNodeOutline),
            sf::Vertex(end, UITheme::Color::HeapNodeOutline)
        };
        window.draw(line, 2, sf::Lines);
    }

    for (std::size_t i = 0; i < visibleNodes; ++i) {
        sf::CircleShape node(NODE_RADIUS);
        node.setOrigin({NODE_RADIUS, NODE_RADIUS});
        node.setPosition(nodePosition(i));
        node.setFillColor(UITheme::Color::HeapNodeFill);
        node.setOutlineThickness(4.f);
        node.setOutlineColor(nodeColor(i));
        window.draw(node);

        sf::Text valueText = makeText(mFont, std::to_string(mDisplayArray[i]), 18, UITheme::Color::TextWhite, {0.f, 0.f});
        sf::FloatRect bounds = valueText.getLocalBounds();
        valueText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        valueText.setPosition({node.getPosition().x, node.getPosition().y - 1.f});
        window.draw(valueText);
    }
}

void HeapVisualizer::drawLegend(sf::RenderWindow& window) const {
    const float LEGEND_Y = window.getSize().y - 60.f;
    
    if (!mHighlight.label.empty()) {
        sf::Text stepText = makeText(mFont, "Step: " + mHighlight.label, 16, UITheme::Color::HeapTextHighlight, {g_TreeLeftX, LEGEND_Y});
        window.draw(stepText);
    }

    float itemX = g_TreeLeftX + (g_TreeWidth / 2.f) - 150.f; 

    sf::CircleShape compare(8.f);
    compare.setFillColor(UITheme::Color::HeapCompare);
    compare.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(compare);
    window.draw(makeText(mFont, "Compare", 15, UITheme::Color::TextWhite, {itemX + 25.f, LEGEND_Y}));

    itemX += 120.f;
    sf::CircleShape swap(8.f);
    swap.setFillColor(UITheme::Color::HeapSwap);
    swap.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(swap);
    window.draw(makeText(mFont, "Swap", 15, UITheme::Color::TextWhite, {itemX + 25.f, LEGEND_Y}));

    itemX += 100.f;
    sf::CircleShape focus(8.f);
    focus.setFillColor(UITheme::Color::HeapFocus);
    focus.setPosition({itemX, LEGEND_Y + 4.f});
    window.draw(focus);
    window.draw(makeText(mFont, "Focused", 15, UITheme::Color::TextWhite, {itemX + 25.f, LEGEND_Y}));
}

void HeapVisualizer::drawCodeSnippet(sf::RenderWindow& window) const {}

void HeapVisualizer::appendDigit(char digit) {
    if (mInputBuffer.size() < 60) mInputBuffer.push_back(digit);
}

void HeapVisualizer::appendCharacter(char character) {
    if ((character >= '0' && character <= '9') || character == '-' || character == ',' || character == ' ') appendDigit(character);
}

void HeapVisualizer::backspaceInput() {
    if (!mInputBuffer.empty()) { mInputBuffer.pop_back(); mInputText.setString(mInputBuffer); }
}

void HeapVisualizer::setStatus(const std::string& status) {
    mStatusMessage = status;
    mStatusText.setString(status);
    mPlayPauseButton.setText(mIsPlaying ? "Pause" : "Play");
}

bool HeapVisualizer::tryParseSingleValue(int& value) const {
    std::stringstream stream(mInputBuffer); stream >> value; return !stream.fail() && stream.eof();
}

std::vector<int> HeapVisualizer::parseSequence(bool& ok) const {
    std::vector<int> values;
    ok = true;
    std::string normalized = mInputBuffer;
    std::replace(normalized.begin(), normalized.end(), ',', ' ');
    std::stringstream stream(normalized);
    int value = 0;
    while (stream >> value) values.push_back(value);
    if (!stream.eof()) ok = false;
    return values;
}

sf::Vector2f HeapVisualizer::nodePosition(std::size_t index) const {
    const int level = static_cast<int>(std::floor(std::log2(static_cast<float>(index + 1))));
    const std::size_t firstIndexInLevel = (1u << level) - 1u;
    const std::size_t positionInLevel = index - firstIndexInLevel;
    const std::size_t nodesInLevel = 1u << level;
    
    // Scales the tree width dynamically based on open panels
    const float horizontalGap = g_TreeWidth / static_cast<float>(nodesInLevel);
    const float x = g_TreeLeftX + horizontalGap * (static_cast<float>(positionInLevel) + 0.5f);
    const float y = TREE_TOP_Y + level * 60.f;
    return {x, y};
}

sf::Color HeapVisualizer::nodeColor(std::size_t index) const {
    if (static_cast<int>(index) == mHighlight.first)  return mHighlight.firstColor;
    if (static_cast<int>(index) == mHighlight.second) return mHighlight.secondColor;
    return UITheme::Color::HeapNodeOutline;
}

void HeapVisualizer::clearInput() { mInputBuffer.clear(); mInputText.setString(""); }

int HeapVisualizer::run(sf::RenderWindow& window, sf::Font& font) {
    if (!mBgTexture.loadFromFile("assets/textures/avl_background.png")) std::cerr << "Failed to load background.png\n";
    mBgSprite.setTexture(mBgTexture);
    
    mBgSprite.setScale(static_cast<float>(window.getSize().x) / mBgTexture.getSize().x, 
                       static_cast<float>(window.getSize().y) / mBgTexture.getSize().y);
                       
    mBgSprite.setColor(UITheme::Color::HeapBackground);

    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); return -1; }
            if (mReturnButton.isClicked(event, window)) return 0;
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) return 0;

            handleEvent(event, window);
        }

        update(deltaTime, window);

        window.clear(UITheme::Color::HeapBackground);
        window.draw(mBgSprite);
        render(window);         
        window.display();
    }
    return -1;
}