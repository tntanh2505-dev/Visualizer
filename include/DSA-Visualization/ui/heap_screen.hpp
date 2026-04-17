#pragma once

#include <SFML/Graphics.hpp>
#include "DSA-Visualization/ui/Screen.hpp"
#include "DSA-Visualization/ui/CodePanel.hpp"
#include <deque>
#include <string>
#include <vector>

#include "DSA-Visualization/ui/button.hpp"
#include "DSA-Visualization/heap/heap.hpp"

class HeapVisualizer : public Screen{
public:
    HeapVisualizer();
    int run(sf::RenderWindow& window, sf::Font& font) override;

    explicit HeapVisualizer(const sf::Font& font);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(float deltaTime, const sf::RenderWindow& window);
    void render(sf::RenderWindow& window) const;
    void reset();

private:
    struct HighlightState {
        int first = -1;
        int second = -1;
        sf::Color firstColor = sf::Color::Transparent;
        sf::Color secondColor = sf::Color::Transparent;
        std::string label;
    };
    
    void runInsert();
    void runDeleteRoot();
    void runBuildHeap();
    void runClear();
    void togglePlayback();
    void queueOperation(const std::vector<int>& startArray);
    void processNextAction();
    void processPreviousAction();
    void loadHeapifyCode();
    void loadInsertCode();

    void drawPanel(sf::RenderWindow& window) const;
    void drawInputArea(sf::RenderWindow& window) const;
    void drawButtons(sf::RenderWindow& window) const;
    void drawArray(sf::RenderWindow& window) const;
    void drawTree(sf::RenderWindow& window) const;
    void drawLegend(sf::RenderWindow& window) const;
    void drawCodeSnippet(sf::RenderWindow& window) const;

    void appendDigit(char digit);
    void appendCharacter(char character);
    void backspaceInput();
    void setStatus(const std::string& status);
    bool tryParseSingleValue(int& value) const;
    std::vector<int> parseSequence(bool& ok) const;
    sf::Vector2f nodePosition(std::size_t index) const;
    sf::Color nodeColor(std::size_t index) const;

    CodePanel mCodePanel;
    const sf::Font& mFont;
    MaxHeap mHeap;
    std::vector<int> mDisplayArray;
    std::deque<Action> mPendingActions;
    std::vector<Action> mHistory;
    HighlightState mHighlight;

    std::vector<std::string> mCurrentCode;
    int mActiveLine = -1;

    sf::RectangleShape mPanel;
    sf::RectangleShape mInputBox;
    sf::RectangleShape mControlPanelBg;
    sf::RectangleShape mCodeBox;

    sf::Text mTitleText;
    sf::Text mSubtitleText;
    sf::Text mInputLabel;
    sf::Text mInputText;
    sf::Text mHintText;
    sf::Text mStatusText;
    sf::Text mRootText;
    sf::Text mLegendText;

    Button mInsertButton;
    Button mDeleteButton;
    Button mBuildButton;
    Button mClearButton;
    Button mReturnButton;
    Button mPlayPauseButton;
    Button mStepButton;
    Button mPrevButton;

    sf::Texture mBgTexture;
    sf::Sprite mBgSprite;

    std::string mInputBuffer;
    std::string mStatusMessage;
    bool mInputFocused = false;
    bool mIsPlaying = true;
    float mActionTimer = 0.f;
    float mActionInterval = 0.6f;
};
