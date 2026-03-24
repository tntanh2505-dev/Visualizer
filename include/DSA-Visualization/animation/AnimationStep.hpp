#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct NodeState {
    int          value;
    int          balanceFactor;
    sf::Vector2f startPos;      // where it was
    sf::Vector2f targetPos;     // where it's going
    sf::Color    fillColor;
    sf::Color    outlineColor;
    bool         hasLeft;
    bool         hasRight;
    int          leftValue;     // -1 if none
    int          rightValue;    // -1 if none
};

struct AnimationStep {
    std::vector<NodeState> nodes;
    std::string            description;
    int                    codeLineIndex;
};