#pragma once
#include <functional>
#include <vector>
#include "DSA-Visualization/animation/AnimationStep.hpp"

struct AVLNode {
    int      value;
    int      height;
    AVLNode* left;
    AVLNode* right;
};

class AVLTree {
public:
    AVLTree();
    ~AVLTree();

    void insert(int value, std::vector<AnimationStep>* steps = nullptr);

    AVLNode* root;

private:
    AVLNode* insert(AVLNode* node, int value,
                    std::vector<AnimationStep>* steps);

    int      height(AVLNode* node);
    void     updateHeight(AVLNode* node);
    int      balanceFactor(AVLNode* node);
    AVLNode* rotateLeft(AVLNode* node);
    AVLNode* rotateRight(AVLNode* node);
    AVLNode* balance(AVLNode* node,
                     std::vector<AnimationStep>* steps);

    void calcPositions(AVLNode* node,
                       float x, float y, float offsetX,
                       std::vector<std::pair<int, sf::Vector2f>>& out);

    void snapshotStep(const std::string& desc,
                      int codeLine,
                      std::vector<AnimationStep>* steps,
                      const std::vector<std::pair<int, sf::Vector2f>>& prevPos,
                      int highlightValue = -1);
};