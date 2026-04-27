#pragma once

#include "DSA-Visualization/linked_list/SLL_UI.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"
#include <string>
#include <vector>
#include <fstream>

namespace SLL {

struct SLLNode {
    int val;
    int UID; 
    SLLNode* next;
    
    SLLNode(int x);
};

// --- SLL Math Core ---
void ChangeNodeValue(SLLNode* head, int targetUID, int newVal);
void DeleteNodeByUID(SLLNode*& head, int targetUID);
void DeleteAll(SLLNode*& head);

// --- SLL Visual Snapshot Math ---
Snapshot TakeSnapshot_SLL(SLLNode* head, std::string desc, int highlightedUID = INT_MIN, bool isKeyStage = false, std::string algorithmName = "", int activeLine = -1);
void ApplySLLLayout(Snapshot& snap, float startX, float startY);

// --- Visual Algorithms ---
void InsertBack_Visual(SLLNode*& head, int x, std::vector<Snapshot>& timeline);
void InsertFront_Visual(SLLNode*& head, int x, std::vector<Snapshot>& timeline);
void DeleteFront_Visual(SLLNode*& head, std::vector<Snapshot>& timeline);
void DeleteBack_Visual(SLLNode*& head, std::vector<Snapshot>& timeline);
int Search_Visual(SLLNode* head, int targetValue, std::vector<Snapshot>& timeline);

// --- SLL Scene ---
enum class SLLTabState { Info, Code };

class LinkedListScene : public Scene {
private:
    float baseWidth;
    float baseHeight;
    SLLNode* pHead;
    std::vector<VisualNode> nodes;
    std::vector<Connector> lines;
    std::vector<Snapshot> timeline;
    ModernButton menuBtn;
    
    int currentFrame; 
    bool isAutoPlaying; 
    int pendingSearchUID;
    sf::Clock animationClock; 
    float timeInterval; 
    float dt; 
    sf::Font* fontPtr;

    std::vector<ModernButton> buttons; 
    std::vector<TextBox> boxes;
    NodePanel nodePanel; 
    CodePanel codePanel; 
    TimelineSlider slider; 
    SpeedController speedCtrl;

    // --- Customization Properties ---
    sf::Color mCurrentNodeColor;
    std::vector<sf::Color> mThemeColors;
    std::vector<sf::RectangleShape> mColorSwatches;
    float mColorSwatchesStartY;
    bool mIsDarkMode;
    ModernButton mDarkThemeBtn;
    ModernButton mLightThemeBtn;
    sf::Text mStatusText;
    sf::RectangleShape mWorkspaceBg;
    sf::Color mPanelBgColor;

    // --- Right Panel Tabs ---
    SLLTabState activeTab;
    ModernButton mInfoTabBtn;
    ModernButton mCodeTabBtn;
    std::vector<sf::Text> mInfoTexts;

    // --- Sliding Panel Layout Properties ---
    float leftWidth;
    float rightWidth;
    bool leftExpanded;
    bool rightExpanded;
    const float LEFT_PANEL_WIDTH = 300.f;
    const float RIGHT_PANEL_WIDTH = 450.f;
    const float TAB_WIDTH = 35.f;
    const float TAB_HEIGHT = 50.f;
    
    void applyTheme();
    void UpdateVisualsFromFrame();

public:
    LinkedListScene(sf::Font& font, float windowWidth, float windowHeight);
    ~LinkedListScene() override;

    void HandleEvent(sf::Event& event, sf::RenderWindow& window, SceneType& nextScene) override;
    void Update(float deltaTime, sf::RenderWindow& window) override;
    void Draw(sf::RenderWindow& window) override;
};

} // namespace SLL
