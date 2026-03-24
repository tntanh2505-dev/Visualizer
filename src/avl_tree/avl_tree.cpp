#include "DSA-Visualization/avl_tree/AVL_tree.hpp"
#include <algorithm>
#include <functional>

static const float TREE_ROOT_X   = 230.f + 760.f / 2.f;
static const float TREE_ROOT_Y   = 80.f;
static const float TREE_X_OFFSET = 180.f;
static const float TREE_Y_GAP    = 80.f;

static void destroyTree(AVLNode* node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

AVLTree::AVLTree() : root(nullptr) {}
AVLTree::~AVLTree() { destroyTree(root); }

int AVLTree::height(AVLNode* node) {
    return node ? node->height : 0;
}

void AVLTree::updateHeight(AVLNode* node) {
    if (node)
        node->height = 1 + std::max(height(node->left), height(node->right));
}

int AVLTree::balanceFactor(AVLNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

// Calculate positions of all nodes for snapshot
void AVLTree::calcPositions(AVLNode* node,
                             float x, float y, float offsetX,
                             std::vector<std::pair<int, sf::Vector2f>>& out)
{
    if (!node) return;
    out.push_back({node->value, {x, y}});
    calcPositions(node->left,  x - offsetX, y + TREE_Y_GAP, offsetX / 2.f, out);
    calcPositions(node->right, x + offsetX, y + TREE_Y_GAP, offsetX / 2.f, out);
}

// Record a step: startPos = prevPos, targetPos = current positions
void AVLTree::snapshotStep(AVLNode* root,
                            const std::string& desc,
                            int codeLine,
                            std::vector<AnimationStep>* steps,
                            const std::vector<std::pair<int,sf::Vector2f>>& prevPos)
{
    if (!steps) return;

    std::vector<std::pair<int, sf::Vector2f>> currPos;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, currPos);

    // Build lookup for prev positions
    std::map<int, sf::Vector2f> prevMap;
    for (auto& p : prevPos) prevMap[p.first] = p.second;

    AnimationStep step;
    step.description   = desc;
    step.codeLineIndex = codeLine;

    for (auto& cp : currPos) {
        NodeState ns;
        ns.value         = cp.first;
        ns.targetPos     = cp.second;
        ns.startPos      = prevMap.count(cp.first)
                           ? prevMap.at(cp.first)
                           : sf::Vector2f(cp.second.x, cp.second.y - 60.f);
        ns.fillColor     = sf::Color(52, 152, 219);
        ns.outlineColor  = sf::Color(21, 101, 192);
        ns.balanceFactor = 0; // updated below
        ns.hasLeft       = false;
        ns.hasRight      = false;
        ns.leftValue     = -1;
        ns.rightValue    = -1;
        step.nodes.push_back(ns);
    }

    // Fill in balance factors and edge info
    std::function<void(AVLNode*)> fillInfo = [&](AVLNode* node) {
        if (!node) return;
        for (auto& ns : step.nodes) {
            if (ns.value == node->value) {
                ns.balanceFactor = balanceFactor(node);
                ns.hasLeft       = node->left  != nullptr;
                ns.hasRight      = node->right != nullptr;
                ns.leftValue     = node->left  ? node->left->value  : -1;
                ns.rightValue    = node->right ? node->right->value : -1;
            }
        }
        fillInfo(node->left);
        fillInfo(node->right);
    };
    fillInfo(root);

    steps->push_back(step);
}

AVLNode* AVLTree::rotateRight(AVLNode* y, std::vector<AnimationStep>* steps) {
    // Snapshot before rotation
    std::vector<std::pair<int, sf::Vector2f>> before;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, before);

    AVLNode* x  = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left  = T2;
    updateHeight(y);
    updateHeight(x);

    // Snapshot after rotation
    snapshotStep(root,
        "Right rotation on node " + std::to_string(y->value),
        5, steps, before);

    return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x, std::vector<AnimationStep>* steps) {
    std::vector<std::pair<int, sf::Vector2f>> before;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, before);

    AVLNode* y  = x->right;
    AVLNode* T2 = y->left;
    y->left  = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);

    snapshotStep(root,
        "Left rotation on node " + std::to_string(x->value),
        6, steps, before);

    return y;
}

AVLNode* AVLTree::balance(AVLNode* node, std::vector<AnimationStep>* steps) {
    if (!node) return nullptr;
    updateHeight(node);
    int bf = balanceFactor(node);

    if (bf > 1) {
        if (balanceFactor(node->left) < 0)
            node->left = rotateLeft(node->left, steps);
        return rotateRight(node, steps);
    }
    if (bf < -1) {
        if (balanceFactor(node->right) > 0)
            node->right = rotateRight(node->right, steps);
        return rotateLeft(node, steps);
    }
    return node;
}

AVLNode* AVLTree::insert(AVLNode* node, int value,
                          std::vector<AnimationStep>* steps)
{
    if (!node) {
        return new AVLNode{value, 1, nullptr, nullptr};
    }
    if (value < node->value) {
        std::vector<std::pair<int, sf::Vector2f>> before;
        if (steps)
            calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, before);

        node->left  = insert(node->left,  value, steps);

        if (steps)
            snapshotStep(root,
                "Inserted " + std::to_string(value) + " as left child of " + std::to_string(node->value),
                2, steps, before);

    } else if (value > node->value) {
        std::vector<std::pair<int, sf::Vector2f>> before;
        if (steps)
            calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, before);

        node->right = insert(node->right, value, steps);

        if (steps)
            snapshotStep(root,
                "Inserted " + std::to_string(value) + " as right child of " + std::to_string(node->value),
                3, steps, before);

    } else {
        return node;
    }

    return balance(node, steps);
}

void AVLTree::insert(int value, std::vector<AnimationStep>* steps) {
    // Snapshot before insert
    std::vector<std::pair<int, sf::Vector2f>> before;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, before);

    if (steps) {
        AnimationStep step;
        step.description   = "Inserting " + std::to_string(value) + "...";
        step.codeLineIndex = 0;
        // current tree before insert
        for (auto& p : before) {
            NodeState ns;
            ns.value         = p.first;
            ns.startPos      = p.second;
            ns.targetPos     = p.second;
            ns.fillColor     = sf::Color(52, 152, 219);
            ns.outlineColor  = sf::Color(21, 101, 192);
            ns.balanceFactor = 0;
            ns.hasLeft       = false;
            ns.hasRight      = false;
            step.nodes.push_back(ns);
        }
        steps->push_back(step);
    }

    root = insert(root, value, steps);

    // Snapshot final state
    if (steps) {
        std::vector<std::pair<int, sf::Vector2f>> after;
        calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, after);

        std::map<int, sf::Vector2f> beforeMap;
        for (auto& p : before) beforeMap[p.first] = p.second;

        AnimationStep step;
        step.description   = "Inserted " + std::to_string(value) + ". Tree balanced.";
        step.codeLineIndex = 7;

        for (auto& p : after) {
            NodeState ns;
            ns.value         = p.first;
            ns.targetPos     = p.second;
            ns.startPos      = beforeMap.count(p.first)
                               ? beforeMap.at(p.first)
                               : sf::Vector2f(p.second.x, p.second.y - 60.f);
            ns.fillColor     = (p.first == value)
                               ? sf::Color(39, 174, 96)
                               : sf::Color(52, 152, 219);
            ns.outlineColor  = (p.first == value)
                               ? sf::Color(20, 120, 60)
                               : sf::Color(21, 101, 192);
            ns.balanceFactor = 0;
            ns.hasLeft       = false;
            ns.hasRight      = false;
            ns.leftValue     = -1;
            ns.rightValue    = -1;
            step.nodes.push_back(ns);
        }
        steps->push_back(step);
    }
}