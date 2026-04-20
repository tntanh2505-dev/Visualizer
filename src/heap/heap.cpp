#include <vector>
#include <climits>
#include "DSA-Visualization/heap/heap.hpp"
#include "DSA-Visualization/ui/heap_screen.hpp"
using namespace std;

int MaxHeap::getParent(int i) {
    return ((i - 1) / 2 >= 0) ? nums[(i - 1) / 2] : -1; 
}

int MaxHeap::getChildLeft(int i) {
    return (2 * i + 1 < nums.size()) ? nums[2 * i + 1] : -1;
}

int MaxHeap::getChildRight(int i) {
    return (2 * i + 2 < nums.size()) ? nums[2 * i + 2] : -1;
}

void MaxHeap::MaxHeapify(int i) {
    while (true) {
        int l = 2 * i + 1, r = 2 * i + 2, largest = i;  
        if (l < nums.size()) {
            actionQueue.push_back({ActionType::COMPARE, l, i, 2}); 
            if (nums[largest] < nums[l]) largest = l;
        }
        if (r < nums.size()) {
            actionQueue.push_back({ActionType::COMPARE, r, largest, 4}); 
            if (nums[largest] < nums[r]) largest = r;
        }   
        if (largest == i) break;
        actionQueue.push_back({ActionType::SWAP, i, largest, 7}); 
        int temp = nums[i];
        nums[i] = nums[largest];
        nums[largest] = temp;
        i = largest;
    }
}

void MaxHeap::BuildHeap(const std::vector<int>& input_array) {
    nums = input_array;
    for (int i= nums.size()/2 - 1; i >= 0; --i) MaxHeapify(i);
}

void MaxHeap::Increase(int i, int k) {
    if (nums[i] > k) return;
    nums[i] = k;
    while (i > 0 && nums[i] > nums[(i - 1) / 2]) {
        actionQueue.push_back({ActionType::COMPARE, i, (i - 1) / 2, 1}); 
        actionQueue.push_back({ActionType::SWAP, i, (i - 1) / 2, 2});
        int temp = nums[i];
        nums[i] = nums[(i - 1) / 2];
        nums[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

void MaxHeap::Decrease(int i,int k) {
    if (nums[i] < k) return;
    actionQueue.push_back({ActionType::HIGHLIGHT, i, -1});
    nums[i] = k;
    //Sink down if violates
    MaxHeapify(i);
}

void MaxHeap::Insert(int k) {
    actionQueue.push_back({ActionType::INSERT, (int)nums.size(), k});
    nums.push_back(INT_MIN);
    int i = nums.size() - 1;
    Increase(i, k);
}

void MaxHeap::Update(int i, int k) {
    if (i < 0 || i >= (int)nums.size()) return;
    int oldValue = nums[i];

    actionQueue.push_back({ActionType::CHANGE_VALUE, i, k, 0});
    
    if (k > oldValue) {
        Increase(i, k);
    } 
    else if (k < oldValue) {
        Decrease(i, k);
    }
    else {
        actionQueue.push_back({ActionType::HIGHLIGHT, i, -1, 0});
    }
}

void MaxHeap::Delete(int i) {
    if (nums.empty() || i < 0 || i >= (int)nums.size()) return;

    int lastIdx = (int)nums.size() - 1;
    if (i == lastIdx) {
        actionQueue.push_back({ActionType::REMOVE, lastIdx, nums.back(), 9});
        nums.pop_back();
        return;
    }

    actionQueue.push_back({ActionType::SWAP, i, lastIdx, 9});
    std::swap(nums[i], nums[lastIdx]);
    actionQueue.push_back({ActionType::REMOVE, lastIdx, nums.back(), 9});
    nums.pop_back();

    if (!nums.empty() && i < (int)nums.size()) {
        MaxHeapify(i); 
        int current = i;
        while (current > 0 && nums[current] > nums[(current - 1) / 2]) {
            int parent = (current - 1) / 2;
            actionQueue.push_back({ActionType::COMPARE, current, parent, 1}); 
            actionQueue.push_back({ActionType::SWAP, current, parent, 2});
            std::swap(nums[current], nums[parent]);
            current = parent;
        }
    }
}