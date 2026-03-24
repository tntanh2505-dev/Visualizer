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
    AVLNode* rotateLeft(AVLNode* node,
                        std::vector<AnimationStep>* steps);
    AVLNode* rotateRight(AVLNode* node,
                         std::vector<AnimationStep>* steps);
    AVLNode* balance(AVLNode* node,
                     std::vector<AnimationStep>* steps);

    // position calculator for step recording
    void     calcPositions(AVLNode* node,
                           float x, float y, float offsetX,
                           std::vector<std::pair<int, sf::Vector2f>>& out);
    void     snapshotStep(AVLNode* root,
                          const std::string& desc,
                          int codeLine,
                          std::vector<AnimationStep>* steps,
                          const std::vector<std::pair<int,sf::Vector2f>>& prevPos);
};