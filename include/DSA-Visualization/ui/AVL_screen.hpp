#pragma once
#include "DSA-Visualization/ui/Screen.hpp"
#include "DSA-Visualization/ui/Button.hpp"
#include "DSA-Visualization/ui/CodePanel.hpp"
#include "DSA-Visualization/avl_tree/AVL_tree.hpp"
#include "DSA-Visualization/animation/AnimationController.hpp"
#include <optional>
#include <string>
#include <vector>

class AVLScreen : public Screen {
public:
    AVLScreen();
    int run(sf::RenderWindow& window, sf::Font& font) override;

private:
    void buildInsertSteps(int value);
    void drawTree(sf::RenderWindow& window, const sf::Font& font);
    void drawNode(sf::RenderWindow& window, const sf::Font& font,
                  const NodeState& ns, float t);
    void drawEdges(sf::RenderWindow& window,
                   const std::vector<NodeState>& nodes, float t);
    void drawControls(sf::RenderWindow& window, const sf::Font& font);
    void drawInputBox(sf::RenderWindow& window, const sf::Font& font);
    void drawDescription(sf::RenderWindow& window, const sf::Font& font);
    void drawSpeedSlider(sf::RenderWindow& window, const sf::Font& font);
    void updateSliderHandle();

    AVLTree             mTree;
    AnimationController mController;
    CodePanel           mCodePanel;

    std::optional<Button> mInsertBtn;
    std::optional<Button> mPrevBtn;
    std::optional<Button> mNextBtn;
    std::optional<Button> mReturnBtn;

    std::string           mInputString;
    bool                  mInputActive;

    sf::Texture           mBgTexture;
    sf::Sprite            mBgSprite;

    sf::RectangleShape    mSliderTrack;
    sf::CircleShape       mSliderHandle;
    float                 mSpeedValue;
    bool                  mSliderDragging;

    static const std::vector<std::string> INSERT_CODE;
};