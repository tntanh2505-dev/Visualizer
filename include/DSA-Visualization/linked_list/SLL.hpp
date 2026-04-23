#pragma once
#include "DSA-Visualization/linked_list/SLL_UI.hpp"
#include "DSA-Visualization/ui/UI_Theme.hpp"

namespace SLL {
struct SLLNode {
    int val;
    int UID; 
    SLLNode* next;
    SLLNode(int x) { val = x; UID = UniqueID++; next = NULL; }
};

// --- SLL Math Core ---
inline void ChangeNodeValue(SLLNode* head, int targetUID, int newVal) {
    SLLNode* cur = head;
    while (cur != nullptr) {
        if (cur->UID == targetUID) { cur->val = newVal; return; }
        cur = cur->next;
    }
}

inline void DeleteNodeByUID(SLLNode*& head, int targetUID) {
    if (head == nullptr) return;
    if (head->UID == targetUID) {
        SLLNode* temp = head; head = head->next; delete temp; return;
    }
    SLLNode* cur = head;
    while (cur->next != nullptr && cur->next->UID != targetUID) { cur = cur->next; }
    if (cur->next != nullptr) {
        SLLNode* temp = cur->next; cur->next = cur->next->next; delete temp;
    }
}

inline void DeleteAll(SLLNode*& head) {
    if(!head) return;
    DeleteAll(head->next);
    head->next = NULL;
    delete head;
    head = NULL;
}

// --- SLL Visual Snapshot Math ---
inline Snapshot TakeSnapshot_SLL(SLLNode* head, std::string desc, int highlightedUID = INT_MIN, bool isKeyStage = false, std::string algorithmName = "", int activeLine = -1) {
    Snapshot snap;
    snap.description = desc; snap.isKeyStage = isKeyStage; 
    snap.algorithmName = algorithmName; snap.activeLine = activeLine;
    SLLNode* cur = head;
    while (cur != nullptr) {
        bool highlight = (cur->UID == highlightedUID);
        snap.nodes.push_back({cur->UID, cur->val, highlight, 0.f, 0.f});
        if (cur->next != nullptr) { snap.lines.push_back({cur->UID, cur->next->UID}); }
        cur = cur->next;
    }
    return snap;
}

inline void ApplySLLLayout(Snapshot& snap, float startX, float startY) {
    float horizontalSpacing = 140.f; float verticalSpacing = 120.f; int nodesPerRow = 6;             
    for (int i = 0; i < snap.nodes.size(); ++i) {
        int row = i / nodesPerRow; int col = i % nodesPerRow;
        if(row % 2 == 0) snap.nodes[i].targetX = startX + (col * horizontalSpacing);
        else snap.nodes[i].targetX = startX + ((nodesPerRow - 1) * horizontalSpacing) - (col * horizontalSpacing);
        snap.nodes[i].targetY = startY + (row * verticalSpacing);
    }
}

// Visual Algorithms
inline void InsertBack_Visual(SLLNode*& head, int x, std::vector<Snapshot>& timeline) {
    if (head == nullptr) {
        head = new SLLNode(x);
        timeline.push_back(TakeSnapshot_SLL(head, "Inserted " + std::to_string(x) + " as Head", head->UID, true, "InsertBack", 0));
        return;
    }
    SLLNode* cur = head;
    while (cur->next != nullptr) { 
        timeline.push_back(TakeSnapshot_SLL(head, "Traversing...", cur->UID, false, "InsertBack", 2));
        cur = cur->next;
    }
    cur->next = new SLLNode(x);
    timeline.push_back(TakeSnapshot_SLL(head, "Inserted " + std::to_string(x) + " at Back", cur->next->UID, false, "InsertBack", 3));
    timeline.push_back(TakeSnapshot_SLL(head, "Insertion Complete", INT_MIN, true, "InsertBack", 3));
}

inline void InsertFront_Visual(SLLNode*& head, int x, std::vector<Snapshot>& timeline) {
    SLLNode* newNode = new SLLNode(x);
    timeline.push_back(TakeSnapshot_SLL(head, "Created new node with value " + std::to_string(x), INT_MIN, false, "InsertFront", 0));
    newNode->next = head;
    timeline.push_back(TakeSnapshot_SLL(head, "Linked new node to current head", INT_MIN, false, "InsertFront", 1));
    head = newNode;
    timeline.push_back(TakeSnapshot_SLL(head, "Updated head to point to new node", head->UID, false, "InsertFront", 2));
    timeline.push_back(TakeSnapshot_SLL(head, "Insertion Complete", INT_MIN, true, "InsertFront", 2));
}

inline void DeleteFront_Visual(SLLNode*& head, std::vector<Snapshot>& timeline) {
    timeline.push_back(TakeSnapshot_SLL(head, "Checking if list is empty", INT_MIN, false, "DeleteFront", 0));
    if (head == nullptr) {
        timeline.push_back(TakeSnapshot_SLL(head, "List is empty, nothing to delete.", INT_MIN, true, "DeleteFront", 0));
        return;
    }
    timeline.push_back(TakeSnapshot_SLL(head, "Targeting Front Node for Deletion", head->UID, false, "DeleteFront", 1));
    SLLNode* temp = head;
    head = head->next;
    timeline.push_back(TakeSnapshot_SLL(head, "Moved head pointer to the next node", temp->UID, false, "DeleteFront", 2));
    delete temp;
    timeline.push_back(TakeSnapshot_SLL(head, "Deleted the old front node", INT_MIN, false, "DeleteFront", 3));
    timeline.push_back(TakeSnapshot_SLL(head, "Deletion Complete", INT_MIN, true, "DeleteFront", 3));
}

inline void DeleteBack_Visual(SLLNode*& head, std::vector<Snapshot>& timeline) {
    timeline.push_back(TakeSnapshot_SLL(head, "Checking if list is empty", INT_MIN, false, "DeleteBack", 0));
    if (head == nullptr) {
        timeline.push_back(TakeSnapshot_SLL(head, "List is empty, nothing to delete.", INT_MIN, true, "DeleteBack", 0));
        return;
    }
    timeline.push_back(TakeSnapshot_SLL(head, "Checking if list has only one node", head->UID, false, "DeleteBack", 1));
    if (head->next == nullptr) {
        delete head; head = nullptr;
        timeline.push_back(TakeSnapshot_SLL(head, "Deleted Back Node. List is now empty.", INT_MIN, true, "DeleteBack", 1));
        return;
    }
    SLLNode* cur = head;
    timeline.push_back(TakeSnapshot_SLL(head, "Initialized cur to head", cur->UID, false, "DeleteBack", 2));
    while (cur->next->next != nullptr) { 
        timeline.push_back(TakeSnapshot_SLL(head, "Traversing: Checking next->next", cur->UID, false, "DeleteBack", 3));
        cur = cur->next;
    }
    timeline.push_back(TakeSnapshot_SLL(head, "Found second-to-last node", cur->UID, false, "DeleteBack", 3));
    int targetDeleteUID = cur->next->UID;
    timeline.push_back(TakeSnapshot_SLL(head, "Targeting last node for deletion", targetDeleteUID, false, "DeleteBack", 4));
    delete cur->next; cur->next = nullptr;
    timeline.push_back(TakeSnapshot_SLL(head, "Deletion Complete", INT_MIN, true, "DeleteBack", 4));
}

inline int Search_Visual(SLLNode* head, int targetValue, std::vector<Snapshot>& timeline) {
    SLLNode* cur = head;
    while (cur != nullptr) {
        timeline.push_back(TakeSnapshot_SLL(head, "Searching: Checking " + std::to_string(cur->val), cur->UID, false, "Search", 2));
        if (cur->val == targetValue) {
            timeline.push_back(TakeSnapshot_SLL(head, "Found Node!", cur->UID, true, "Search", 2)); return cur->UID; 
        }
        timeline.push_back(TakeSnapshot_SLL(head, "Moving to next", cur->UID, false, "Search", 3));
        cur = cur->next;
    }
    timeline.push_back(TakeSnapshot_SLL(head, "Not found.", INT_MIN, true, "Search", 5));
    return INT_MIN;
}

// --- SLL Scene ---
class LinkedListScene : public Scene {
private:
    float baseWidth;
    float baseHeight;
    SLLNode* pHead;
    std::vector<VisualNode> nodes;
    std::vector<Connector> lines;
    std::vector<Snapshot> timeline;
    ModernButton menuBtn;
    
    int currentFrame; bool isAutoPlaying; int pendingSearchUID;
    sf::Clock animationClock; float timeInterval; float dt; sf::Font* fontPtr;

    std::vector<ModernButton> buttons; std::vector<TextBox> boxes;
    NodePanel nodePanel; CodePanel codePanel; TimelineSlider slider; SpeedController speedCtrl;

    // --- Customization Properties ---
    sf::Color mCurrentNodeColor;
    std::vector<sf::Color> mThemeColors;
    std::vector<sf::RectangleShape> mColorSwatches;
    bool mIsDarkMode;
    ModernButton mDarkThemeBtn;
    ModernButton mLightThemeBtn;
    sf::Text mStatusText;
    sf::RectangleShape mWorkspaceBg;
    sf::Color mPanelBgColor;

    // --- Sliding Panel Layout Properties ---
    float leftWidth;
    float rightWidth;
    bool leftExpanded;
    bool rightExpanded;
    const float LEFT_PANEL_WIDTH = 300.f;
    const float RIGHT_PANEL_WIDTH = 450.f;
    const float TAB_WIDTH = 35.f;
    const float TAB_HEIGHT = 50.f;
    
    void applyTheme() {
        if (mIsDarkMode) {
            mWorkspaceBg.setFillColor(UITheme::Color::GlobalBg); 
            mPanelBgColor = UITheme::Color::GraphPanelBg;
            
            sf::Color dTop = UITheme::Color::GlobalButtonFill;
            sf::Color dBot = UITheme::Color::GlobalButtonBot;
            sf::Color dBorder = UITheme::Color::GlobalButtonBorder;
            sf::Color dText = UITheme::Color::GlobalTextPrimary;
            
            for (size_t i = 0; i < buttons.size(); ++i) {
                if (i == 11) { // CLEAR ALL Button
                    buttons[i].setColors(UITheme::Color::ButtonDangerFill, UITheme::Color::ButtonDangerBot, UITheme::Color::ButtonDangerBorder, UITheme::Color::GlobalTextPrimary);
                } else {
                    buttons[i].setColors(dTop, dBot, dBorder, dText);
                }
            }
            menuBtn.setColors(dTop, dBot, dBorder, dText);
            mDarkThemeBtn.setColors(dTop, dBot, sf::Color::Cyan, dText);
            mLightThemeBtn.setColors(UITheme::Color::LightButtonFill, UITheme::Color::LightButtonBot, UITheme::Color::ButtonInactiveBorder, UITheme::Color::LightTextPrimary);

            // Update Node Panel
            nodePanel.bg.setFillColor(UITheme::Color::PanelBg);
            nodePanel.infoText.setFillColor(UITheme::Color::GlobalTextPrimary);
            nodePanel.changeBtn.setColors(dTop, dBot, dBorder, dText);
            nodePanel.deleteBtn.setColors(UITheme::Color::ButtonDangerFill, UITheme::Color::ButtonDangerBot, UITheme::Color::ButtonDangerBorder, UITheme::Color::GlobalTextPrimary);
            nodePanel.closeBtn.setColors(UITheme::Color::ButtonDangerFill, UITheme::Color::ButtonDangerBot, UITheme::Color::ButtonDangerBorder, UITheme::Color::GlobalTextPrimary);

        } else {
            mWorkspaceBg.setFillColor(UITheme::Color::LightGlobalBg); 
            mPanelBgColor = UITheme::Color::LightPanelBg;
            
            sf::Color lTop = UITheme::Color::LightButtonFill;
            sf::Color lBot = UITheme::Color::LightButtonBot;
            sf::Color lBorder = UITheme::Color::LightButtonBorder;
            sf::Color lText = UITheme::Color::LightTextPrimary;
            
            for (size_t i = 0; i < buttons.size(); ++i) {
                if (i == 11) { // CLEAR ALL Button
                    buttons[i].setColors(UITheme::Color::ButtonDangerFill, UITheme::Color::ButtonDangerBot, UITheme::Color::ButtonDangerBorder, UITheme::Color::GlobalTextPrimary);
                } else {
                    buttons[i].setColors(lTop, lBot, lBorder, lText);
                }
            }
            menuBtn.setColors(lTop, lBot, lBorder, lText);
            mDarkThemeBtn.setColors(UITheme::Color::GlobalButtonFill, UITheme::Color::GlobalButtonBot, UITheme::Color::ButtonInactiveBorder, UITheme::Color::GlobalTextPrimary);
            mLightThemeBtn.setColors(lTop, lBot, sf::Color::Cyan, lText);

            // Update Node Panel
            nodePanel.bg.setFillColor(UITheme::Color::LightPanelBg);
            nodePanel.infoText.setFillColor(UITheme::Color::LightTextPrimary);
            nodePanel.changeBtn.setColors(lTop, lBot, lBorder, lText);
            nodePanel.deleteBtn.setColors(UITheme::Color::ButtonDangerFill, UITheme::Color::ButtonDangerBot, UITheme::Color::ButtonDangerBorder, UITheme::Color::GlobalTextPrimary);
            nodePanel.closeBtn.setColors(UITheme::Color::ButtonDangerFill, UITheme::Color::ButtonDangerBot, UITheme::Color::ButtonDangerBorder, UITheme::Color::GlobalTextPrimary);
        }
    }

    void UpdateVisualsFromFrame() {
        ApplySLLLayout(timeline[currentFrame], leftWidth + 200.f, 200.f);
        for (const auto& record : timeline[currentFrame].nodes) {
            VisualNode* vn = FindNode(record.UID, nodes);
            if (vn) vn->index = record.index; 
        }
        SyncToFrame(timeline[currentFrame], nodes, lines, *fontPtr);
    }

public:
    LinkedListScene(sf::Font& font, float windowWidth, float windowHeight) 
    : nodePanel(font), codePanel(font), slider({200.f, 750.f}, 1000.f), speedCtrl(font, &timeInterval), 
      menuBtn("MENU", font, {80.f, 40.f}), mDarkThemeBtn("Dark", font, {80.f, 30.f}), mLightThemeBtn("Light", font, {80.f, 30.f})
    {   
        baseWidth = windowWidth;
        baseHeight = windowHeight;
        fontPtr = &font; pHead = nullptr; currentFrame = 0; isAutoPlaying = true; pendingSearchUID = INT_MIN; timeInterval = 0.5f; dt = 0.001f;

        leftWidth = TAB_WIDTH;
        rightWidth = TAB_WIDTH;
        leftExpanded = false;
        rightExpanded = false;

        // Theme Initialization
        mWorkspaceBg.setSize({windowWidth, windowHeight});
        mIsDarkMode = true;
        mCurrentNodeColor = UITheme::Color::GlobalNodeFill;
        
        mThemeColors = {
            sf::Color(245, 249, 255), // White
            sf::Color(181, 58, 199),  // Accent Purple
            sf::Color(52, 152, 219),  // Blue
            sf::Color(231, 76, 60),   // Red
            sf::Color(241, 196, 15),  // Yellow
            sf::Color(46, 204, 113)   // Green
        };

        for (const auto& color : mThemeColors) {
            sf::RectangleShape swatch(sf::Vector2f(25.f, 25.f));
            swatch.setFillColor(color);
            swatch.setOutlineThickness(1.5f);
            swatch.setOutlineColor(sf::Color(100, 100, 100));
            mColorSwatches.push_back(swatch);
        }

        mStatusText.setFont(font);
        mStatusText.setCharacterSize(16);
        mStatusText.setFillColor(sf::Color(251, 209, 101));
        mStatusText.setPosition({350.f, 570.f}); // Aligned with the heap screen logic

        std::map<std::string, std::vector<std::string>> llSnippets;
        llSnippets["InsertFront"] = { 
            "Node* n = new Node(x);", 
            "n->next = head;", 
            "head = n;" 
        };
        llSnippets["InsertBack"] = { 
            "if (!head) { head = new Node(x); return; }", 
            "Node* cur = head;", 
            "while (cur->next) cur = cur->next;", 
            "cur->next = new Node(x);" 
        };
        llSnippets["Search"] = { 
            "Node* cur = head;", 
            "while (cur) {", 
            "  if (cur->val == target) return cur;", 
            "  cur = cur->next;", 
            "}", 
            "return NULL;" 
        };
        llSnippets["DeleteBack"] = { 
            "if (!head) return;", 
            "if (!head->next) { delete head; head = NULL; }", 
            "Node* cur = head;", 
            "while (cur->next->next) cur = cur->next;", 
            "delete cur->next; cur->next = NULL;" 
        };
        llSnippets["DeleteFront"] = { 
            "if (!head) return;", 
            "Node* temp = head;", 
            "head = head->next;", 
            "delete temp;" 
        };
        codePanel.loadSnippets(llSnippets);

        boxes.emplace_back(TextBox(font, {0, 0}, 220, 40)); boxes[0].placeholder = "Enter value..."; boxes[0].allowNegative = true;
        boxes.emplace_back(TextBox(font, {0, 0}, 220, 40)); boxes[1].allowAllChars = true; boxes[1].placeholder = "Paste path (Ctrl+V)...";

        // ModernButton is initialized with (Text, Font, Size)
        buttons.push_back(ModernButton("INS BACK", font, {105.f, 40.f}));  // 0
        buttons.push_back(ModernButton("INS FRONT", font, {105.f, 40.f})); // 1
        buttons.push_back(ModernButton("PREV", font, {70.f, 40.f}));       // 2
        buttons.push_back(ModernButton("PAUSE", font, {70.f, 40.f}));      // 3
        buttons.push_back(ModernButton("NEXT", font, {70.f, 40.f}));       // 4
        buttons.push_back(ModernButton("DEL FRONT", font, {105.f, 40.f})); // 5
        buttons.push_back(ModernButton("DEL BACK", font, {105.f, 40.f}));  // 6
        buttons.push_back(ModernButton("RANDOM", font, {105.f, 40.f}));    // 7
        buttons.push_back(ModernButton("SEARCH", font, {105.f, 40.f}));    // 8
        buttons.push_back(ModernButton("SKIP", font, {70.f, 40.f}));       // 9
        buttons.push_back(ModernButton("BACK", font, {70.f, 40.f}));       // 10
        buttons.push_back(ModernButton("CLEAR ALL", font, {220.f, 40.f})); // 11
        buttons.push_back(ModernButton("ADD FILE", font, {220.f, 40.f}));  // 12     
        buttons.push_back(ModernButton("CREATE RANDOM", font, {220.f, 40.f})); // 13

        applyTheme();

        timeline.push_back(TakeSnapshot_SLL(pHead, "Initial State", INT_MIN, true));
        SyncToFrame(timeline[currentFrame], nodes, lines, font);
    }

    ~LinkedListScene() { DeleteAll(pHead); }

    void HandleEvent(sf::Event& event, sf::RenderWindow& window, SceneType& nextScene) override {
        sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        // --- Tab Slide Interaction ---
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            float centerY = window.getSize().y / 2.f;
            float winW = window.getSize().x;
            
            bool mouseInLeftTab = (mPos.x >= leftWidth - TAB_WIDTH && mPos.x <= leftWidth &&
                                   mPos.y >= centerY - TAB_HEIGHT / 2.f && mPos.y <= centerY + TAB_HEIGHT / 2.f);
            if (mouseInLeftTab) { leftExpanded = !leftExpanded; return; }

            bool mouseInRightTab = (mPos.x >= winW - rightWidth && mPos.x <= winW - rightWidth + TAB_WIDTH &&
                                    mPos.y >= centerY - TAB_HEIGHT / 2.f && mPos.y <= centerY + TAB_HEIGHT / 2.f);
            if (mouseInRightTab) { rightExpanded = !rightExpanded; return; }
        }

        bool isClickingOnPanel = (mPos.x < leftWidth) || (mPos.x > window.getSize().x - rightWidth);

        if (!isClickingOnPanel) {
            for(VisualNode& node : nodes) node.HandleEvent(event, window);
        }

        for(TextBox& box : boxes) box.HandleEvent_IB(event, window);
        speedCtrl.HandleEvent(event, window);

        int previousFrame = currentFrame;
        slider.HandleEvent(event, window, currentFrame, timeline.size() - 1, timeline, nodes, lines, *fontPtr, isAutoPlaying, buttons[3]);
        if (currentFrame != previousFrame) { UpdateVisualsFromFrame(); }

        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            // Theme toggle clicks
            if (mDarkThemeBtn.isClicked(mPos, true)) { mIsDarkMode = true; applyTheme(); }
            if (mLightThemeBtn.isClicked(mPos, true)) { mIsDarkMode = false; applyTheme(); }
            
            // Color Swatch Clicks
            if (rightExpanded && mPos.x > window.getSize().x - rightWidth) {
                for (size_t i = 0; i < mColorSwatches.size(); ++i) {
                    if (mColorSwatches[i].getGlobalBounds().contains(mPos)) {
                        mCurrentNodeColor = mThemeColors[i];
                    }
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (!isClickingOnPanel) {
                for (auto& node : nodes) { if (node.contains(mPos) && node.isVisible) { nodePanel.show(&node); break; } }
            }
            if (menuBtn.isClicked(mPos, true)) { nextScene = SceneType::MAIN_MENU; return; }
        }

        PanelResult pResult = nodePanel.HandleEvent(event, window);
        if (pResult.action != PanelAction::NONE) {
            if (currentFrame < timeline.size() - 1) currentFrame = timeline.size() - 1; 
            if (pResult.action == PanelAction::CHANGE_VALUE) {
                ChangeNodeValue(pHead, pResult.targetUID, pResult.newValue);
                timeline.push_back(TakeSnapshot_SLL(pHead, "Changed Value to " + std::to_string(pResult.newValue)));
            } else if (pResult.action == PanelAction::DELETE_NODE) {
                DeleteNodeByUID(pHead, pResult.targetUID); timeline.push_back(TakeSnapshot_SLL(pHead, "Deleted Node"));
            }
            currentFrame = timeline.size() - 1; UpdateVisualsFromFrame();
            if (pResult.action == PanelAction::CHANGE_VALUE) {
                VisualNode* updated = FindNode(pResult.targetUID, nodes);
                if (updated) nodePanel.show(updated);
            }
        }

        if (event.type == sf::Event::Resized) {
            sf::View stretchedView(sf::FloatRect(0, 0, baseWidth, baseHeight));
            window.setView(stretchedView);
            mWorkspaceBg.setSize({baseWidth, baseHeight}); 
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            auto FastForward = [&]() { if (currentFrame < timeline.size() - 1) currentFrame = timeline.size() - 1; };

            if (buttons[0].isClicked(mPos, true) && !boxes[0].input.empty()) { FastForward(); InsertBack_Visual(pHead, std::stoi(boxes[0].input), timeline); isAutoPlaying = true; boxes[0].clear(); }
            else if (buttons[1].isClicked(mPos, true) && !boxes[0].input.empty()) { FastForward(); InsertFront_Visual(pHead, std::stoi(boxes[0].input), timeline); isAutoPlaying = true; boxes[0].clear(); }
            else if (buttons[5].isClicked(mPos, true) && pHead != nullptr) { FastForward(); DeleteFront_Visual(pHead, timeline); isAutoPlaying = true; }
            else if (buttons[6].isClicked(mPos, true) && pHead != nullptr) { FastForward(); DeleteBack_Visual(pHead, timeline); isAutoPlaying = true; }
            else if (buttons[8].isClicked(mPos, true) && !boxes[0].input.empty()) { FastForward(); pendingSearchUID = Search_Visual(pHead, std::stoi(boxes[0].input), timeline); isAutoPlaying = true; boxes[0].clear(); }
            else if (buttons[11].isClicked(mPos, true) && pHead != nullptr) { FastForward(); DeleteAll(pHead); timeline.push_back(TakeSnapshot_SLL(pHead, "Cleared All Nodes")); currentFrame = timeline.size() - 1; UpdateVisualsFromFrame(); }
            else if (buttons[12].isClicked(mPos, true) && !boxes[1].input.empty()) { 
                FastForward(); std::ifstream file(boxes[1].input);
                if (file.is_open()) {
                    std::string word;
                    while (file >> word) { try { int val = std::stoi(word); if (val >= -99 && val <= 99) InsertBack_Visual(pHead, val, timeline); } catch (...) {} }
                    file.close(); isAutoPlaying = true;
                }
                boxes[1].clear();
            }
            else if (buttons[13].isClicked(mPos, true)) { // CREATE RANDOM logic
                FastForward(); 
                DeleteAll(pHead); 
                timeline.push_back(TakeSnapshot_SLL(pHead, "Cleared All Nodes for Random List")); 
                for (int i = 0; i < 7; ++i) {
                    InsertFront_Visual(pHead, (rand() % 199) - 99, timeline);
                }
                isAutoPlaying = true;
            }
            else if (buttons[2].isClicked(mPos, true)) { isAutoPlaying = false; buttons[3].setText("PLAY"); if (currentFrame > 0) { currentFrame--; UpdateVisualsFromFrame(); } }
            else if (buttons[3].isClicked(mPos, true)) { if (!isAutoPlaying && currentFrame == timeline.size() - 1) currentFrame = 0; isAutoPlaying = !isAutoPlaying; buttons[3].setText(isAutoPlaying ? "PAUSE" : "PLAY"); }
            else if (buttons[4].isClicked(mPos, true)) { isAutoPlaying = false; buttons[3].setText("PLAY"); if (currentFrame < timeline.size() - 1) { currentFrame++; UpdateVisualsFromFrame(); } }
            else if (buttons[7].isClicked(mPos, true)) { int randomVal = (rand() % 199) - 99; boxes[0].input = std::to_string(randomVal); boxes[0].text.setString(boxes[0].input); boxes[0].text.setFillColor(UITheme::Color::TextDark); }
            else if (buttons[9].isClicked(mPos, true)) { isAutoPlaying = false; buttons[3].setText("PLAY"); int target = timeline.size() - 1; for (int i = currentFrame + 1; i < timeline.size(); i++) { if (timeline[i].isKeyStage) { target = i; break; } } if (currentFrame != target) { currentFrame = target; UpdateVisualsFromFrame(); } }
            else if (buttons[10].isClicked(mPos, true)) { isAutoPlaying = false; buttons[3].setText("PLAY"); int target = 0; for (int i = currentFrame - 1; i >= 0; i--) { if (timeline[i].isKeyStage) { target = i; break; } } if (currentFrame != target) { currentFrame = target; UpdateVisualsFromFrame(); } }
        }
    }

    void Update(float deltaTime, sf::RenderWindow& window) override {
        // --- Smooth Panel Transitions ---
        float targetLeft = leftExpanded ? LEFT_PANEL_WIDTH : TAB_WIDTH;
        leftWidth += (targetLeft - leftWidth) * 12.f * deltaTime;
        
        float targetRight = rightExpanded ? RIGHT_PANEL_WIDTH : TAB_WIDTH;
        rightWidth += (targetRight - rightWidth) * 12.f * deltaTime;

        // --- Left Panel Layout Mathematics ---
        float leftBaseX = leftWidth - LEFT_PANEL_WIDTH; 
        float currentY = 20.f;
        
        menuBtn.setPosition({leftBaseX + 30.f + 40.f, currentY + 20.f}); currentY += 50.f;
        boxes[0].setPosition({leftBaseX + 30.f, currentY}); currentY += 50.f;

        buttons[1].setPosition({leftBaseX + 30.f + 52.5f, currentY + 20.f});   // INS FRONT
        buttons[0].setPosition({leftBaseX + 145.f + 52.5f, currentY + 20.f}); currentY += 50.f; // INS BACK
        buttons[5].setPosition({leftBaseX + 30.f + 52.5f, currentY + 20.f});   // DEL FRONT
        buttons[6].setPosition({leftBaseX + 145.f + 52.5f, currentY + 20.f}); currentY += 50.f; // DEL BACK
        buttons[8].setPosition({leftBaseX + 30.f + 52.5f, currentY + 20.f});   // SEARCH
        buttons[7].setPosition({leftBaseX + 145.f + 52.5f, currentY + 20.f}); currentY += 50.f; // RANDOM
        buttons[11].setPosition({leftBaseX + 30.f + 110.f, currentY + 20.f}); currentY += 70.f; // CLEAR ALL
        
        // --- RESTORED: Original Playback Controls Position ---
        buttons[4].setPosition({leftBaseX + 180.f + 35.f, currentY + 20.f});    // NEXT
        buttons[2].setPosition({leftBaseX + 30.f + 35.f, currentY + 20.f});     // PREV
        buttons[3].setPosition({leftBaseX + 105.f + 35.f, currentY + 20.f}); currentY += 50.f; // PLAY/PAUSE
        
        buttons[10].setPosition({leftBaseX + 30.f + 35.f, currentY + 20.f});    // BACK
        buttons[9].setPosition({leftBaseX + 180.f + 35.f, currentY + 20.f}); currentY += 60.f; // SKIP
        // -----------------------------------------------------

        speedCtrl.setPosition({leftBaseX + 30.f, currentY}); currentY += 60.f;
        speedCtrl.update(window);
        boxes[1].setPosition({leftBaseX + 30.f, currentY}); currentY += 50.f;
        buttons[12].setPosition({leftBaseX + 30.f + 110.f, currentY + 20.f}); currentY += 50.f;
        buttons[13].setPosition({leftBaseX + 30.f + 110.f, currentY + 20.f}); // CREATE RANDOM



        // --- Right Panel Layout Mathematics ---
        float rightBaseX = window.getSize().x - rightWidth;
        codePanel.setPosition({rightBaseX + TAB_WIDTH + 15.f, 20.f});
        
        // --- Timeline Slider stays at bottom, scales dynamically ---
        slider.setPosition({leftWidth + 20.f, window.getSize().y - 50.f}, window.getSize().x - leftWidth - rightWidth - 40.f);

        if (nodePanel.isVisible) {
            float npX = rightBaseX - UITheme::Size::PanelDefault.x - 20.f;
            float npY = window.getSize().y - 230.f; 
            nodePanel.setPosition({npX, npY});
        }

        // --- Apply Hovers ---
        sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (auto& btn : buttons) btn.update(mPos); 
        mDarkThemeBtn.update(mPos);
        mLightThemeBtn.update(mPos);
        menuBtn.update(mPos);

        if (isAutoPlaying) {
            if (animationClock.getElapsedTime().asSeconds() >= timeInterval) {
                if (currentFrame < timeline.size() - 1) {
                    currentFrame++; UpdateVisualsFromFrame();
                } else {
                    isAutoPlaying = false; buttons[3].setText("PLAY"); 
                    if (pendingSearchUID != INT_MIN) {
                        VisualNode* foundNode = FindNode(pendingSearchUID, nodes);
                        if (foundNode) nodePanel.show(foundNode); else nodePanel.hide();
                        pendingSearchUID = INT_MIN; 
                    }
                }
                animationClock.restart();
            }
        }

        for (auto& node : nodes) {
            // 1. Check if the timeline snapshot wants this highlighted
            bool animHighlight = false;
            for (const auto& rec : timeline[currentFrame].nodes) {
                if (rec.UID == node.UID) { animHighlight = rec.isHighlighted; break; }
            }
            
            // 2. Check if the user manually clicked it (NodePanel is open)
            bool panelHighlight = (nodePanel.isVisible && nodePanel.targetUID == node.UID);
            
            // Apply true if EITHER condition is met
            node.isHighlighted = (animHighlight || panelHighlight);
            
            node.update(window, deltaTime);
        }
        
        ResolveCollisions(nodes, window, leftWidth + 100.0f, window.getSize().x - rightWidth); 
        
        for (auto& line : lines) {
            VisualNode* s = FindNode(line.start_UID, nodes);
            VisualNode* e = FindNode(line.end_UID, nodes);
            if (s && e) line.update(*s, *e, deltaTime);
        }
    }

    void Draw(sf::RenderWindow& window) override {
        // 0. Draw Environment Background
        window.draw(mWorkspaceBg);

        // 1. Draw Links and Nodes Background Layer
        std::vector<VisualNode> printNode = nodes; std::vector<Connector> printLine = lines;
        sort(printNode.begin(), printNode.end(), [](VisualNode& a, VisualNode& b){return a.UID < b.UID;});
        sort(printLine.begin(), printLine.end(), [](Connector& a, Connector& b){return a.start_UID < b.start_UID;});

        for (auto& line : printLine) line.draw(window);
        for (auto& node : printNode) {
            // Override the node's internal UITheme static color with our custom chosen color 
            // without interrupting the fade in/out animation logic
            sf::Color blend = mCurrentNodeColor;
            blend.a = static_cast<sf::Uint8>(node.currentAlpha);
            node.m_graphic.setFillColor(blend);
            
            node.draw(window);
        }

        // 2. Center Bottom Content
        slider.update(currentFrame, timeline.size() - 1); slider.draw(window, timeline);
        nodePanel.draw(window); 
        codePanel.update(timeline[currentFrame].algorithmName, timeline[currentFrame].activeLine);
        
        // Draw the Snapshot Status Description Text
        if (!timeline.empty() && currentFrame < timeline.size()) {
            mStatusText.setString("Step: " + timeline[currentFrame].description);
            window.draw(mStatusText);
        }

        // 3. Draw Left Panel Architecture
        sf::RectangleShape leftMenu(sf::Vector2f(leftWidth, window.getSize().y));
        leftMenu.setFillColor(mPanelBgColor);
        window.draw(leftMenu);

        sf::RectangleShape leftTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
        leftTab.setFillColor(UITheme::Color::GraphTabBg);
        leftTab.setPosition(leftWidth - TAB_WIDTH, window.getSize().y / 2.f - TAB_HEIGHT / 2.f);
        window.draw(leftTab);

        sf::Text lIcon(leftExpanded ? "<<" : ">>", *fontPtr, 18);
        sf::FloatRect lb = lIcon.getLocalBounds();
        lIcon.setOrigin(lb.left + lb.width/2.f, lb.top + lb.height/2.f);
        lIcon.setPosition(leftWidth - TAB_WIDTH/2.f, window.getSize().y / 2.f - 2.f);
        lIcon.setFillColor(UITheme::Color::NodeOutlineColor);
        window.draw(lIcon);

        // 4. Draw Right Panel Architecture
        float winW = window.getSize().x;
        sf::RectangleShape rightMenu(sf::Vector2f(rightWidth, window.getSize().y));
        rightMenu.setFillColor(mPanelBgColor);
        rightMenu.setPosition(winW - rightWidth, 0);
        window.draw(rightMenu);

        sf::RectangleShape rightTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
        rightTab.setFillColor(UITheme::Color::GraphTabBg);
        rightTab.setPosition(winW - rightWidth, window.getSize().y / 2.f - TAB_HEIGHT / 2.f);
        window.draw(rightTab);

        sf::Text rIcon(rightExpanded ? ">>" : "<<", *fontPtr, 18);
        sf::FloatRect rb = rIcon.getLocalBounds();
        rIcon.setOrigin(rb.left + rb.width/2.f, rb.top + rb.height/2.f);
        rIcon.setPosition(winW - rightWidth + TAB_WIDTH/2.f, window.getSize().y / 2.f - 2.f);
        rIcon.setFillColor(UITheme::Color::NodeHighlightColor);
        window.draw(rIcon);

        // --- Customization Settings on Right Panel ---
        float rightBaseX = winW - rightWidth;
        float startX = rightBaseX + TAB_WIDTH + 15.f; 
        
        if (rightWidth > 180.f) {
            float startY_color = 380.f;
            sf::Text cLabel("Node Fill Color", *fontPtr, 14);
            cLabel.setFillColor(mIsDarkMode ? sf::Color(200, 200, 210) : sf::Color(50, 50, 60));
            cLabel.setPosition({startX, startY_color});
            window.draw(cLabel);

            for (size_t i = 0; i < mColorSwatches.size(); ++i) {
                float x = startX + (i % 3) * 35.f;
                float y = startY_color + 25.f + (i / 3) * 35.f;
                mColorSwatches[i].setPosition({x, y});

                if (mThemeColors[i] == mCurrentNodeColor) {
                    mColorSwatches[i].setOutlineColor(sf::Color::Yellow);
                    mColorSwatches[i].setOutlineThickness(2.5f);
                } else {
                    mColorSwatches[i].setOutlineColor(sf::Color(100, 100, 100));
                    mColorSwatches[i].setOutlineThickness(1.5f);
                }
                window.draw(mColorSwatches[i]);
            }

            float startY_theme = 480.f;
            sf::Text tLabel("UI Theme", *fontPtr, 14);
            tLabel.setFillColor(mIsDarkMode ? sf::Color(200, 200, 210) : sf::Color(50, 50, 60));
            tLabel.setPosition({startX, startY_theme});
            window.draw(tLabel);

            mDarkThemeBtn.setPosition({startX + 40.f, startY_theme + 45.f});
            mLightThemeBtn.setPosition({startX + 130.f, startY_theme + 45.f});
            window.draw(mDarkThemeBtn);
            window.draw(mLightThemeBtn);
        }

        // 5. Draw Content over Panels
        for (auto& box : boxes) box.draw(window); 
        for (auto& btn : buttons) window.draw(btn); 
        codePanel.draw(window); 
        speedCtrl.draw(window); 
        window.draw(menuBtn);
    }
};
}