#pragma once
#include <vector>
#include <string>
#include "DSA-Visualization/animation/AnimationStep.hpp"

struct SLLNode {
    int value;
    SLLNode* next;
};

class SinglyLinkedList {
public:
    SinglyLinkedList();
    ~SinglyLinkedList();
    void clear();

    void insert(int value, std::vector<AnimationStep>* steps = nullptr);
    void remove(int value, std::vector<AnimationStep>* steps = nullptr);
    void search(int value, std::vector<AnimationStep>* steps = nullptr);

    SLLNode* head;

private:
    void calcPositions(SLLNode* node, float x, float y, float offsetX, std::vector<std::pair<int, sf::Vector2f>>& out);
    void snapshotStep(const std::string& desc,
                      int codeLine,
                      std::vector<AnimationStep>* steps,
                      const std::vector<std::pair<int, sf::Vector2f>>& prevPos,
                      int highlightValue = -1,
                      int activeNodeValue = -1);
};
