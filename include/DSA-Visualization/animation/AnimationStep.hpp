#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct NodeState {
    int          value;
    sf::Vector2f startPos;
    sf::Vector2f targetPos;
    sf::Color    fillColor;
    sf::Color    outlineColor;
    int          leftChild;    // value of left child, -1 if none
    int          rightChild;   // value of right child, -1 if none
};

struct AnimationStep {
    std::vector<NodeState> nodes;
    std::string            description;
    int                    codeLineIndex;
};