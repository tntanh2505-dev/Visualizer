#include "DSA-Visualization/ui/DijkstraScreen.hpp"
#include <cmath>

const float NODE_RADIUS = 25.f;
const float LEFT_PANEL_WIDTH = 250.f;
const float RIGHT_PANEL_WIDTH = 300.f;
const float TAB_WIDTH = 35.f;
const float TAB_HEIGHT = 50.f;
const std::vector<std::string> pseudoCode = {
    "function Dijkstra(Graph, source):",        // 0
    "    dist[source] = 0",                     // 1
    "    for each vertex v in Graph",           // 2
    "        add v to Q",                       // 3
    "        if v != source",                   // 4
    "            dist[v] = INFINITY",           // 5
    "",                                         // 6
    "    while Q is not empty",                 // 7
    "        u = vertex in Q with min dist",    // 8
    "        remove u from Q",                  // 9
    "",                                         // 10
    "        for each neighbor v of u",         // 11
    "           w = weight(u, v)",              // 12
    "           if dist[v] > dist[u] + w",      // 13
    "              dist[v] = dist[u] + w",      // 14
    "",                                         // 15
    "    return dist[]"                         // 16
};

int DijkstraScreen::run(sf::RenderWindow &window, sf::Font &font) {
    initialization();
    for (int i = 0; i < 5; ++i) {
        button[i] = std::make_unique<ModernButton>("", font, sf::Vector2f(160.f, 40.f), 5.f);
        button[i]->setPosition(100.f, 50.f + (i * 50.f));
    }

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2i mPos = sf::Mouse::getPosition(window);
        updateAnimation(deltaClock.restart().asSeconds());

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) return -1;

            // Xử lý nhập liệu bàn phím khi đang sửa Label
            if ((editingNode != -1 || editingEdge != -1) && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 13) { // Enter để hoàn tất
                    if (editingNode != -1) {
                        nodes[editingNode].label = inputBuffer;
                    } else {
                        // Chuyển buffer thành số, mặc định là 1 nếu rỗng hoặc lỗi
                        try {
                            edges[editingEdge].weight = inputBuffer.empty() ? 1 : std::stoi(inputBuffer);
                        } catch (...) { edges[editingEdge].weight = 1; }
                    }
                    editingNode = -1;
                    editingEdge = -1;
                    inputBuffer = "";
                }
                else if (event.text.unicode == 8) { // Backspace
                    if (!inputBuffer.empty()) inputBuffer.pop_back();
                }
                else if (event.text.unicode < 128 && inputBuffer.length() < 5) { // Chỉ nhận ASCII và giới hạn 5 ký tự
                    inputBuffer += static_cast<char>(event.text.unicode);
                }
            }

            handleInput(window, event, mPos);
        }

        if (returnFlag) return 0;

        if (isAutoMode && !isEditMode && sourceNode != -1 && tickClock.getElapsedTime().asSeconds() > 0.8f) {
            if (visitingList.empty()) {
                visitingNode = -1;
                processingNode = algorithm.stage(nodes);
                if (processingNode == -1) {
                    isAutoMode = false;
                    currentLine = {16, 16};
                } else {
                    visitingList = algorithm.getAdjacent(processingNode);
                    currentLine = {7, 9};
                }
            } else {
                visitingNode = visitingList.back();
                visitingList.pop_back();
                currentLine = {11, 14};
            }
            tickClock.restart();
        }

        window.clear(sf::Color(20, 20, 25));
        drawGraph(window, font);

        // Rubber-band effect
        if (isEditMode && !isDeleting && selectNode != -1) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(nodes[selectNode].x, nodes[selectNode].y), sf::Color(255, 255, 255, 100)),
                sf::Vertex(window.mapPixelToCoords(mPos), sf::Color(255, 255, 255, 40))
            };
            window.draw(line, 2, sf::Lines);
        }

        drawUI(window, font, mPos);
        window.display();
    }
    return -1;
}

void DijkstraScreen::initialization() {
    visitingList.clear();
    dist.clear();
    nodes.clear();
    edges.clear();

    returnFlag = false;
    isEditMode = true;
    isAutoMode = false;
    isDeleting = false;
    isDirected = false;

    sourceNode = -1;
    selectNode = -1;
    editingNode = -1;
    editingEdge = -1;
    draggingNode = -1;
    visitingNode = -1;
    processingNode = -1;
    inputBuffer.clear();

    leftWidth = 0.f;
    rightWidth = 0.f;
    leftExpanded = true;
    rightExpanded = true;
    currentLine = {0, 0};
    activeTab = TabState::Info;
}

void DijkstraScreen::handleInput(sf::RenderWindow &window, sf::Event &event, sf::Vector2i mPos) {
    sf::Vector2f worldPos = window.mapPixelToCoords(mPos);
    float winW = static_cast<float>(window.getSize().x);
    float winH = static_cast<float>(window.getSize().y);
    float centerY = winH / 2.f;


    if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window.setView(sf::View(visibleArea));
    }

    // Khi thả chuột trái -> Ngừng trạng thái di chuyển node
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        draggingNode = -1;
    }

    if ((editingNode != -1 || editingEdge != -1) && event.type == sf::Event::MouseButtonPressed) {
        editingNode = -1;
        editingEdge = -1;
        inputBuffer = "";
        return;
    }

    for (int i = 0; i < 5; ++i)
        button[i]->update(worldPos);

    // --- 1. XỬ LÝ UI PANEL (Ưu tiên các thao tác trên bảng điều khiển) ---
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        // --- LEFT TAB ---
        bool mouseInLeftIcon = (mPos.x >= leftWidth - TAB_WIDTH && mPos.x <= leftWidth &&
                                mPos.y >= centerY - TAB_HEIGHT / 2.f && mPos.y <= centerY + TAB_HEIGHT / 2.f);
        if (mouseInLeftIcon) {
            leftExpanded = !leftExpanded;
            return;
        }
        if (leftExpanded && mPos.x < leftWidth - TAB_WIDTH) {
            selectNode = -1;
            if (button[0]->isClicked(worldPos, true)) { // Nút MODE
                isEditMode = !isEditMode;
                sourceNode = -1;
                currentLine = {0, 0};
                algorithm.init(nodes, edges, isDirected);
            }
            else if (button[1]->isClicked(worldPos, true)) { // Nút INSERT/DELETE // AUTO
                if (isEditMode) {
                    isDeleting = !isDeleting;
                } else {
                    isAutoMode = !isAutoMode;
                }
            }
            else if (button[2]->isClicked(worldPos, true)) { // Nút DIRECTED/UNDIRECTED // FINISH
                if (isEditMode) {
                    isDirected = !isDirected;
                } else {
                    finishFlag = true;
                }
            }
            else if (button[3]->isClicked(worldPos, true)) { // Nút CLEAR // RESET
                if (isEditMode) {
                    nodes.clear();
                    edges.clear();
                } else {
                    sourceNode = -1;
                    algorithm.init(nodes, edges, isDirected);
                }
            }
            else if (button[4]->isClicked(worldPos, true)) { // Nút RETURN
                returnFlag = true;
            }
            return;
        }

        // --- RIGHT TAB ---
        bool mouseInRightIcon = (mPos.x >= winW - rightWidth && mPos.x <= winW - rightWidth + TAB_WIDTH &&
                                 mPos.y >= centerY - TAB_HEIGHT / 2.f && mPos.y <= centerY + TAB_HEIGHT / 2.f);
        if (mouseInRightIcon) {
            rightExpanded = !rightExpanded;
            return;
        }
        if (rightExpanded && mPos.y >= 10 && mPos.y <= 40) {
            float panelStart = winW - rightWidth + TAB_WIDTH;
            float tabAreaWidth = RIGHT_PANEL_WIDTH - TAB_WIDTH;
            float tabW = tabAreaWidth / 3.f;

            for (int i = 0; i < 3; ++i) {
                if (mPos.x >= panelStart + i * tabW && mPos.x <= panelStart + (i+1) * tabW) {
                    activeTab = static_cast<TabState>(i);
                    return;
                }
            }
        }
    }

    if (mPos.x < leftWidth) return;
    if (mPos.x > winW - rightWidth) return;

    // --- 2. XÁC ĐỊNH NODE DƯỚI CON TRỎ CHUỘT ---
    int hoveredNode = -1;
    for (int i = 0; i < (int)nodes.size(); ++i) {
        if (std::hypot(nodes[i].x - worldPos.x, nodes[i].y - worldPos.y) < NODE_RADIUS) {
            hoveredNode = i;
            break;
        }
    }

    // --- 3. XỬ LÝ CHUỘT TRÁI (TẠO, NỐI, DI CHUYỂN, XÓA) ---
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (isEditMode) {
            fill(dist.begin(), dist.end(), INF);
            if (isDeleting) {
                // CHẾ ĐỘ XÓA
                if (hoveredNode != -1) {
                    int id = hoveredNode;
                    // Xóa các cạnh liên quan
                    edges.erase(std::remove_if(edges.begin(), edges.end(), [&](const Edge& e) {
                        return e.from == id || e.to == id;
                    }), edges.end());
                    // Cập nhật lại index cho các cạnh còn lại
                    for (auto& e : edges) {
                        if (e.from > id) e.from--;
                        if (e.to > id) e.to--;
                    }
                    nodes.erase(nodes.begin() + id);
                    sourceNode = -1;
                    selectNode = -1;
                } else {
                    edges.erase(std::remove_if(edges.begin(), edges.end(), [&](const Edge& e) {
                        sf::Vector2f A(nodes[e.from].x, nodes[e.from].y);
                        sf::Vector2f B(nodes[e.to].x, nodes[e.to].y);
                        return isSegmentHovering(worldPos, A, B);
                    }), edges.end());
                }
            } else {
                // CHẾ ĐỘ THÊM / NỐI
                if (hoveredNode == -1) {
                    if (isPosValid(worldPos, winW)) {
                        nodes.emplace_back(Node(std::to_string(nodes.size()), worldPos.x, worldPos.y));
                        selectNode = -1;
                    }
                } else {
                    draggingNode = hoveredNode;
                    if (selectNode == -1) selectNode = hoveredNode;
                    else {
                        if (selectNode != hoveredNode) edges.emplace_back(Edge(selectNode, hoveredNode, 1));
                        selectNode = -1;
                    }
                }
            }
        } else {
            // CHẾ ĐỘ RUN
            if (sourceNode == -1) {
                if (hoveredNode != -1) {
                    processingNode = -1;
                    sourceNode = hoveredNode;
                    algorithm.init(nodes, edges, isDirected, sourceNode);
                    currentLine = {1, 5};
                }
            } else if (mPos.x > leftWidth && mPos.x < winW - rightWidth) {
                if (visitingList.empty()) {
                    visitingNode = -1;
                    processingNode = algorithm.stage(nodes);
                    if (processingNode == -1) {
                        isAutoMode = false;
                        currentLine = {16, 16};
                    } else {
                        visitingList = algorithm.getAdjacent(processingNode);
                        currentLine = {7, 9};
                    }
                } else {
                    visitingNode = visitingList.back();
                    visitingList.pop_back();
                    currentLine = {11, 14};
                }
            }
        }
    }
 
    // --- 4. XỬ LÝ CHUỘT PHẢI (BẮT ĐẦU SỬA NỘI DUNG) ---
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
        if (!isEditMode)
            return;
        if (hoveredNode != -1) {
            editingNode = hoveredNode;
            inputBuffer = nodes[editingNode].label;
            selectNode = -1;
            editingEdge = -1;
        } else {
            for (int i = 0; i < (int)edges.size(); ++i) {
                sf::Vector2f A(nodes[edges[i].from].x, nodes[edges[i].from].y);
                sf::Vector2f B(nodes[edges[i].to].x, nodes[edges[i].to].y);
                
                if (isSegmentHovering(worldPos, A, B)) {
                    editingEdge = i;
                    editingNode = -1; // Tắt sửa node nếu đang sửa cạnh
                    inputBuffer = std::to_string(edges[i].weight);
                    break;
                }
            }
        }
    }

    // --- 5. LOGIC DI CHUYỂN NODE & VẬT LÝ ĐẨY NHAU ---
    if (event.type == sf::Event::MouseMoved && draggingNode != -1) {
        selectNode = -1;

        nodes[draggingNode].x = worldPos.x;
        nodes[draggingNode].y = worldPos.y;

        for (int i = 0; i < (int)nodes.size(); ++i) {
            if (i == draggingNode) continue;
            
            float dx = nodes[draggingNode].x - nodes[i].x;
            float dy = nodes[draggingNode].y - nodes[i].y;
            float dist = std::hypot(dx, dy);
            float minSafe = NODE_RADIUS * 2.5f;

            if (dist < minSafe && dist > 0.01f) {
                float overlap = minSafe - dist;
                nodes[draggingNode].x += (dx / dist) * overlap;
                nodes[draggingNode].y += (dy / dist) * overlap;
            }
        }

        if (nodes[draggingNode].x < leftWidth + NODE_RADIUS) {
            nodes[draggingNode].x = leftWidth + NODE_RADIUS;
        }
    }
}

bool DijkstraScreen::isSegmentHovering(sf::Vector2f pos, sf::Vector2f A, sf::Vector2f B) {
    float l2 = std::pow(A.x - B.x, 2) + std::pow(A.y - B.y, 2);
    if (l2 == 0.0) return std::hypot(pos.x - A.x, pos.y - A.y);
    float t = std::max(0.f, std::min(1.f, ((pos.x - A.x) * (B.x - A.x) + (pos.y - A.y) * (B.y - A.y)) / l2));
    sf::Vector2f projection = A + t * (B - A);
    return std::hypot(pos.x - projection.x, pos.y - projection.y) < 8.f;
}

bool DijkstraScreen::isPosValid(sf::Vector2f pos, float winW, int ignoreNode) {
    if (pos.x < leftWidth + NODE_RADIUS) return false;
    if (pos.x > winW - rightWidth - NODE_RADIUS) return false;
    for (int i = 0; i < (int)nodes.size(); ++i) {
        if (i == ignoreNode) continue;
        float d = std::hypot(nodes[i].x - pos.x, nodes[i].y - pos.y);
        if (d < NODE_RADIUS * 2.5f) return false;
    }
    return true;
}

void DijkstraScreen::updateAnimation(float dt) {
    float targetLeft = leftExpanded ? LEFT_PANEL_WIDTH : TAB_WIDTH; 
    leftWidth += (targetLeft - leftWidth) * 12.f * dt;
    float targetRight = rightExpanded ? RIGHT_PANEL_WIDTH : TAB_WIDTH;
    rightWidth += (targetRight - rightWidth) * 12.f * dt;
}

void DijkstraScreen::drawGraph(sf::RenderWindow &window, sf::Font &font) {
    sf::Vector2i mPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mPos);

    // Xác định node đang hover (để vẽ hiệu ứng)
    int hoveredNode = -1;
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (std::hypot(nodes[i].x - worldPos.x, nodes[i].y - worldPos.y) < NODE_RADIUS) {
            hoveredNode = i;
            break;
        }
    }

    //  Draw edges
    for (size_t i = 0; i < edges.size(); ++i) {
        auto [from, to, weight] = edges[i];
        sf::Vector2f A(nodes[from].x, nodes[from].y);
        sf::Vector2f B(nodes[to].x, nodes[to].y);

        sf::Vector2f direction = B - A;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float angle = std::atan2(direction.y, direction.x) * 180.f / M_PI;
        float thickness = 3.f;

        sf::RectangleShape line(sf::Vector2f(length, thickness));
        line.setPosition(A);
        line.setRotation(angle);
        line.setOrigin(0, thickness / 2.f);
        // Hiệu ứng hover cạnh khi ở chế độ Delete
        if (isEditMode && isSegmentHovering(worldPos, A, B) && hoveredNode == -1)
            line.setFillColor(sf::Color::White);
        else if (from == processingNode && to == visitingNode ||
                (from == visitingNode && to == processingNode && !isDirected))
            line.setFillColor(sf::Color::Yellow);
        else
            line.setFillColor(sf::Color(100, 100, 100));

        window.draw(line);
        
        // --- VẼ HÌNH TAM GIÁC (MŨI TÊN) Ở CUỐI ĐƯỜNG NỐI ---
        // 1. Tính toán vector hướng và độ dài

        if (isDirected && length > 0) {
            sf::Vector2f unitDir = direction / length; // Vector đơn vị
            // 2. Tạo hình tam giác
            float arrowSize = 12.f; 
            sf::ConvexShape arrow;
            arrow.setPointCount(3);
            // Đỉnh nhọn hướng về phía trước (trục X)
            arrow.setPoint(0, sf::Vector2f(0, 0));                          // Đỉnh nhọn
            arrow.setPoint(1, sf::Vector2f(-arrowSize, -arrowSize / 1.8f)); // Cánh trên
            arrow.setPoint(2, sf::Vector2f(-arrowSize, arrowSize / 1.8f));  // Cánh dưới
            // 3. Vị trí: Tiếp xúc ngay mép Node đích
            // Vị trí = Tâm Node B - (hướng * bán kính Node)
            sf::Vector2f arrowPos = B - unitDir * NODE_RADIUS;
            arrow.setPosition(arrowPos);
            // 4. Xoay tam giác theo hướng của cạnh
            arrow.setRotation(angle);

            // 5. Màu sắc (Đổi màu nếu cạnh đang được hover hoặc trong đường đi)
            if (isEditMode && isSegmentHovering(worldPos, A, B) && hoveredNode == -1)
                arrow.setFillColor(sf::Color::White);
            else if (from == processingNode && to == visitingNode)
                arrow.setFillColor(sf::Color::Yellow);
            else
                arrow.setFillColor(sf::Color(100, 100, 100));

            window.draw(arrow);
        }

        //  Label
        auto getLabelPosition = [](sf::Vector2f A, sf::Vector2f B, float offset = 12.f) -> sf::Vector2f {
            sf::Vector2f midPoint = (A + B) / 2.f;
            sf::Vector2f direction = B - A;
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            if (length < 1.f) return midPoint; // Tránh chia cho 0

            // Tính vector pháp tuyến (vuông góc)
            sf::Vector2f normal(-direction.y / length, direction.x / length);
            
            // Đảm bảo vector pháp tuyến luôn hướng lên trên màn hình (y âm) để nhất quán
            if (normal.y > 0) normal = -normal;

            return midPoint + normal * offset;
        };

        sf::Vector2f labelPos = getLabelPosition(A, B, 12.f);

        if (editingEdge == i) {
            // Vẽ nền cho ô nhập trọng số
            sf::RectangleShape box(sf::Vector2f(40, 20));
            box.setOrigin(20, 10);
            box.setPosition(labelPos);
            box.setFillColor(sf::Color::White);
            window.draw(box);

            std::string cursor = "";
            if (std::fmod(tickClock.getElapsedTime().asSeconds(), 1.0f) < 0.5f)
                cursor = "|";
            sf::Text itxt(inputBuffer + cursor, font, 16);
            itxt.setFillColor(sf::Color::Black);
            sf::FloatRect ib = itxt.getLocalBounds();
            itxt.setOrigin(ib.left + ib.width/2.f, ib.top + ib.height/2.f);
            itxt.setPosition(labelPos);
            window.draw(itxt);
        } else {
            // Tạo văn bản trọng số
            sf::Text wText(std::to_string(weight), font, 16);
            wText.setFillColor(sf::Color::White); // MÀU TRẮNG PURE
            wText.setOutlineColor(sf::Color(20, 20, 25, 150)); // Thêm viền mờ màu nền để tăng tương phản
            wText.setOutlineThickness(1.5f); // Viền giúp chữ nổi trên các cạnh khác nếu bị đè

            // Căn giữa văn bản tại vị trí đã tính
            sf::FloatRect b = wText.getLocalBounds();
            wText.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
            wText.setPosition(labelPos);
            
            window.draw(wText);
        }
    }

    //  Draw nodes
    for (size_t i = 0; i < nodes.size(); ++i) {
        sf::CircleShape shape(NODE_RADIUS);
        shape.setOrigin(NODE_RADIUS, NODE_RADIUS);
        shape.setPosition(nodes[i].x, nodes[i].y);
        shape.setFillColor(sf::Color(50, 50, 50)); 
        shape.setOutlineThickness(-3.f);
        shape.setOutlineColor(sf::Color(100, 100, 100));

        if (i == hoveredNode) {
            shape.setOutlineThickness(-5.f);
            shape.setOutlineColor(sf::Color::White);
        }
        if (i == selectNode) {
            shape.setOutlineThickness(-5.f); 
            shape.setOutlineColor(sf::Color::Magenta);
        }
      
        if (!isEditMode) {
            if (nodes[i].isProcessed)
                shape.setOutlineColor(sf::Color::Blue);
            if (i == sourceNode)
                shape.setOutlineColor(sf::Color::Green);
            if (i == visitingNode)
                shape.setOutlineColor(sf::Color::Yellow);
            if (i == processingNode)
                shape.setOutlineColor(sf::Color::Red);
        }

        window.draw(shape);

        // Hiển thị nhãn hoặc ô nhập liệu
        if (i == editingNode) {
            // Vẽ khung nền cho ô nhập liệu
            sf::RectangleShape inputBg(sf::Vector2f(60, 25));
            inputBg.setOrigin(30, 12.5f);
            inputBg.setPosition(nodes[i].x, nodes[i].y);
            inputBg.setFillColor(sf::Color::White);
            window.draw(inputBg);

            std::string cursor = "";
            if (std::fmod(tickClock.getElapsedTime().asSeconds(), 1.0f) < 0.5f)
                cursor = "|";
            sf::Text txt(inputBuffer + cursor, font, 18);
            txt.setFillColor(sf::Color::Black);
            sf::FloatRect b = txt.getLocalBounds();
            txt.setOrigin(b.left + b.width/2.f, b.top + b.height/2.f);
            txt.setPosition(nodes[i].x, nodes[i].y);
            window.draw(txt);
        } else {
            sf::Text txt(nodes[i].label, font, 18);
            sf::FloatRect b = txt.getLocalBounds();
            txt.setOrigin(b.left + b.width/2.f, b.top + b.height/2.f);
            txt.setPosition(nodes[i].x, nodes[i].y);
            window.draw(txt);
        }
    }
}

void DijkstraScreen::drawUI(sf::RenderWindow &window, sf::Font &font, sf::Vector2i mPos) {
    float winW = static_cast<float>(window.getSize().x);
    float winH = static_cast<float>(window.getSize().y);
    float centerY = winH / 2.f;

    //  ----- LEFT PANEL -----
    sf::RectangleShape leftMenu(sf::Vector2f(leftWidth, winH));
    leftMenu.setFillColor(sf::Color(35, 35, 40));
    window.draw(leftMenu);

    sf::RectangleShape leftTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
    leftTab.setFillColor(sf::Color(45, 45, 50));
    leftTab.setPosition(leftWidth - TAB_WIDTH, centerY - TAB_HEIGHT / 2.f);
    leftTab.setOutlineThickness(-1.f);
    leftTab.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(leftTab);

    sf::Text leftIcon(leftExpanded ? "<<" : ">>", font, 18);
    sf::FloatRect lb = leftIcon.getLocalBounds();
    leftIcon.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
    leftIcon.setPosition(leftWidth - TAB_WIDTH / 2.f, centerY);
    leftIcon.setFillColor(sf::Color::Cyan);
    window.draw(leftIcon);

    if (leftExpanded && leftWidth > 200.f) {
        button[0]->setText(isEditMode ? "MODE: EDIT" : "MODE: RUN");
        button[1]->setText(isEditMode ? (isDeleting ? "DELETE" : "INSERT") : (isAutoMode ? "AUTO: ON" : "AUTO: OFF"));
        button[2]->setText(isEditMode ? (isDirected ? "DIRECTED" : "UNDIRECTED") : "FINISH");
        button[3]->setText(isEditMode ? "CLEAR" : "RESET");
        button[4]->setText("RETURN");
        for (size_t i = 0; i < 5; ++i)
            window.draw(*button[i]);
    }

    // ----- RIGHT PANEL -----
    // 1. Vẽ nền Panel
    sf::RectangleShape rightMenu(sf::Vector2f(rightWidth, winH));
    rightMenu.setOrigin(rightWidth, 0); // Đặt gốc bên phải để giãn về bên trái
    rightMenu.setPosition(winW, 0);
    rightMenu.setFillColor(sf::Color(35, 35, 40));
    window.draw(rightMenu);

    // 2. Nút Tab Collapse (Mũi tên lật ngược lại so với bên trái)
    sf::RectangleShape rightTab(sf::Vector2f(TAB_WIDTH, TAB_HEIGHT));
    rightTab.setFillColor(sf::Color(45, 45, 50));
    rightTab.setPosition(winW - rightWidth, centerY - TAB_HEIGHT / 2.f);
    rightTab.setOutlineThickness(-1.f);
    rightTab.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(rightTab);

    sf::Text rightIcon(rightExpanded ? ">>" : "<<", font, 18);
    sf::FloatRect rb = rightIcon.getLocalBounds();
    rightIcon.setOrigin(rb.left + rb.width / 2.f, rb.top + rb.height / 2.f);
    rightIcon.setPosition(winW - rightWidth + TAB_WIDTH / 2.f, centerY);
    rightIcon.setFillColor(sf::Color::Yellow);
    window.draw(rightIcon);

    // 3. Nội dung bên trong khi mở rộng
    if (rightExpanded && rightWidth > 250.f) {
        float panelStart = winW - rightWidth + TAB_WIDTH;
        float tabAreaWidth = RIGHT_PANEL_WIDTH - TAB_WIDTH;

        // Vẽ 3 Thanh Tab nhỏ ở trên cùng
        std::vector<std::string> labels = {"INFO", "DIST", "CODE"};
        float tabW = tabAreaWidth / 3.f;

        for (int i = 0; i < 3; ++i) {
            // Khung tab
            TabState state = static_cast<TabState>(i);
            sf::RectangleShape tRect(sf::Vector2f(tabW - 4.f, 30.f));
            tRect.setPosition(panelStart + i * tabW, 10.f);
            tRect.setFillColor(activeTab == state ? sf::Color(70, 70, 80) : sf::Color(40, 40, 45));
            tRect.setOutlineThickness(activeTab == state ? 1.f : 0.f);
            tRect.setOutlineColor(sf::Color::Yellow);
            window.draw(tRect);

            // Chữ tab
            sf::Text tText(labels[i], font, 14);
            sf::FloatRect tb = tText.getLocalBounds();
            tText.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height / 2.f);
            tText.setPosition(panelStart + i * tabW + tabW / 2.f, 25.f);
            tText.setFillColor(activeTab == state ? sf::Color::White : sf::Color(150, 150, 150));
            window.draw(tText);
        }


        /*float contentY = 60.f; // Dưới thanh Tab một chút
        sf::RectangleShape contentBg(sf::Vector2f(tabAreaWidth - 10.f, winH - contentY - 20.f));
        contentBg.setPosition(panelStart + 5.f, contentY);
        contentBg.setFillColor(sf::Color(30, 30, 35)); // Màu xám tối chuyên nghiệp
        contentBg.setOutlineThickness(1.f);
        contentBg.setOutlineColor(sf::Color(60, 60, 70));
        window.draw(contentBg);*/

        // --- NỘI DUNG TỪNG TRANG ---
        float contentY = 60.f;
        if (activeTab == TabState::Info) { // TRANG INFO
            sf::Text info(R"(
Dijkstra's Algorithm
Finds shortest paths
from source to all
other nodes.
                )", font, 14);
            info.setPosition(panelStart + 10, contentY);
            window.draw(info);
        }
        else if (activeTab == TabState::Dist) { // TRANG DIST
            sf::Text title("Distance Table", font, 16);
            title.setPosition(panelStart + 10, contentY);
            window.draw(title);
            // Bạn có thể dùng vòng lặp vẽ nodes[i].dist tại đây
        } 
        else if (activeTab == TabState::Code) { // TRANG CODE
            float lineIncr = 22.f; // Khoảng cách giữa các dòng (Line spacing)
            float startX = panelStart + 5.f; // Lùi vào một chút so với mép panel
            float startY = contentY + 10.f;

            for (int i = 0; i < pseudoCode.size(); ++i) {
                float currentYPos = startY + (i * lineIncr);

                // 1. VẼ NỀN HIGHLIGHT (Dành cho dòng hiện tại)
                if (currentLine.first <= i && i <= currentLine.second) {
                    sf::RectangleShape background;
                    background.setSize(sf::Vector2f(rightWidth - 10.f, lineIncr)); 
                    background.setPosition(startX, currentYPos);
                    
                    // Màu nền highlight (Xanh dương đậm hoặc Cam nhạt tùy gu của bạn)
                    background.setFillColor(sf::Color(60, 60, 90)); 
                    window.draw(background);
                }

                // 2. VẼ CHỮ
                sf::Text lineText(pseudoCode[i], font, 14);
                lineText.setPosition(startX + 10.f, currentYPos); // Thụt lề chữ so với nền xám

                // Đổi màu chữ cho đẹp
                if (currentLine.first <= i && i <= currentLine.second)
                    lineText.setFillColor(sf::Color::Yellow); // Dòng đang chạy chữ màu Vàng
                else
                    // Màu code mặc định (Trắng mờ hoặc xanh nhạt kiểu IDE)
                    lineText.setFillColor(sf::Color(200, 200, 200));

                window.draw(lineText);
            }
        }
    }
}