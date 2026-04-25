#pragma once
#include "DSA-Visualization/ui/Screen.hpp"
#include "DSA-Visualization/ui/button.hpp"
#include "DSA-Visualization/ui/CodePanel.hpp"
#include "DSA-Visualization/avl_tree/AVL_tree.hpp"
#include "DSA-Visualization/animation/AnimationController.hpp"
#include <optional>
#include <string>
#include <vector>

enum class OpType { Insert, Delete, Search, Clear, Update };
struct Operation {
    OpType type;
    int value;
    int oldValue = -1; // only used for Update
};

// Define the Tab State for the Right Panel
enum class AVLRightTabState { INFO, CODE };

class AVLScreen : public Screen {
public:
    AVLScreen();
    int run(sf::RenderWindow& window, sf::Font& font) override;

private:

    void buildSteps(Operation op);
    void drawTree(sf::RenderWindow& window, const sf::Font& font, float shiftX);
    void drawNode(sf::RenderWindow& window, const sf::Font& font,
                  const NodeState& ns, float t, float shiftX);
    void drawEdges(sf::RenderWindow& window,
                   const std::vector<NodeState>& nodes, float t, float shiftX);
                   
    void drawLeftPanel(sf::RenderWindow& window, const sf::Font& font, float leftBaseX);
    void drawRightPanel(sf::RenderWindow& window, const sf::Font& font, float rightBaseX);
    void drawDescription(sf::RenderWindow& window, const sf::Font& font, float shiftX);

    AVLTree             mTree;
    AnimationController mController;
    CodePanel           mCodePanel;

    std::optional<ModernButton> mInsertBtn;
    std::optional<ModernButton> mDeleteBtn;
    std::optional<ModernButton> mSearchBtn;
    std::optional<ModernButton> mRandomBtn;
    std::optional<ModernButton> mClearBtn;
    std::optional<ModernButton> mPrevBtn;
    std::optional<ModernButton> mNextBtn;
    std::optional<ModernButton> mReturnBtn;
    std::optional<ModernButton> mSkipAnimationBtn;
    std::optional<ModernButton> mLoadFileBtn;
    std::optional<ModernButton> mUpdateBtn;

    std::string           mInputString;
    bool                  mInputActive;

    sf::Texture           mBgTexture;
    sf::Sprite            mBgSprite;

    sf::RectangleShape    mSliderTrack;
    sf::CircleShape       mSliderHandle;
    float                 mSpeedValue;
    bool                  mSliderDragging;

    std::vector<Operation> mHistory;
    int                    mHistoryIndex;

    // --- Sliding Panels Architecture ---
    float m_leftWidth;
    float m_rightWidth;
    bool  m_leftExpanded;
    bool  m_rightExpanded;
    int   m_selectedNodeValue;

    // --- Color Customization ---
    sf::Color                       mCurrentNodeColor;
    std::vector<sf::RectangleShape> mColorSwatches;
    std::vector<sf::Color>          mThemeColors;

    bool  mStepMode = false;
    bool  mStepAnimatingNext = false;
    bool  mStepAnimatingPrev = false;

    static const std::vector<std::string> INSERT_CODE;
    static const std::vector<std::string> DELETE_CODE;
    static const std::vector<std::string> SEARCH_CODE;

    // --- Resizing & Workspace Properties ---
    float mBaseWidth = 1280.f;
    float mBaseHeight = 720.f;
    sf::RectangleShape mWorkspaceBg;
    sf::RectangleShape mInputBox; 

    // --- Tab and Customization Properties ---
    AVLRightTabState mRightTabState = AVLRightTabState::CODE;
    std::optional<ModernButton> mInfoTabBtn;
    std::optional<ModernButton> mCodeTabBtn;
    sf::Text mInfoTextDisplay;

    sf::Vector2f mColorLabelPos;
    sf::Vector2f mThemeLabelPos;

    std::optional<ModernButton> mDarkThemeBtn;
    std::optional<ModernButton> mLightThemeBtn;
    bool m_isLightMode = false;
    void applyTheme(sf::Font& font); 
};