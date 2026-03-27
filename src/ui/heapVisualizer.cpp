#include "DSA-Visualization/ui/heapVisualizer.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace {
constexpr float PANEL_X = 286.f;
constexpr float PANEL_Y = 22.f;
constexpr float PANEL_WIDTH = 974.f;
constexpr float PANEL_HEIGHT = 676.f;
constexpr float INPUT_X = 322.f;
constexpr float INPUT_Y = 146.f;
constexpr float INPUT_WIDTH = 422.f;
constexpr float INPUT_HEIGHT = 44.f;
constexpr float BUTTON_WIDTH = 154.f;
constexpr float BUTTON_HEIGHT = 44.f;
constexpr float NODE_RADIUS = 24.f;
constexpr float TREE_TOP_Y = 312.f;
constexpr float TREE_LEFT_X = 320.f;
constexpr float TREE_WIDTH = 900.f;
constexpr float ARRAY_Y = 594.f;
constexpr float ARRAY_CELL = 52.f;
constexpr std::size_t MAX_RENDERED_NODES = 31;

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
    , mTitleText(makeText(font, "Heap Visualizer", 34, sf::Color(241, 246, 255), {320.f, 44.f}))
    , mSubtitleText(makeText(font, "Build, insert, delete, and inspect a max heap with animated steps.", 16, sf::Color(210, 220, 236), {322.f, 96.f}))
    , mInputLabel(makeText(font, "Input", 18, sf::Color(241, 246, 255), {322.f, 118.f}))
    , mInputText(makeText(font, "", 22, sf::Color(26, 32, 44), {336.f, 154.f}))
    , mHintText(makeText(font, "Insert: one integer. Build Heap: space- or comma-separated integers.", 14, sf::Color(189, 198, 214), {322.f, 204.f}))
    , mStatusText(makeText(font, "", 16, sf::Color(251, 209, 101), {322.f, 234.f}))
    , mRootText(makeText(font, "Root: --", 18, sf::Color(241, 246, 255), {322.f, 258.f}))
    , mLegendText(makeText(font, "Compare    Swap    Focused node", 15, sf::Color(225, 232, 242), {1020.f, 650.f}))
    , mInsertButton("Insert", font, {774.f, 146.f}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mDeleteButton("Delete Root", font, {938.f, 146.f}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mBuildButton("Build Heap", font, {1102.f, 146.f}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mClearButton("Clear", font, {774.f, 202.f}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mPlayPauseButton("Pause", font, {938.f, 202.f}, {BUTTON_WIDTH, BUTTON_HEIGHT})
    , mStepButton("Step", font, {1102.f, 202.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}) {
    mPanel.setPosition({PANEL_X, PANEL_Y});
    mPanel.setSize({PANEL_WIDTH, PANEL_HEIGHT});
    mPanel.setFillColor(sf::Color(9, 16, 29, 222));
    mPanel.setOutlineThickness(2.f);
    mPanel.setOutlineColor(sf::Color(72, 98, 138));

    mInputBox.setPosition({INPUT_X, INPUT_Y});
    mInputBox.setSize({INPUT_WIDTH, INPUT_HEIGHT});
    mInputBox.setFillColor(sf::Color(244, 247, 252));
    mInputBox.setOutlineThickness(2.f);
    mInputBox.setOutlineColor(sf::Color(113, 139, 178));

    setStatus("Ready. Insert a value or build from a sequence.");
}

void HeapVisualizer::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f mouse = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
        mInputFocused = mInputBox.getGlobalBounds().contains(mouse);
    }

    if (mInsertButton.isClicked(event, window)) {
        runInsert();
    }
    if (mDeleteButton.isClicked(event, window)) {
        runDeleteRoot();
    }
    if (mBuildButton.isClicked(event, window)) {
        runBuildHeap();
    }
    if (mClearButton.isClicked(event, window)) {
        runClear();
    }
    if (mPlayPauseButton.isClicked(event, window)) {
        togglePlayback();
    }
    if (mStepButton.isClicked(event, window)) {
        if (!mPendingActions.empty()) {
            processNextAction();
        } else {
            setStatus("No pending animation steps.");
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

void HeapVisualizer::update(float deltaTime, const sf::RenderWindow& window) {
    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    mInsertButton.setHighlight(mInsertButton.getGlobalBounds().contains(mouse));
    mDeleteButton.setHighlight(mDeleteButton.getGlobalBounds().contains(mouse));
    mBuildButton.setHighlight(mBuildButton.getGlobalBounds().contains(mouse));
    mClearButton.setHighlight(mClearButton.getGlobalBounds().contains(mouse));
    mPlayPauseButton.setHighlight(mPlayPauseButton.getGlobalBounds().contains(mouse));
    mStepButton.setHighlight(mStepButton.getGlobalBounds().contains(mouse));

    mInputBox.setOutlineColor(mInputFocused ? sf::Color(255, 187, 76) : sf::Color(113, 139, 178));
    mInputText.setString(mInputBuffer + (mInputFocused ? "|" : ""));
    mRootText.setString(mDisplayArray.empty() ? "Root: --" : "Root: " + std::to_string(mDisplayArray.front()));

    if (!mIsPlaying || mPendingActions.empty()) {
        return;
    }

    mActionTimer += deltaTime;
    if (mActionTimer >= mActionInterval) {
        mActionTimer = 0.f;
        processNextAction();
    }
}

void HeapVisualizer::render(sf::RenderWindow& window) const {
    drawPanel(window);
    drawInputArea(window);
    drawButtons(window);
    drawTree(window);
    drawArray(window);
    drawLegend(window);
}

void HeapVisualizer::reset() {
    mInputFocused = false;
    mPendingActions.clear();
    mHighlight = {};
    mActionTimer = 0.f;
}

void HeapVisualizer::runInsert() {
    int value = 0;
    if (!tryParseSingleValue(value)) {
        setStatus("Insert expects a single integer.");
        return;
    }

    const std::vector<int> startArray = mPendingActions.empty() ? mDisplayArray : mHeap.getArray();
    if (startArray.size() >= MAX_RENDERED_NODES) {
        setStatus("Limit reached. Clear or use fewer than 31 nodes.");
        return;
    }

    mHeap.BuildHeap(startArray);
    mHeap.flushActions();
    mHeap.Insert(value);
    queueOperation(startArray);
    setStatus("Inserted " + std::to_string(value) + ".");
}

void HeapVisualizer::runDeleteRoot() {
    const std::vector<int> startArray = mPendingActions.empty() ? mDisplayArray : mHeap.getArray();
    if (startArray.empty()) {
        setStatus("Heap is empty.");
        return;
    }

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
        setStatus("Build Heap expects integers separated by spaces or commas.");
        return;
    }

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

    const std::vector<Action> actions = mHeap.flushActions();
    for (const Action& action : actions) {
        mPendingActions.push_back(action);
    }
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
    mHighlight = {};
    mActionTimer = 0.f;

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

void HeapVisualizer::processNextAction() {
    if (mPendingActions.empty()) {
        mDisplayArray = mHeap.getArray();
        mHighlight = {};
        return;
    }

    const Action action = mPendingActions.front();
    mPendingActions.pop_front();
    mHighlight = {};

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
    }

    if (mPendingActions.empty()) {
        mDisplayArray = mHeap.getArray();
    }
}

void HeapVisualizer::drawPanel(sf::RenderWindow& window) const {
    window.draw(mPanel);
    window.draw(mTitleText);
    window.draw(mSubtitleText);
    window.draw(mStatusText);
    window.draw(mRootText);
}

void HeapVisualizer::drawInputArea(sf::RenderWindow& window) const {
    window.draw(mInputLabel);
    window.draw(mInputBox);
    window.draw(mInputText);
    window.draw(mHintText);
}

void HeapVisualizer::drawButtons(sf::RenderWindow& window) const {
    mInsertButton.draw(window);
    mDeleteButton.draw(window);
    mBuildButton.draw(window);
    mClearButton.draw(window);
    mPlayPauseButton.draw(window);
    mStepButton.draw(window);
}

void HeapVisualizer::drawArray(sf::RenderWindow& window) const {
    const std::size_t visibleNodes = std::min(mDisplayArray.size(), MAX_RENDERED_NODES);
    for (std::size_t i = 0; i < visibleNodes; ++i) {
        sf::RectangleShape cell({ARRAY_CELL, ARRAY_CELL});
        cell.setPosition({322.f + i * (ARRAY_CELL + 8.f), ARRAY_Y});
        cell.setFillColor(sf::Color(247, 250, 255, 230));
        cell.setOutlineThickness(2.f);
        cell.setOutlineColor(nodeColor(i));
        window.draw(cell);

        sf::Text valueText = makeText(mFont, std::to_string(mDisplayArray[i]), 18, sf::Color(20, 28, 40), {0.f, 0.f});
        sf::FloatRect bounds = valueText.getLocalBounds();
        valueText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        valueText.setPosition({cell.getPosition().x + ARRAY_CELL / 2.f, cell.getPosition().y + ARRAY_CELL / 2.f - 2.f});
        window.draw(valueText);

        sf::Text indexText = makeText(mFont, std::to_string(i), 14, sf::Color(189, 198, 214), {cell.getPosition().x + 18.f, cell.getPosition().y + 58.f});
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

void HeapVisualizer::drawLegend(sf::RenderWindow& window) const {
    // 1. Compare Dot & Text
    sf::CircleShape compare(8.f);
    compare.setFillColor(sf::Color(87, 190, 255));
    compare.setPosition({950.f, 652.f});
    window.draw(compare);
    window.draw(makeText(mFont, "Compare", 15, sf::Color(225, 232, 242), {970.f, 648.f}));

    // 2. Swap Dot & Text
    sf::CircleShape swap(8.f);
    swap.setFillColor(sf::Color(255, 124, 124));
    swap.setPosition({1038.f, 652.f});
    window.draw(swap);
    window.draw(makeText(mFont, "Swap", 15, sf::Color(225, 232, 242), {1058.f, 648.f}));

    // 3. Focus Dot & Text
    sf::CircleShape focus(8.f);
    focus.setFillColor(sf::Color(248, 196, 76));
    focus.setPosition({1122.f, 652.f});
    window.draw(focus);
    window.draw(makeText(mFont, "Focused node", 15, sf::Color(225, 232, 242), {1142.f, 648.f}));

    // 4. Step Text (Moved down to Y=674 to avoid the array indices)
    if (!mHighlight.label.empty()) {
        sf::Text stepText = makeText(mFont, "Step: " + mHighlight.label, 16, sf::Color(251, 209, 101), {322.f, 674.f});
        window.draw(stepText);
    }

    // 5. Play Status (Moved right to X=550 to avoid hitting the Step text)
    sf::Text playText = makeText(mFont, mIsPlaying ? "Animation: auto" : "Animation: paused", 16, sf::Color(189, 198, 214), {550.f, 674.f});
    window.draw(playText);
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
    }
}

void HeapVisualizer::setStatus(const std::string& status) {
    mStatusMessage = status;
    mStatusText.setString(status);
    mPlayPauseButton = Button(mIsPlaying ? "Pause" : "Play", mFont, {938.f, 202.f}, {BUTTON_WIDTH, BUTTON_HEIGHT});
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
    while (stream >> value) {
        values.push_back(value);
    }

    if (!stream.eof()) {
        ok = false;
    }

    return values;
}

sf::Vector2f HeapVisualizer::nodePosition(std::size_t index) const {
    const int level = static_cast<int>(std::floor(std::log2(static_cast<float>(index + 1))));
    const std::size_t firstIndexInLevel = (1u << level) - 1u;
    const std::size_t positionInLevel = index - firstIndexInLevel;
    const std::size_t nodesInLevel = 1u << level;
    const float horizontalGap = TREE_WIDTH / static_cast<float>(nodesInLevel);
    const float x = TREE_LEFT_X + horizontalGap * (static_cast<float>(positionInLevel) + 0.5f);
    const float y = TREE_TOP_Y + level * 78.f;
    return {x, y};
}

sf::Color HeapVisualizer::nodeColor(std::size_t index) const {
    if (static_cast<int>(index) == mHighlight.first) {
        return mHighlight.firstColor;
    }
    if (static_cast<int>(index) == mHighlight.second) {
        return mHighlight.secondColor;
    }
    return sf::Color(106, 133, 176);
}
