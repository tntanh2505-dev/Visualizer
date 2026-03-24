#include "DSA-Visualization/animation/AnimationController.hpp"
#include <algorithm>

AnimationController::AnimationController()
: mCurrentIndex(0)
, mT(1.f)
, mLerpSpeed(2.f)
, mReverse(false)
{}

void AnimationController::loadSteps(const std::vector<AnimationStep>& steps) {
    mSteps        = steps;
    mCurrentIndex = 0;
    mT            = 1.f;
    mReverse      = false;
}

void AnimationController::clear() {
    mSteps.clear();
    mCurrentIndex = 0;
    mT            = 1.f;
    mReverse      = false;
}

void AnimationController::next() {
    if (mT < 1.f) return; // wait for current animation to finish
    if (mCurrentIndex < (int)mSteps.size() - 1) {
        mCurrentIndex++;
        mT       = 0.f;
        mReverse = false;
    }
}

void AnimationController::prev() {
    if (mT < 1.f && !mReverse) return;
    if (mCurrentIndex > 0) {
        mCurrentIndex--;
        mT       = 0.f;
        mReverse = true;
    }
}

void AnimationController::update(float dt) {
    if (mT < 1.f)
        mT = std::min(1.f, mT + dt * mLerpSpeed);
}

const AnimationStep* AnimationController::currentStep() const {
    if (mSteps.empty()) return nullptr;
    return &mSteps[mCurrentIndex];
}

bool AnimationController::hasSteps() const {
    return !mSteps.empty();
}

float AnimationController::t() const {
    // if reversing, flip t so nodes animate back to startPos
    return mReverse ? 1.f - mT : mT;
}

int AnimationController::currentIndex() const { return mCurrentIndex; }
int AnimationController::totalSteps()   const { return (int)mSteps.size(); }

void AnimationController::setSpeed(float speed) {
    mLerpSpeed = std::max(0.5f, speed);
}