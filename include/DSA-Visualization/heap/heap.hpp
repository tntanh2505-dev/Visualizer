#pragma once
#include <vector>

// Define the Actions for the visualizer to read
enum class ActionType {
    COMPARE,
    SWAP,
    HIGHLIGHT,
    INSERT
};

struct Action {
    ActionType type;
    int index1;
    int index2;
    int lineIdx;
};

class MaxHeap {
private:
    std::vector<int> nums;
    std::vector<Action> actionQueue; // The "diary" of steps for SFML

    int getParent(int i);
    int getChildLeft(int i);
    int getChildRight(int i);
    void MaxHeapify(int i);
    void Increase(int i, int k);
    void Decrease(int i, int k);

public:
    MaxHeap() = default;

    void Insert(int k);
    void Delete(int i);
    int GetMax();
    void BuildHeap(const std::vector<int>& input_array); 

    // Safely give SFML a copy of the current array to draw
    std::vector<int> getArray() const { return nums; } 
    
    // Safely give SFML the animation steps, then clear the queue
    std::vector<Action> flushActions() {
        std::vector<Action> currentActions = actionQueue;
        actionQueue.clear();
        return currentActions;
    }
};