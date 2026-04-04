#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

namespace ui{
    // Define the structure of a single node in the swarm
struct SwarmNode {
        sf::Vector2f position;
        sf::Vector2f velocity;
    };

    class SwarmEffect : public sf::Drawable {
    public:
        // Constructor: numNodes ~60-90 is good for a sparse, full-screen look.
        SwarmEffect(unsigned int numNodes, sf::Vector2f screenBounds);

        // Update node positions and apply mouse repulsion
        void update(float dt, sf::Vector2f mousePos);

        // Allow dynamic resizing if the window changes size
        void setBounds(sf::Vector2f newBounds);

    protected:
        // SFML draw override
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
        std::vector<SwarmNode> nodes;
        sf::Vector2f bounds;
        float connectionDistance;
    };
}