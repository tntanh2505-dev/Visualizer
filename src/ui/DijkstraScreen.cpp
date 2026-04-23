#include "DSA-Visualization/ui/DijkstraScreen.hpp"
#include <cmath>
#include <random>
#include <fstream>

const float NODE_RADIUS = 25.f;
const float LEFT_PANEL_WIDTH = 250.f;
const float RIGHT_PANEL_WIDTH = 300.f;
const float TAB_WIDTH = 35.f;
const float TAB_HEIGHT = 50.f;
const std::vector<std::string> pseudoCode = {
    "Dijkstra(Graph, source)",        
    "  dist[source] = 0",                     
    "  for each vertex v in Graph",           
    "      add v to Q",                       
    "      if v != source",                   
    "          dist[v] = INFINITY",
    "          prev[v] = NULL",       
    "",                                        
    "  while Q is not empty",                 
    "      v = vertex in Q with min dist",    
    "      remove v from Q",                 
    "",                                       
    "      for each neighbor u of v",         
    "          w = weight(v, u)",            
    "          if dist[u] > dist[v] + w",    
    "             dist[u] = dist[v] + w",
    "             prev[u] = v"
    "",                                       
    "  return dist[]",                     
    "",
    "",
    "Path_Finding(Graph, target)",
    "  if dist[target] = INFINITY",
    "      return empty",
    "",
    "  path = {}",
    "  while target != NULL",
    "      path.insert(target)",
    "      target = prev[target]",
    "  reverse(path)",
    "",
    "  return path"
};

int DijkstraScreen::run(sf::RenderWindow &window, sf::Font &font) {
    initialization();
    for (int i = 0; i < 8; ++i) {
        button[i] = std::make_unique<ModernButton>("", font, sf::Vector2f(160.f, 40.f), 5.f);
        button[i]->setPosition(100.f, 50.f + (i * 50.f));
    }
    button[8] = std::make_unique<ModernButton>("<< PREV", font, sf::Vector2f(100.f, 40.f), 5.f);
    button[9] = std::make_unique<ModernButton>("NEXT >>", font, sf::Vector2f(100.f, 40.f), 5.f);


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

        if (isEditMode) {
            nodes = m_edit[currentIndex - 1].m_nodes;
            edges = m_edit[currentIndex - 1].m_edges;
        } else if (currentIndex != 0 && !isAlgoDone) {
            currentLine = m_run[currentIndex - 1].m_currentLine;
            visitingNode = m_run[currentIndex - 1].m_visitingNode;
            processingNode = m_run[currentIndex - 1].m_processingNode;
            nodes = m_run[currentIndex - 1].m_nodes;
            dist = m_run[currentIndex - 1].m_dist;
        }

        if (finishFlag) {
            finishFlag = false;
            
        }

        if (isAutoMode && tickClock.getElapsedTime().asSeconds() > 0.8f) {
            
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
    path.clear();
    currentIndex = 1;
    m_edit.clear();
    m_edit.push_back({{}, {}});
    m_run.clear();

    visitingList.clear();
    dist.clear();
    nodes.clear();
    edges.clear();

    returnFlag = false;
    finishFlag = false;
    isEditMode = true;
    isAutoMode = false;
    isDeleting = false;
    isDirected = false;
    isAlgoDone = false;
    isDragging = false;

    sourceNode = -1;
    selectNode = -1;
    editingNode = -1;
    editingEdge = -1;
    draggingNode = -1;
    visitingNode = -1;
    processingNode = -1;
    inputBuffer.clear();

    currentLine = 0;
    activeTab = TabState::Info;
}

void DijkstraScreen::handleInput(sf::RenderWindow &window, sf::Event &event, sf::Vector2i mPos) {
    sf::Vector2f worldPos = window.mapPixelToCoords(mPos);
    float winW = static_cast<float>(window.getSize().x);
    float winH = static_cast<float>(window.getSize().y);
    float centerX = winW / 2.f;
    float centerY = winH / 2.f;
    float barStart = LEFT_PANEL_WIDTH;
    float barEnd = winW - RIGHT_PANEL_WIDTH;
    float barCenter = (barEnd + barStart) / 2.f;
    float bottomY = winH - 60.f;
    float barWidth = (barEnd - barStart) * 0.7f;
    float barHeight = 6.f;

    if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window.setView(sf::View(visibleArea));
    }

    // Khi thả chuột trái -> Ngừng trạng thái di chuyển node
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        isDragging = false;
        draggingNode = -1;
    }

    if ((editingNode != -1 || editingEdge != -1) && event.type == sf::Event::MouseButtonPressed) {
        editingNode = -1;
        editingEdge = -1;
        inputBuffer = "";
        return;
    }

    for (int i = 0; i < 10; ++i)
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
            if (button[0]->isClicked(worldPos, true)) { // Nút MODE
                selectNode = -1;
                isEditMode = !isEditMode;
                if (isEditMode) {
                    currentIndex = m_edit.size();
                } else {
                    currentIndex = 0;
                }
                m_run.clear();
                processingNode = -1;
                sourceNode = -1;
                isAlgoDone = false;
                dist.assign(nodes.size(), INF);
                for (size_t i = 0; i < nodes.size(); ++i)
                    nodes[i].isProcessed = false;
                visitingList.clear();
                visitingNode = -1;
                path.clear();
                currentLine = 0;
                algorithm.init(nodes, edges, isDirected);
            }
            else if (button[1]->isClicked(worldPos, true)) { // Nút INSERT/DELETE // AUTO
                selectNode = -1;
                if (isEditMode) {
                    isDeleting = !isDeleting;
                } else {
                    isAutoMode = !isAutoMode;
                }
            }
            else if (button[2]->isClicked(worldPos, true)) { // Nút DIRECTED/UNDIRECTED // FINISH
                if (isEditMode) {
                    selectNode = -1;
                    isDirected = !isDirected;
                } else {
                    finishFlag = true;
                }
            }
            else if (button[3]->isClicked(worldPos, true)) { // Nút CLEAR // RESET
                selectNode = -1;
                if (isEditMode) {
                    nodes.clear();
                    edges.clear();
                    m_edit.clear();
                    m_edit.push_back({{}, {}});
                    currentIndex = 1;
                } else {
                    sourceNode = -1;
                    isAlgoDone = false;
                    processingNode = -1;
                    visitingNode = -1;
                    visitingList.clear();
                    dist.assign(nodes.size(), INF);
                    m_run.clear();
                    currentIndex = 0;
                    for (size_t i = 0; i < nodes.size(); ++i)
                        nodes[i].isProcessed = false;
                    path.clear();
                    currentLine = 0;
                }
            }
            else if (button[4]->isClicked(worldPos, true)) { // Nút SAVE
                std::ofstream outFile("data/graph.txt");
                if (!outFile) return;

                outFile << nodes.size() << "\n";
                for (const auto &n : nodes)
                    outFile << n.label << " " << n.x << " " << n.y << "\n";

                outFile << edges.size() << ' ' << isDirected << "\n";
                for (const auto &[from, to, weight] : edges)
                    outFile << from << " " << to << " " << weight << "\n";

                outFile.close();
            }
            else if (button[5]->isClicked(worldPos, true)) { // Nút LOAD
                std::ifstream inFile("data/graph.txt");
                    if (!inFile) return;
                    initialization();

                    int numNodes;
                    inFile >> numNodes;
                    for (int i = 0; i < numNodes; ++i) {
                        float x, y;
                        std::string label;
                        inFile >> label >> x >> y;
                        nodes.emplace_back(Node(label, x, y));
                        dist.emplace_back(INF);
                    }

                    int numEdges;
                    inFile >> numEdges >> isDirected;
                    for (int i = 0; i < numEdges; ++i) {
                        int u, v, w;
                        inFile >> u >> v >> w;
                        edges.emplace_back(Edge(u, v, w));
                    }

                    inFile.close();

                    m_edit.clear();
                    m_edit.push_back({{}, {}});
                    m_edit.push_back({nodes, edges});
                    currentIndex = 2;
            }
            else if (button[6]->isClicked(worldPos, true)) { // Nút RANDOM
                initialization();

                float radius = winW / 8.f;
                for (int i = 0; i < 6; ++i) {
                    float angle = i * (2.0f * M_PI / 6.0f); 
                    Node n;
                    n.label = std::to_string(i);
                    n.x = centerX + radius * std::cos(angle);
                    n.y = centerY + radius * std::sin(angle);
                    nodes.emplace_back(n);
                }

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> nodeDist(0, 5);
                std::uniform_int_distribution<> weightDist(1, 20);
                std::vector<std::vector<bool>> exist(6, std::vector<bool>(6, false));
                for (int i = 0; i < 15; ++i) {
                    int u = nodeDist(gen);
                    int v = nodeDist(gen);
                    int w = weightDist(gen);
                    if (u == v || exist[u][v] || exist[v][u])
                        continue;
                        exist[u][v] = exist[v][u] = true;
                    edges.emplace_back(Edge(u, v, w));
                }

                m_edit.push_back({nodes, edges});
                currentIndex = 2;
            }
            else if (button[7]->isClicked(worldPos, true)) { // Nút RETURN
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

        bool mouseInSeekBar = (mPos.x >= barCenter - barWidth / 2.f && mPos.x <= barCenter + barWidth / 2.f &&
                               mPos.y >= bottomY - 10.f && mPos.y <= bottomY + 10.f);
        isDragging = mouseInSeekBar;

        if (button[8]->isClicked(worldPos, true)) { // Nút PREV
            if (currentIndex > 1)
                currentIndex--;
        }
        if (button[9]->isClicked(worldPos, true)) { // Nút NEXT
            if (currentIndex != 0) {
                if (isEditMode) {
                    if (currentIndex < m_edit.size())
                        currentIndex++;
                } else if (isAlgoDone) {
                    if (currentIndex < path.size())
                        currentIndex++;
                } else {
                    if (currentIndex < m_run.size())
                        currentIndex++;
                }
            }
        }
        
    }

    if (selectNode >= nodes.size())
        selectNode = -1;
    if (event.type == sf::Event::MouseMoved && isDragging)
        fixedSeekBar(worldPos.x, barCenter - barWidth / 2.f, barWidth);

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

                    while (m_edit.size() > (size_t)currentIndex) m_edit.pop_back();
                    m_edit.push_back({nodes, edges});
                    currentIndex = m_edit.size();

                    sourceNode = -1;
                    selectNode = -1;
                } else {
                    int sz = edges.size();
                    edges.erase(std::remove_if(edges.begin(), edges.end(), [&](const Edge& e) {
                        sf::Vector2f A(nodes[e.from].x, nodes[e.from].y);
                        sf::Vector2f B(nodes[e.to].x, nodes[e.to].y);
                        return isSegmentHovering(worldPos, A, B);
                    }), edges.end());
                    if (sz != edges.size()) {
                        while (m_edit.size() > (size_t)currentIndex) m_edit.pop_back();
                        m_edit.push_back({nodes, edges});
                        currentIndex = m_edit.size();
                    }
                }
            } else {
                // CHẾ ĐỘ THÊM / NỐI
                if (hoveredNode == -1) {
                    if (isPosValid(worldPos, winW, winH)) {
                        nodes.emplace_back(Node(std::to_string(nodes.size()), worldPos.x, worldPos.y));
                        selectNode = -1;
                        dist.push_back(INF);

                        while (m_edit.size() > (size_t)currentIndex) m_edit.pop_back();
                        m_edit.push_back({nodes, edges});
                        currentIndex = m_edit.size();
                    }
                } else {
                    draggingNode = hoveredNode;
                    if (selectNode == -1) selectNode = hoveredNode;
                    else {
                        if (selectNode != hoveredNode) {
                            edges.emplace_back(Edge(selectNode, hoveredNode, 1));

                            while (m_edit.size() > (size_t)currentIndex) m_edit.pop_back();
                            m_edit.push_back({nodes, edges});
                            currentIndex = m_edit.size();
                        }
                        selectNode = -1;
                    }
                }
            }
        } else {
            // CHẾ ĐỘ RUN
            if (sourceNode == -1) {
                if (hoveredNode == -1)
                    return;
                selectNode = -1;
                sourceNode = hoveredNode;
                dist.assign(nodes.size(), INF);
                dist[sourceNode] = 0;
                isAlgoDone = false;
                algorithm.init(nodes, edges, isDirected, sourceNode);
                m_run.clear();
                m_run.push_back({1, -1, -1, nodes, dist});
                while (true) {
                    visitingNode = -1;
                    processingNode = algorithm.stage(nodes);
                    if (processingNode == -1) {
                        m_run.push_back({17, -1, -1, nodes, dist});
                        break;
                    }
                    m_run.push_back({9, visitingNode, processingNode, nodes, dist});
                    visitingList = algorithm.getAdjacent(processingNode);
                    for (int v : visitingList) {
                        visitingNode = v;
                        dist[v] = nodes[v].dist;
                        m_run.push_back({14, visitingNode, processingNode, nodes, dist});
                    }
                }
                currentIndex = 1;
            } else {
                if (isAlgoDone) {
                    if (selectNode == -1) {
                        if (hoveredNode == -1)
                            return;
                        currentLine = 20;
                        selectNode = hoveredNode;
                        path = algorithm.getShortestPath(nodes, selectNode);
                        currentIndex = 1;
                    } else {
                        if (currentIndex < path.size())
                            currentIndex++;
                        else {
                            selectNode = -1;
                            path.clear();
                            currentIndex = 0;
                        }
                    }
                } else {
                    if (currentIndex < m_run.size()) {
                        currentIndex++;
                    } else {
                        isAlgoDone = true;
                        currentIndex = 0;
                        path.clear();
                    }
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

        m_edit[currentIndex - 1].m_nodes = nodes;
    }
}

bool DijkstraScreen::isSegmentHovering(sf::Vector2f pos, sf::Vector2f A, sf::Vector2f B) {
    float l2 = std::pow(A.x - B.x, 2) + std::pow(A.y - B.y, 2);
    if (l2 == 0.0) return std::hypot(pos.x - A.x, pos.y - A.y);
    float t = std::max(0.f, std::min(1.f, ((pos.x - A.x) * (B.x - A.x) + (pos.y - A.y) * (B.y - A.y)) / l2));
    sf::Vector2f projection = A + t * (B - A);
    return std::hypot(pos.x - projection.x, pos.y - projection.y) < 8.f;
}

bool DijkstraScreen::isPosValid(sf::Vector2f pos, float winW, float winH, int ignoreNode) {
    if (pos.x < leftWidth + NODE_RADIUS) return false;
    if (pos.x > winW - rightWidth - NODE_RADIUS) return false;
    if (pos.y > winH - 120.f - NODE_RADIUS) return false;
    for (int i = 0; i < (int)nodes.size(); ++i) {
        if (i == ignoreNode) continue;
        float d = std::hypot(nodes[i].x - pos.x, nodes[i].y - pos.y);
        if (d < NODE_RADIUS * 2.5f) return false;
    }
    return true;
}

void DijkstraScreen::fixedSeekBar(float mouseX, float startX, float barWidth) {
    float relativeX = std::max(0.f, std::min(mouseX - startX, barWidth));
    float clampedX = std::max(startX, std::min(mouseX, startX + barWidth));
    float ratio = (clampedX - startX) / barWidth;

    // 3. Xác định tổng số bước dựa trên Mode hiện tại
    size_t totalSteps = 0;
    if (isEditMode) totalSteps = m_edit.size();
    else if (!isAlgoDone) totalSteps = m_run.size();
    else totalSteps = path.size();
    if (totalSteps == 0)
        totalSteps = 1;

    float stepWidth = barWidth / static_cast<float>(totalSteps);
    int newIndex = static_cast<int>(relativeX / stepWidth) + 1;
    if (newIndex > (int)totalSteps) newIndex = (int)totalSteps;
    currentIndex = newIndex;
}

void DijkstraScreen::updateAnimation(float dt) {
    float targetLeft = leftExpanded ? LEFT_PANEL_WIDTH : TAB_WIDTH; 
    leftWidth += (targetLeft - leftWidth) * 12.f * dt;
    float targetRight = rightExpanded ? RIGHT_PANEL_WIDTH : TAB_WIDTH;
    rightWidth += (targetRight - rightWidth) * 12.f * dt;
}

sf::Color DijkstraScreen::getNodeColor(sf::RenderWindow &window, int index) {
    if (!isEditMode) {
        if (index == processingNode)
            return sf::Color::Red;
        if (index == visitingNode)
            return sf::Color::Yellow;
        if (index == sourceNode)
            return sf::Color::Green;
        if (nodes[index].isProcessed)
            return sf::Color::Cyan;
    }
    if (index == selectNode)
        return sf::Color::Magenta;

    return sf::Color(100, 100, 100);
}

void DijkstraScreen::drawGraph(sf::RenderWindow &window, sf::Font &font) {
    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Xác định node đang hover (để vẽ hiệu ứng)
    int hoveredNode = -1;
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (std::hypot(nodes[i].x - worldPos.x, nodes[i].y - worldPos.y) < NODE_RADIUS) {
            hoveredNode = i;
            break;
        }
    }

    //  Draw edges
    auto drawEdge = [&](sf::Vector2f A, sf::Vector2f B, bool condition) -> void {
        sf::Vector2f direction = B - A;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float angle = std::atan2(direction.y, direction.x) * 180.f / M_PI;
        float thickness = 3.f;

        sf::RectangleShape line(sf::Vector2f(length, thickness));
        line.setPosition(A);
        line.setRotation(angle);
        line.setOrigin(0, thickness / 2.f);

        line.setFillColor(sf::Color(100, 100, 100));
        if (isEditMode && isSegmentHovering(worldPos, A, B) && hoveredNode == -1)
            line.setFillColor(sf::Color::White);
        if (condition)
            line.setFillColor(sf::Color::Yellow);
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
            else if (condition)
                arrow.setFillColor(sf::Color::Yellow);
            else
                arrow.setFillColor(sf::Color(100, 100, 100));

            window.draw(arrow);
        }
    };

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

    for (size_t i = 0; i < edges.size(); ++i) {
        auto [from, to, weight] = edges[i];
        sf::Vector2f A(nodes[from].x, nodes[from].y);
        sf::Vector2f B(nodes[to].x, nodes[to].y);
        sf::Vector2f labelPos = getLabelPosition(A, B, 12.f);
        bool condition = (from == processingNode && to == visitingNode) ||
                         (from == visitingNode && to == processingNode && !isDirected);

        drawEdge(A, B, condition);
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

    if (isAlgoDone && currentIndex != 0)
        for (int i = 1; i < currentIndex; ++i) {
            sf::Vector2f A(nodes[path[i - 1]].x, nodes[path[i - 1]].y);
            sf::Vector2f B(nodes[path[i]].x, nodes[path[i]].y);
            drawEdge(A, B, true);
        }

    //  Draw nodes
    for (size_t i = 0; i < nodes.size(); ++i) {
        sf::CircleShape shape(NODE_RADIUS);
        shape.setOrigin(NODE_RADIUS, NODE_RADIUS);
        shape.setPosition(nodes[i].x, nodes[i].y);
        shape.setFillColor(sf::Color(50, 50, 50)); 
        shape.setOutlineThickness(-3.f);

        shape.setOutlineColor(getNodeColor(window, i));
        if (i == hoveredNode) {
            shape.setOutlineThickness(-5.f);
            shape.setOutlineColor(sf::Color::White);
        }
        if (i == selectNode)
            shape.setOutlineColor(sf::Color::Magenta);

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

    if (isAlgoDone && currentIndex != 0)
        for (int i = 1; i < currentIndex; ++i) {
            if (path[i] == selectNode)
                continue;
            sf::CircleShape shape(NODE_RADIUS);
            shape.setOrigin(NODE_RADIUS, NODE_RADIUS);
            shape.setPosition(nodes[path[i]].x, nodes[path[i]].y); 
            shape.setFillColor(sf::Color::Transparent);
            shape.setOutlineThickness(-3.f);
            shape.setOutlineColor(sf::Color::Yellow);
            window.draw(shape);
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
        button[4]->setText("SAVE FILE");
        button[5]->setText("LOAD FILE");
        button[6]->setText("RANDOM");
        button[7]->setText("RETURN");
        for (size_t i = 0; i < 8; ++i)
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

        // --- NỘI DUNG TỪNG TRANG ---
        float contentY = 60.f;
        if (activeTab == TabState::Info) { // TRANG INFO
            sf::Text title("DIJKSTRA'S ALGORITHM", font, 16);
            title.setStyle(sf::Text::Bold);
            title.setFillColor(sf::Color::Yellow);
            title.setPosition(panelStart + 10, contentY);
            window.draw(title);

            sf::Text info(R"(
[ ALGORITHM ]
Dijkstra finds the shortest
path from Source to all nodes.
- Weights: Non-negative only
- Complexity: O(V*V + E)

[ MODE: EDIT ]
- [L-Click]: Create Node/Edge
- [R-Click]: Edit Weight/Label

[ MODE: RUN ]
- Step 1: Select Source Node
- Step 2: Observe Expansion
- Step 3: Select Target Node
- Step 4: Observe Final Path
(All actions via Left-Click)
)", font, 16);
            info.setPosition(panelStart + 10, contentY + 25.f);
            info.setFillColor(sf::Color(220, 220, 220));
            window.draw(info);

            float legendStartY = contentY + 400.f;
            float circleRadius = 6.f;
            float itemSpacing = 22.f;

            // Danh sách các trạng thái cần hiển thị

            std::vector<std::pair<sf::Color, std::string>> legends = {
                {sf::Color::White,         "Hovering"},
                {sf::Color::Magenta,       "Selecting"},
                {sf::Color::Green,         "Source Node"},
                {sf::Color::Red,           "Best Node"},
                {sf::Color::Yellow,        "Visiting"},
                {sf::Color::Blue,          "Processed"}
            };

            for (int i = 0; i < legends.size(); ++i) {
                float yPos = legendStartY + (i * itemSpacing);

                // Vẽ hình tròn màu
                sf::CircleShape circle(circleRadius);
                circle.setFillColor(legends[i].first);
                circle.setOutlineThickness(1.f);
                circle.setOutlineColor(sf::Color(255, 255, 255, 50)); // Viền mờ cho đẹp
                circle.setPosition(panelStart + 15.f, yPos + 3.f);
                window.draw(circle);

                // Vẽ nhãn văn bản
                sf::Text label(legends[i].second, font, 12);
                label.setPosition(panelStart + 15.f + circleRadius * 2 + 10.f, yPos);
                label.setFillColor(sf::Color(180, 180, 180));
                window.draw(label);
            }
        }
        else if (activeTab == TabState::Dist) { // TRANG DIST
            sf::Text title("DIAGNOSTICS", font, 16);
            title.setStyle(sf::Text::Bold);
            title.setFillColor(sf::Color::Yellow);
            title.setPosition(panelStart + 10.f, contentY);
            window.draw(title);

            // 2. Định nghĩa các cột (Anchor points)
            float colStatusX = panelStart + 15.f;
            float colNodeX   = panelStart + 75.f;
            float colDistX   = panelStart + 140.f;
            float headerY    = contentY + 35.f;
            float rowHeight  = 28.f;

            // Vẽ Header của bảng
            sf::Text h1("STATUS", font, 12); h1.setPosition(colStatusX, headerY); h1.setFillColor(sf::Color(120, 120, 120));
            sf::Text h2("NODE", font, 12); h2.setPosition(colNodeX, headerY); h2.setFillColor(sf::Color(120, 120, 120));
            sf::Text h3("DISTANCE", font, 12); h3.setPosition(colDistX, headerY); h3.setFillColor(sf::Color(120, 120, 120));
            window.draw(h1); window.draw(h2); window.draw(h3);

            // Đường kẻ phân cách Header
            sf::RectangleShape line(sf::Vector2f(tabAreaWidth - 25.f, 1.f));
            line.setPosition(panelStart + 10.f, headerY + 20.f);
            line.setFillColor(sf::Color(80, 80, 80));
            window.draw(line);

            // 3. Vẽ dữ liệu các hàng
            float startTableY = headerY + 30.f;

            for (size_t i = 0; i < nodes.size(); ++i) {
                float yPos = startTableY + (i * rowHeight);

                // --- CỘT 1: STATUS (Hình tròn màu) ---
                sf::CircleShape statusCircle(5.f);
                statusCircle.setPosition(colStatusX + 8.f, yPos + 3.f);
                
                // Màu sắc dựa trên trạng thái của node (logic của bạn)
                statusCircle.setFillColor(getNodeColor(window, i)); 
                window.draw(statusCircle);

                // --- CỘT 2: NODE (Tên/Nhãn) ---
                sf::Text nodeTxt(nodes[i].label, font, 14);
                nodeTxt.setPosition(colNodeX + 8.f, yPos);
                nodeTxt.setFillColor(sf::Color::White);
                window.draw(nodeTxt);

                // --- CỘT 3: DISTANCE ---
                if (!isEditMode) {
                    std::string dStr = (dist[i] == INF) ? "INF" : std::to_string(dist[i]);
                    sf::Text distTxt(dStr, font, 14);
                    distTxt.setPosition(colDistX + 8.f, yPos);
                    
                    // Tô màu cột Dist cho sinh động
                    if (dStr == "INF") distTxt.setFillColor(sf::Color(100, 100, 100));
                    else distTxt.setFillColor(getNodeColor(window, i));

                    window.draw(distTxt);
                }

                // Đường kẻ mờ giữa các hàng
                sf::RectangleShape rowLine(sf::Vector2f(tabAreaWidth - 25.f, 0.5f));
                rowLine.setPosition(panelStart + 10.f, yPos + rowHeight - 2.f);
                rowLine.setFillColor(sf::Color(50, 50, 50));
                window.draw(rowLine);
            }
        } 
        else if (activeTab == TabState::Code) { // TRANG CODE
            sf::Vector2f panelPos(winW - RIGHT_PANEL_WIDTH + TAB_WIDTH + 5.f, contentY + 10.f);
            sf::Vector2f panelSize(tabAreaWidth - 10.f, winH - contentY - 30.f);
            panel = CodePanel(font, panelPos, panelSize);
            panel.setPosition(sf::Vector2f(panelStart + 5.f, contentY + 10.f));
            panel.loadSnippets({ {"pseudoCode", pseudoCode} });
            panel.update("pseudoCode", currentLine);
            panel.draw(window);
        }
    }

    // --- SEEK BAR
    float availableAreaStart = LEFT_PANEL_WIDTH;
    float availableAreaEnd = winW - RIGHT_PANEL_WIDTH;
    float centerX = availableAreaStart + (availableAreaEnd - availableAreaStart) / 2.f;
    float bottomY = winH - 60.f; // Cách đáy màn hình 60px

    // 2. Cấu hình kích thước thanh Seek Bar
    float barWidth = (availableAreaEnd - availableAreaStart) * 0.7f; // Dài bằng 70% vùng trống
    float barHeight = 6.f;

    // 3. Vẽ thanh nền (Background Bar) - Màu tối
    sf::RectangleShape barBg(sf::Vector2f(barWidth, barHeight));
    barBg.setOrigin(barWidth / 2.f, barHeight / 2.f);
    barBg.setPosition(centerX, bottomY);
    barBg.setFillColor(sf::Color(45, 45, 55)); // Màu xám xanh đậm
    window.draw(barBg);

    float progressFactor = 0.0f; 
    if (isEditMode) {
        if (!m_edit.empty())
            progressFactor = (float)currentIndex / m_edit.size();
    }
    else if (isAlgoDone) {
        if (!path.empty())
            progressFactor = (float)currentIndex / path.size();
    }
    else {
        if (!m_run.empty())
            progressFactor = (float)currentIndex / m_run.size();
    }
    sf::RectangleShape barFill(sf::Vector2f(barWidth * progressFactor, barHeight));
    barFill.setOrigin(0, barHeight / 2.f);
    barFill.setPosition(centerX - barWidth / 2.f, bottomY);
    barFill.setFillColor(sf::Color(0, 122, 255));
    window.draw(barFill);

    // 5. Vẽ nút tròn điều khiển (Handle) - Màu trắng
    sf::CircleShape handle(8.f); // Bán kính 8px
    handle.setOrigin(8.f, 8.f);
    handle.setPosition(centerX - barWidth / 2.f + barWidth * progressFactor, bottomY);
    handle.setFillColor(sf::Color::White);
    handle.setOutlineThickness(1.5f);
    handle.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(handle);
    
    // 6. Vẽ text hiển thị số bước ở ngay dưới thanh bar
    /*if (!history.empty())*/ {
        sf::Text stepInfo("", font, 16);
        if (isEditMode)
            stepInfo.setString(std::to_string(currentIndex) + '/' + std::to_string(m_edit.size()));
        else if (isAlgoDone)
            stepInfo.setString(std::to_string(currentIndex) + '/' + std::to_string(path.size()));
        else
            stepInfo.setString(std::to_string(currentIndex) + '/' + std::to_string(m_run.size()));
        sf::FloatRect textRect = stepInfo.getLocalBounds();
        stepInfo.setOrigin(textRect.left + textRect.width / 2.0f, 0);
        stepInfo.setPosition(centerX, bottomY + 20.f);
        stepInfo.setFillColor(sf::Color(150, 150, 150));
        window.draw(stepInfo);
    }

    button[8]->setPosition(centerX - (barWidth / 2.f) - 80.f, bottomY);
    window.draw(*button[8]);
    button[9]->setPosition(centerX + (barWidth / 2.f) + 80.f, bottomY);
    window.draw(*button[9]);
}
