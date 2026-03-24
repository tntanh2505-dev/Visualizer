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
    void clear();

    void insert(int value, std::vector<AnimationStep>* steps = nullptr);
    void remove(int value, std::vector<AnimationStep>* steps = nullptr);
    void search(int value, std::vector<AnimationStep>* steps = nullptr);

    AVLNode* root;

private:
    AVLNode* insert(AVLNode* node, int value,
                    std::vector<AnimationStep>* steps);
    AVLNode* remove(AVLNode* node, int value,
                    std::vector<AnimationStep>* steps);
    void search(AVLNode* node, int value,
                std::vector<AnimationStep>* steps);
    AVLNode* minValueNode(AVLNode* node);

    int      height(AVLNode* node);
    void     updateHeight(AVLNode* node);
    int      balanceFactor(AVLNode* node);
    AVLNode* rotateLeft(AVLNode* node);
    AVLNode* rotateRight(AVLNode* node);
    AVLNode* balance(AVLNode* node,
                     std::vector<AnimationStep>* steps, bool isDelete = false);

    void calcPositions(AVLNode* node,
                       float x, float y, float offsetX,
                       std::vector<std::pair<int, sf::Vector2f>>& out);

    void snapshotStep(const std::string& desc,
                      int codeLine,
                      std::vector<AnimationStep>* steps,
                      const std::vector<std::pair<int, sf::Vector2f>>& prevPos,
                      int highlightValue = -1);
};