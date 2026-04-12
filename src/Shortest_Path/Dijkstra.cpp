#include "DSA-Visualization/Shortest_Path/Dijkstra.hpp"

void Dijkstra::init(std::vector<Node> &nodes, std::vector<Edge> &edges, int source) {
    for (Node &v : nodes) {
        v.dist = INF;
        v.parent = -1;
        v.isProcessed = false;
    }

    adjacent.clear();
    adjacent.resize(nodes.size());
    for (auto &[from, to, weight] : edges)
        adjacent[from].emplace_back(to, weight);

    if (source != -1)
        nodes[source].dist = 0;
}

int Dijkstra::stage(std::vector<Node> &nodes) {
    int best = -1;
    long long dist = INF;

    for (int u = 0; u < nodes.size(); ++u)
        if (dist > nodes[u].dist && !nodes[u].isProcessed) {
            dist = nodes[u].dist;
            best = u;
        }

    if (best == -1) return -1;
    nodes[best].isProcessed = true;

    for (auto &[v, w] : adjacent[best])
        if (nodes[v].dist > dist + w) {
            nodes[v].dist = dist + w;
            nodes[v].parent = best;
        }

    return best;
}

std::vector<int> Dijkstra::getShortestPath(std::vector<Node> &nodes, int target) {
    if (nodes[target].dist == INF)
        return {};

    std::vector<int> path;
    while (target != -1) {
        path.emplace_back(target);
        target = nodes[target].parent;
    }
    return path;
}

std::vector<int> Dijkstra::getAdjacent(int index) {
    std::vector<int> neighbor;
    for (auto &[vertex, weight] : adjacent[index])
        neighbor.emplace_back(vertex);
    return neighbor;
}
