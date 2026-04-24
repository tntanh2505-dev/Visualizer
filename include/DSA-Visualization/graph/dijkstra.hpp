#pragma once
#include <vector>
#include <string>
#include <utility>

const long long INF = (long long)1e18;

struct Node {
    std::string label;
    float x, y;
    int parent;
    long long dist;
    bool isProcessed;

    Node() = default;
    Node(std::string label, float x, float y) : label(label), x(x), y(y) {
        parent = -1;
        dist = INF;
        isProcessed = false;
    }
};

struct Edge {
    int from, to, weight;

    Edge() = default;
    Edge(int from, int to, int weight) : from(from), to(to), weight(weight) {}
};

class Dijkstra {
public:
    void init(std::vector<Node> &nodes, std::vector<Edge> &edges, bool isDirected, int source = -1);
    int stage(std::vector<Node> &nodes);
    std::vector<int> getShortestPath(std::vector<Node> &nodes, int target);
    std::vector<int> getAdjacent(int index);

private:
    std::vector<std::vector<std::pair<int, int>>> adjacent;
};