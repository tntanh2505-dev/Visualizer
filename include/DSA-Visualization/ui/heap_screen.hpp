#pragma once

#include <SFML/Graphics.hpp>
#include "DSA-Visualization/ui/Screen.hpp"
#include "DSA-Visualization/ui/CodePanel.hpp"
#include <deque>
#include <string>
#include <vector>
#include <fstream>

#include "DSA-Visualization/ui/button.hpp"
#include "DSA-Visualization/heap/heap.hpp"

// Define the Tab State for the Right Panel
enum class HeapRightTabState { INFO, CODE };

class HeapVisualizer : public Screen {
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

    void runLoadFile();
    void runInsert();
    void runDeleteSelected();
    void runBuildHeap();
    void runClear();
    void runUpdate();
    void runRandom();
    void runSkip();
    void togglePlayback();
    void queueOperation(const std::vector<int>& startArray);
    void processNextAction();
    void clearInput();
    void processPreviousAction();
    void loadHeapifyCode();
    void loadInsertCode();

    void drawInputArea(sf::RenderWindow& window) const;
    void drawButtons(sf::RenderWindow& window) const;
    void drawArray(sf::RenderWindow& window) const;
    void drawTree(sf::RenderWindow& window) const;
    void drawLegend(sf::RenderWindow& window) const;
    void drawCodeSnippet(sf::RenderWindow& window) const;
    void applyTheme();
    void drawThemeToggle(sf::RenderWindow& window) const;

    void appendDigit(char digit);
    void appendCharacter(char character);
    void backspaceInput();
    void setStatus(const std::string& status);
    bool tryParseSingleValue(int& value) const;
    std::vector<int> parseSequence(bool& ok) const;
    sf::Vector2f nodePosition(std::size_t index) const;
    sf::Color nodeColor(std::size_t index) const;

    // --- Properties for Resizing & Workspace ---
    float mBaseWidth = 1280.f;
    float mBaseHeight = 720.f;
    sf::RectangleShape mWorkspaceBg;

    // Panels
    CodePanel mCodePanel;
    const sf::Font& mFont;
    MaxHeap mHeap;
    std::vector<int> mDisplayArray;
    std::deque<Action> mPendingActions;
    std::vector<Action> mHistory;
    HighlightState mHighlight;

    std::vector<std::string> mCurrentCode;
    int mActiveLine = -1;
    int mSelectedIndex = -1;
    
    // Layout logic synced with Linked List
    bool mLeftExpanded = true;
    bool mRightExpanded = true;
    float mLeftWidth = 300.f;  
    float mRightWidth = 450.f;
    const float SIDEBAR_MAX_WIDTH = 300.f;
    const float CODE_PANEL_MAX_WIDTH = 450.f;
    const float TAB_WIDTH = 35.f;
    float mWorkspaceCenterX = 640.f;

    sf::RectangleShape mInputBox;

    // Slider
    sf::RectangleShape mSliderTrack;
    sf::CircleShape mSliderKnob;
    bool mIsDraggingSlider = false;
    const float MIN_INTERVAL = 0.1f;
    const float MAX_INTERVAL = 2.0f;

    // Texts
    sf::Text mTitleText;
    sf::Text mSubtitleText;
    sf::Text mInputLabel;
    sf::Text mSpeedLabel;
    sf::Text mInputText;
    sf::Text mHintText;
    sf::Text mPlaceholderText;
    sf::Text mStatusText;
    sf::Text mRootText;
    sf::Text mLegendText;
    
    // --- Replaced single string with structured vector ---
    std::vector<sf::Text> mInfoTexts;

    // Buttons (Resized to match Linked List)
    ModernButton mInsertButton;
    ModernButton mDeleteButton;
    ModernButton mBuildButton;
    ModernButton mClearButton;
    ModernButton mReturnButton;
    ModernButton mPlayPauseButton;
    ModernButton mStepButton;
    ModernButton mPrevButton;
    ModernButton mLoadButton;
    ModernButton mRandomButton;
    ModernButton mSkipButton;
    ModernButton mUpdateButton;

    // Tab Buttons and State
    HeapRightTabState mRightTabState = HeapRightTabState::CODE;
    ModernButton mInfoTabBtn;
    ModernButton mCodeTabBtn;

    // Background
    sf::Texture mBgTexture;
    sf::Sprite mBgSprite;
    bool notLoaded = true;

    // Animation
    std::string mInputBuffer;
    std::string mStatusMessage;
    bool mInputFocused = false;
    bool mIsPlaying = true;
    float mActionTimer = 0.f;
    float mActionInterval = 0.6f;

    // Customization Vectors for positioning
    sf::Vector2f mColorLabelPos;
    sf::Vector2f mThemeLabelPos;

    // 1. Color
    sf::Color mCurrentNodeColor;
    std::vector<sf::Color> mThemeColors;
    std::vector<sf::RectangleShape> mColorSwatches;
    void drawColorPicker(sf::RenderWindow& window) const;

    // 2. Theme
    ModernButton mDarkThemeBtn;
    ModernButton mLightThemeBtn;
    bool mIsDarkMode = true;

    // 3. Size (Slider)
    float mNodeRadius;
    sf::RectangleShape mSizeSliderTrack;
    sf::CircleShape mSizeSliderKnob;
    sf::Text mSizeLabel;
    bool mIsDraggingSizeSlider = false;

    // 4. Background
    enum class BackgroundType { Default, White, Black };
    BackgroundType mBgType = BackgroundType::Black;
    ModernButton mBgDefaultBtn;
    ModernButton mBgWhiteBtn;
    ModernButton mBgBlackBtn;
    void drawBackgroundToggle(sf::RenderWindow& window) const;
};