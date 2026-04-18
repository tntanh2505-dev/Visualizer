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
    float horizontalSpacing = 140.f; float verticalSpacing = 120.f; int nodesPerRow = 8;             
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
    Button menuBtn;
    
    int currentFrame; bool isAutoPlaying; int pendingSearchUID;
    sf::Clock animationClock; float timeInterval; float dt; sf::Font* fontPtr;

    std::vector<Button> buttons; std::vector<TextBox> boxes;
    NodePanel nodePanel; CodePanel codePanel; TimelineSlider slider; SpeedController speedCtrl;

    // --- Sliding Panel Layout Properties ---
    float leftWidth;
    float rightWidth;
    bool leftExpanded;
    bool rightExpanded;
    const float LEFT_PANEL_WIDTH = 260.f;
    const float RIGHT_PANEL_WIDTH = 420.f;
    const float TAB_WIDTH = 35.f;
    const float TAB_HEIGHT = 50.f;

    void UpdateVisualsFromFrame() {
        ApplySLLLayout(timeline[currentFrame], 200.f, 200.f);
        for (const auto& record : timeline[currentFrame].nodes) {
            VisualNode* vn = FindNode(record.UID, nodes);
            if (vn) vn->index = record.index; 
        }
        SyncToFrame(timeline[currentFrame], nodes, lines, *fontPtr);
    }

public:
    LinkedListScene(sf::Font& font, float windowWidth, float windowHeight) 
    : nodePanel(font), codePanel(font), slider({200.f, 750.f}, 1000.f), speedCtrl(font, &timeInterval), menuBtn("MENU", font, {20.f, 20.f}, 80, 40)
    {   
        baseWidth = windowWidth;
        baseHeight = windowHeight;
        fontPtr = &font; pHead = nullptr; currentFrame = 0; isAutoPlaying = true; pendingSearchUID = INT_MIN; timeInterval = 0.005f; dt = 0.001f;

        // Init panel sliding properties
        leftWidth = TAB_WIDTH;
        rightWidth = TAB_WIDTH;
        leftExpanded = false;
        rightExpanded = false;

        std::map<std::string, std::vector<std::string>> llSnippets;
        llSnippets["InsertFront"] = { "SLLNode* newNode = new SLLNode(x);", "newNode->next = head;", "head = newNode;" };
        llSnippets["InsertBack"] = { "if (head == NULL) { head = new SLLNode(x); return; }", "SLLNode* cur = head;", "while (cur->next != NULL) cur = cur->next;", "cur->next = new SLLNode(x);" };
        llSnippets["Search"] = { "SLLNode* cur = head;", "while (cur != NULL) {", "    if (cur->val == targetValue) return cur;", "    cur = cur->next;", "}", "return NULL;" };
        llSnippets["DeleteBack"] = { "if (head == NULL) return;", "if (head->next == NULL) { delete head; return; }", "SLLNode* cur = head;", "while (cur->next->next != NULL) cur = cur->next;", "delete cur->next; cur->next = NULL;" };
        llSnippets["DeleteFront"] = { "if (head == NULL) return;", "SLLNode* temp = head;", "head = head->next;", "delete temp;" };
        codePanel.loadSnippets(llSnippets);

        // Size optimized to fit in 260px Left Panel (105 + 10 gap + 105 = 220 total span)
        boxes.emplace_back(TextBox(font, {0, 0}, 220, 40)); boxes[0].placeholder = "Enter value..."; boxes[0].allowNegative = true;
        boxes.emplace_back(TextBox(font, {0, 0}, 220, 40)); boxes[1].allowAllChars = true; boxes[1].placeholder = "Paste path (Ctrl+V)...";

        buttons.emplace_back(Button("INS BACK", font, {0, 0}, 105, 40));  // 0
        buttons.emplace_back(Button("INS FRONT", font, {0, 0}, 105, 40)); // 1
        buttons.emplace_back(Button("PREV", font, {0, 0}, 70, 40));       // 2
        buttons.emplace_back(Button("PAUSE", font, {0, 0}, 70, 40));      // 3
        buttons.emplace_back(Button("NEXT", font, {0, 0}, 70, 40));       // 4
        buttons.emplace_back(Button("DEL FRONT", font, {0, 0}, 105, 40)); // 5
        buttons.emplace_back(Button("DEL BACK", font, {0, 0}, 105, 40));  // 6
        buttons.emplace_back(Button("RANDOM", font, {0, 0}, 105, 40));    // 7
        buttons.emplace_back(Button("SEARCH", font, {0, 0}, 105, 40));    // 8
        buttons.emplace_back(Button("SKIP", font, {0, 0}, 70, 40));       // 9
        buttons.emplace_back(Button("BACK", font, {0, 0}, 70, 40));       // 10
        buttons.emplace_back(Button("CLEAR ALL", font, {0, 0}, 220, 40)); // 11
        buttons[11].shape.setFillColor(UITheme::Color::ButtonDanger); 
        buttons.emplace_back(Button("ADD FILE", font, {0, 0}, 220, 40));  // 12     

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

        // Determine if mouse is over expanded UI panels to block background node dragging
        bool isClickingOnPanel = (mPos.x < leftWidth) || (mPos.x > window.getSize().x - rightWidth);

        if (!isClickingOnPanel) {
            for(VisualNode& node : nodes) node.HandleEvent(event, window);
        }

        for(TextBox& box : boxes) box.HandleEvent_IB(event, window);
        speedCtrl.HandleEvent(event, window);

        int previousFrame = currentFrame;
        slider.HandleEvent(event, window, currentFrame, timeline.size() - 1, timeline, nodes, lines, *fontPtr, isAutoPlaying, buttons[3]);
        if (currentFrame != previousFrame) { UpdateVisualsFromFrame(); }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (!isClickingOnPanel) {
                for (auto& node : nodes) { if (node.contains(mPos) && node.isVisible) { nodePanel.show(&node); break; } }
            }
            if (menuBtn.isClicked(mPos)) { nextScene = SceneType::MAIN_MENU; return; }
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
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            auto FastForward = [&]() { if (currentFrame < timeline.size() - 1) currentFrame = timeline.size() - 1; };

            if (buttons[0].isClicked(mPos) && !boxes[0].input.empty()) { FastForward(); InsertBack_Visual(pHead, std::stoi(boxes[0].input), timeline); isAutoPlaying = true; boxes[0].clear(); }
            else if (buttons[1].isClicked(mPos) && !boxes[0].input.empty()) { FastForward(); InsertFront_Visual(pHead, std::stoi(boxes[0].input), timeline); isAutoPlaying = true; boxes[0].clear(); }
            else if (buttons[5].isClicked(mPos) && pHead != nullptr) { FastForward(); DeleteFront_Visual(pHead, timeline); isAutoPlaying = true; }
            else if (buttons[6].isClicked(mPos) && pHead != nullptr) { FastForward(); DeleteBack_Visual(pHead, timeline); isAutoPlaying = true; }
            else if (buttons[8].isClicked(mPos) && !boxes[0].input.empty()) { FastForward(); pendingSearchUID = Search_Visual(pHead, std::stoi(boxes[0].input), timeline); isAutoPlaying = true; boxes[0].clear(); }
            else if (buttons[11].isClicked(mPos) && pHead != nullptr) { FastForward(); DeleteAll(pHead); timeline.push_back(TakeSnapshot_SLL(pHead, "Cleared All Nodes")); currentFrame = timeline.size() - 1; UpdateVisualsFromFrame(); }
            else if (buttons[12].isClicked(mPos) && !boxes[1].input.empty()) { 
                FastForward(); std::ifstream file(boxes[1].input);
                if (file.is_open()) {
                    std::string word;
                    while (file >> word) { try { int val = std::stoi(word); if (val >= -99 && val <= 99) InsertBack_Visual(pHead, val, timeline); } catch (...) {} }
                    file.close(); isAutoPlaying = true;
                }
                boxes[1].clear();
            }
            else if (buttons[2].isClicked(mPos)) { isAutoPlaying = false; buttons[3].setText("PLAY"); if (currentFrame > 0) { currentFrame--; UpdateVisualsFromFrame(); } }
            else if (buttons[3].isClicked(mPos)) { if (!isAutoPlaying && currentFrame == timeline.size() - 1) currentFrame = 0; isAutoPlaying = !isAutoPlaying; buttons[3].setText(isAutoPlaying ? "PAUSE" : "PLAY"); }
            else if (buttons[4].isClicked(mPos)) { isAutoPlaying = false; buttons[3].setText("PLAY"); if (currentFrame < timeline.size() - 1) { currentFrame++; UpdateVisualsFromFrame(); } }
            else if (buttons[7].isClicked(mPos)) { int randomVal = (rand() % 199) - 99; boxes[0].input = std::to_string(randomVal); boxes[0].text.setString(boxes[0].input); boxes[0].text.setFillColor(UITheme::Color::TextDark); }
            else if (buttons[9].isClicked(mPos)) { isAutoPlaying = false; buttons[3].setText("PLAY"); int target = timeline.size() - 1; for (int i = currentFrame + 1; i < timeline.size(); i++) { if (timeline[i].isKeyStage) { target = i; break; } } if (currentFrame != target) { currentFrame = target; UpdateVisualsFromFrame(); } }
            else if (buttons[10].isClicked(mPos)) { isAutoPlaying = false; buttons[3].setText("PLAY"); int target = 0; for (int i = currentFrame - 1; i >= 0; i--) { if (timeline[i].isKeyStage) { target = i; break; } } if (currentFrame != target) { currentFrame = target; UpdateVisualsFromFrame(); } }
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
        
        menuBtn.setPosition({leftBaseX + 20.f, currentY}); currentY += 50.f;
        boxes[0].setPosition({leftBaseX + 20.f, currentY}); currentY += 50.f;
        buttons[1].setPosition({leftBaseX + 20.f, currentY});  // INS FRONT
        buttons[0].setPosition({leftBaseX + 135.f, currentY}); currentY += 50.f; // INS BACK
        buttons[5].setPosition({leftBaseX + 20.f, currentY});  // DEL FRONT
        buttons[6].setPosition({leftBaseX + 135.f, currentY}); currentY += 50.f; // DEL BACK
        buttons[8].setPosition({leftBaseX + 20.f, currentY});  // SEARCH
        buttons[7].setPosition({leftBaseX + 135.f, currentY}); currentY += 50.f; // RANDOM
        buttons[11].setPosition({leftBaseX + 20.f, currentY}); currentY += 70.f; // CLEAR ALL

        // Play Controls Area
        buttons[10].setPosition({leftBaseX + 20.f, currentY}); // BACK (Keyframe)
        buttons[2].setPosition({leftBaseX + 95.f, currentY});  // PREV
        buttons[3].setPosition({leftBaseX + 170.f, currentY}); currentY += 50.f; // PLAY/PAUSE
        buttons[4].setPosition({leftBaseX + 20.f, currentY});  // NEXT
        buttons[9].setPosition({leftBaseX + 95.f, currentY}); currentY += 60.f;  // SKIP (Keyframe)

        speedCtrl.setPosition({leftBaseX + 20.f, currentY}); currentY += 60.f;
        boxes[1].setPosition({leftBaseX + 20.f, currentY}); currentY += 50.f;
        buttons[12].setPosition({leftBaseX + 20.f, currentY}); // ADD FILE

        // --- Right Panel Layout Mathematics ---
        float rightBaseX = window.getSize().x - rightWidth;
        codePanel.setPosition({rightBaseX + TAB_WIDTH + 10.f, 20.f});
        
        // Node Panel Floats Freely, Timeline Slider stays at bottom
        slider.setPosition({leftWidth + 20.f, window.getSize().y - 50.f}, window.getSize().x - leftWidth - rightWidth - 40.f);

        // --- Apply Hovers ---
        sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (auto& btn : buttons) btn.handleHover(mPos);
        menuBtn.handleHover(mPos);
        speedCtrl.minusBtn.handleHover(mPos);
        speedCtrl.plusBtn.handleHover(mPos);

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
        for (auto& node : nodes) node.update(window, deltaTime);
        
        // Nodes will smoothly bounce off the expanded panel edges!
        ResolveCollisions(nodes, window, leftWidth, window.getSize().x - rightWidth); 
        
        for (auto& line : lines) {
            VisualNode* s = FindNode(line.start_UID, nodes);
            VisualNode* e = FindNode(line.end_UID, nodes);
            if (s && e) line.update(*s, *e, deltaTime);
        }
    }

    void Draw(sf::RenderWindow& window) override {
        // 1. Draw Links and Nodes Background Layer
        std::vector<VisualNode> printNode = nodes; std::vector<Connector> printLine = lines;
        sort(printNode.begin(), printNode.end(), [](VisualNode& a, VisualNode& b){return a.UID < b.UID;});
        sort(printLine.begin(), printLine.end(), [](Connector& a, Connector& b){return a.start_UID < b.start_UID;});

        for (auto& line : printLine) line.draw(window);
        for (auto& node : printNode) node.draw(window);

        // 2. Center Bottom Content
        slider.update(currentFrame, timeline.size() - 1); slider.draw(window, timeline);
        nodePanel.draw(window); 
        codePanel.update(timeline[currentFrame].algorithmName, timeline[currentFrame].activeLine);

        // 3. Draw Left Panel Architecture
        sf::RectangleShape leftMenu(sf::Vector2f(leftWidth, window.getSize().y));
        leftMenu.setFillColor(UITheme::Color::GraphPanelBg);
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
        rightMenu.setFillColor(UITheme::Color::GraphPanelBg);
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

        // 5. Draw Content over Panels (Positions are tied to leftBaseX and rightBaseX, so they hide automatically)
        for (auto& box : boxes) box.draw(window); 
        for (auto& btn : buttons) btn.draw(window);
        codePanel.draw(window); 
        speedCtrl.draw(window); 
        menuBtn.draw(window);
    }
};
}