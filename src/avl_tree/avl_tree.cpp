#include "DSA-Visualization/avl_tree/AVL_tree.hpp"
#include <algorithm>
#include <functional>
#include <map>

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

void AVLTree::clear() {
    destroyTree(root);
    root = nullptr;
}

//help from Copilot
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

void AVLTree::calcPositions(AVLNode* node,
                             float x, float y, float offsetX,
                             std::vector<std::pair<int, sf::Vector2f>>& out)
{
    if (!node) return;
    out.push_back({node->value, {x, y}});
    calcPositions(node->left,  x - offsetX, y + TREE_Y_GAP, offsetX / 2.f, out);
    calcPositions(node->right, x + offsetX, y + TREE_Y_GAP, offsetX / 2.f, out);
}

void AVLTree::snapshotStep(const std::string& desc,
                            int codeLine,
                            std::vector<AnimationStep>* steps,
                            const std::vector<std::pair<int,sf::Vector2f>>& prevPos,
                            int highlightValue)
{
    if (!steps) return;

    std::vector<std::pair<int, sf::Vector2f>> currPos;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, currPos);

    std::map<int, sf::Vector2f> prevMap;
    for (auto& p : prevPos) prevMap[p.first] = p.second;

    std::map<int, AVLNode*> nodeMap;
    std::function<void(AVLNode*)> collect = [&](AVLNode* n) {
        if (!n) return;
        nodeMap[n->value] = n;
        collect(n->left);
        collect(n->right);
    };
    collect(root);

    AnimationStep step;
    step.description   = desc;
    step.codeLineIndex = codeLine;

    for (auto& cp : currPos) {
        NodeState ns;
        ns.value        = cp.first;
        ns.targetPos    = cp.second;
        ns.startPos     = prevMap.count(cp.first)
                          ? prevMap.at(cp.first)
                          : sf::Vector2f(cp.second.x, cp.second.y - 60.f);
        ns.leftChild    = -1;
        ns.rightChild   = -1;

        if (ns.value == highlightValue) {
            ns.fillColor    = sf::Color(243, 156, 18);
            ns.outlineColor = sf::Color(180, 100,  0);
        } else {
            ns.fillColor    = sf::Color(52,  152, 219);
            ns.outlineColor = sf::Color(21,  101, 192);
        }

        if (nodeMap.count(cp.first)) {
            AVLNode* n = nodeMap[cp.first];
            if (n->left)  ns.leftChild  = n->left->value;
            if (n->right) ns.rightChild = n->right->value;
        }
        step.nodes.push_back(ns);
    }

    steps->push_back(step);
}

AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x  = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left  = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y  = x->right;
    AVLNode* T2 = y->left;
    y->left  = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

AVLNode* AVLTree::balance(AVLNode* node, std::vector<AnimationStep>* steps, bool isDelete) {
    if (!node) return nullptr;
    updateHeight(node);
    int bf = balanceFactor(node);

    if (steps) {
        std::vector<std::pair<int, sf::Vector2f>> before;
        calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, before);
        snapshotStep(
            "Checking balance at " + std::to_string(node->value) + " (BF = " + std::to_string(bf) + ")",
            isDelete ? 7 : 4, steps, before, node->value);
    }

    if (bf > 1) {
        std::vector<std::pair<int, sf::Vector2f>> before;
        calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, before);

        if (balanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            // update root to reflect LR rotation first step
            if (root == node) root = node;
        }

        // Find parent of node and update pointer
        // Instead, we rely on insert() updating root after balance returns
        // So we temporarily set root = result for snapshot
        AVLNode* newNode = rotateRight(node);

        // Patch root temporarily for correct full-tree snapshot
        AVLNode* oldRoot = root;
        std::function<AVLNode*(AVLNode*, AVLNode*, AVLNode*)>
        patchRoot = [&](AVLNode* cur, AVLNode* oldNode, AVLNode* newNode) -> AVLNode* {
            if (!cur) return cur;
            if (cur == oldNode) return newNode;
            cur->left  = patchRoot(cur->left,  oldNode, newNode);
            cur->right = patchRoot(cur->right, oldNode, newNode);
            return cur;
        };
        root = patchRoot(root, node, newNode);

        snapshotStep(
            "Right rotation — rebalanced at " + std::to_string(newNode->value),
            isDelete ? 8 : 5, steps, before, newNode->value);

        return newNode;
    }

    if (bf < -1) {
        std::vector<std::pair<int, sf::Vector2f>> before;
        calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, before);

        if (balanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
        }

        AVLNode* newNode = rotateLeft(node);

        std::function<AVLNode*(AVLNode*, AVLNode*, AVLNode*)>
        patchRoot = [&](AVLNode* cur, AVLNode* oldNode, AVLNode* newNode) -> AVLNode* {
            if (!cur) return cur;
            if (cur == oldNode) return newNode;
            cur->left  = patchRoot(cur->left,  oldNode, newNode);
            cur->right = patchRoot(cur->right, oldNode, newNode);
            return cur;
        };
        root = patchRoot(root, node, newNode);

        snapshotStep(
            "Left rotation — rebalanced at " + std::to_string(newNode->value),
            isDelete ? 8 : 6, steps, before, newNode->value);

        return newNode;
    }

    return node;
}

AVLNode* AVLTree::insert(AVLNode* node, int value,
                          std::vector<AnimationStep>* steps)
{
    if (!node)
        return new AVLNode{value, 1, nullptr, nullptr};

    // Snapshot traversal step
    if (steps) {
        std::vector<std::pair<int, sf::Vector2f>> curr;
        calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, curr);
        snapshotStep(
            value < node->value
                ? "Going left from " + std::to_string(node->value)
                : "Going right from " + std::to_string(node->value),
            value < node->value ? 2 : 3,
            steps, curr, node->value);
    }

    if (value < node->value)
        node->left  = insert(node->left,  value, steps);
    else if (value > node->value)
        node->right = insert(node->right, value, steps);
    else
        return node;

    return balance(node, steps);
}

void AVLTree::insert(int value, std::vector<AnimationStep>* steps) {
    if (!steps) {
        root = insert(root, value, nullptr);
        return;
    }

    // Step 0: initial state
    std::vector<std::pair<int, sf::Vector2f>> initial;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, initial);
    snapshotStep("Starting insert of " + std::to_string(value),
                 0, steps, initial, -1);

    root = insert(root, value, steps);

    // Final step: highlight inserted node green
    std::vector<std::pair<int, sf::Vector2f>> curr;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, curr);

    std::map<int, sf::Vector2f> currMap;
    for (auto& p : curr) currMap[p.first] = p.second;

    std::map<int, AVLNode*> nodeMap;
    std::function<void(AVLNode*)> collect = [&](AVLNode* n) {
        if (!n) return;
        nodeMap[n->value] = n;
        collect(n->left);
        collect(n->right);
    };
    collect(root);

    AnimationStep finalStep;
    finalStep.description   = "Insert complete. Tree is balanced.";
    finalStep.codeLineIndex = 7;

    for (auto& p : curr) {
        NodeState ns;
        ns.value        = p.first;
        ns.targetPos    = p.second;
        ns.startPos     = currMap.count(p.first)
                          ? currMap.at(p.first)
                          : p.second;
        ns.fillColor    = (p.first == value)
                          ? sf::Color(39, 174,  96)
                          : sf::Color(52, 152, 219);
        ns.outlineColor = (p.first == value)
                          ? sf::Color(20, 120,  60)
                          : sf::Color(21, 101, 192);
        ns.leftChild    = -1;
        ns.rightChild   = -1;
        if (nodeMap.count(p.first)) {
            AVLNode* n = nodeMap[p.first];
            if (n->left)  ns.leftChild  = n->left->value;
            if (n->right) ns.rightChild = n->right->value;
        }
        finalStep.nodes.push_back(ns);
    }
    steps->push_back(finalStep);
}

AVLNode* AVLTree::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current && current->left != nullptr)
        current = current->left;
    return current;
}

AVLNode* AVLTree::remove(AVLNode* node, int value, std::vector<AnimationStep>* steps) {
    if (!node) return node;

    if (steps) {
        std::vector<std::pair<int, sf::Vector2f>> curr;
        calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, curr);
        if (value < node->value) {
            snapshotStep("Going left from " + std::to_string(node->value), 2, steps, curr, node->value);
        } else if (value > node->value) {
            snapshotStep("Going right from " + std::to_string(node->value), 3, steps, curr, node->value);
        } else {
            snapshotStep("Found " + std::to_string(node->value) + " to delete", 4, steps, curr, node->value);
        }
    }

    if (value < node->value) {
        node->left = remove(node->left, value, steps);
    } else if (value > node->value) {
        node->right = remove(node->right, value, steps);
    } else {
        if ((node->left == nullptr) || (node->right == nullptr)) {
            if (steps) {
                std::vector<std::pair<int, sf::Vector2f>> curr;
                calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, curr);
                snapshotStep("Replacing node with its child (or removing leaf)", 5, steps, curr, node->value);
            }
            AVLNode* temp = node->left ? node->left : node->right;
            AVLNode* toDelete = node;
            node = temp;
            delete toDelete;
        } else {
            if (steps) {
                std::vector<std::pair<int, sf::Vector2f>> curr;
                calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, curr);
                snapshotStep("Two children: finding min node of right subtree", 6, steps, curr, node->value);
            }
            AVLNode* temp = minValueNode(node->right);
            node->value = temp->value;
            node->right = remove(node->right, temp->value, steps);
        }
    }

    if (!node) return node;

    return balance(node, steps, true);
}

void AVLTree::remove(int value, std::vector<AnimationStep>* steps) {
    if (!steps) {
        root = remove(root, value, nullptr);
        return;
    }

    std::vector<std::pair<int, sf::Vector2f>> initial;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, initial);
    snapshotStep("Starting delete of " + std::to_string(value), 0, steps, initial, -1);

    root = remove(root, value, steps);

    std::vector<std::pair<int, sf::Vector2f>> curr;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, curr);
    snapshotStep("Delete complete. Tree is balanced.", 9, steps, curr, -1);
}

void AVLTree::search(AVLNode* node, int value, std::vector<AnimationStep>* steps) {
    if (steps) {
        std::vector<std::pair<int, sf::Vector2f>> curr;
        calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, curr);
        if (!node) {
            snapshotStep("Value " + std::to_string(value) + " not found", 1, steps, curr, -1);
            return;
        }
        if (value < node->value) {
            snapshotStep("Searching for " + std::to_string(value) + ": smaller, going left", 2, steps, curr, node->value);
        } else if (value > node->value) {
            snapshotStep("Searching for " + std::to_string(value) + ": greater, going right", 3, steps, curr, node->value);
        } else {
            snapshotStep("Found " + std::to_string(value) + "!", 4, steps, curr, node->value);
        }
    }

    if (!node) return;

    if (value < node->value) {
        search(node->left, value, steps);
    } else if (value > node->value) {
        search(node->right, value, steps);
    }
}

void AVLTree::search(int value, std::vector<AnimationStep>* steps) {
    if (!steps) {
        search(root, value, nullptr);
        return;
    }
    std::vector<std::pair<int, sf::Vector2f>> initial;
    calcPositions(root, TREE_ROOT_X, TREE_ROOT_Y, TREE_X_OFFSET, initial);
    snapshotStep("Starting search for " + std::to_string(value), 0, steps, initial, -1);

    search(root, value, steps);
}