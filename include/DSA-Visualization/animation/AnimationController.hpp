#pragma once
#include "DSA-Visualization/animation/AnimationStep.hpp"
#include <vector>

class AnimationController {
public:
    AnimationController();

    void loadSteps(const std::vector<AnimationStep>& steps);
    void clear();

    void next();
    void prev();

    void update(float dt);

    const AnimationStep* currentStep() const;
    bool                 hasSteps()    const;
    float                t()           const;
    int                  currentIndex() const;
    int                  totalSteps()   const;
    void                 setSpeed(float speed);

private:
    std::vector<AnimationStep> mSteps;
    int                        mCurrentIndex;
    float                      mT;
    float                      mLerpSpeed;
    bool                       mReverse;
};