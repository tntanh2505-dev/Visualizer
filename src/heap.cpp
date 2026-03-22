#include <vector>
#include <climits>
#include "heap.hpp"
using namespace std;

int MaxHeap::getParent(int i) {
    return ((i-1)/2 >= 0) ? nums[(i-1)/2] : -1; 
}

int MaxHeap::getChildLeft(int i) {
    return (2*i+1 < nums.size()) ? nums[2*i+1] : -1;
}

int MaxHeap::getChildRight(int i) {
    return (2*i+2 < nums.size()) ? nums[2*i+2] : -1;
}

void MaxHeap::MaxHeapify(int i) {
    while (true) {
        int l = 2*i+1, r =2*i+2, largest = i;
        if (l < nums.size() && nums[largest] < nums[l]) largest = l;
        if (r < nums.size() && nums[largest] < nums[r]) largest = r;
        //Stop if node i-th violates or has no children
        if (largest == i) break;
        int temp = nums[i];
        nums[i] = nums[largest];
        nums[largest] = temp;
        i = largest;
    }
}

void MaxHeap::BuildHeap(const std::vector<int>& input_array) {
    nums = input_array;
    for (int i= nums.size()/2 - 1; i>=0; --i) MaxHeapify(i);
}

void MaxHeap::Increase(int i,int k) {
    if (nums[i]>k) return;
    nums[i]=k;
    //Bubble up if violates
    while (i > 0 && nums[i] > nums[(i-1)/2]) {
        int temp = nums[i];
        nums[i] = nums[(i-1)/2];
        nums[(i-1)/2] = temp;
        i = (i-1)/2;
    }
}

void MaxHeap::Decrease(int i,int k) {
    if (nums[i]<k) return;
    nums[i]=k;
    //Sink down if violates
    MaxHeapify(i);
}

void MaxHeap::Insert(int k) {
    nums.push_back(INT_MIN);
    int i = nums.size()-1;
    Increase(nums.size()-1,k);
}

void MaxHeap::Delete(int i) {
    if (i>=nums.size()) return;
    nums[i] = nums.back();
    nums.pop_back();
    //Fix if violates
    if (i<nums.size()) {
        //Bubble up first (if needed)
        Increase(i,nums[i]); 
        //Then sink down (if needed)
        MaxHeapify(i); 
    } 
}

int MaxHeap::GetMax() {
    return nums[0];
}
