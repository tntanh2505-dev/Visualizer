#include <DSA-Visualization/ui/components/SwarmEffect.hpp> // Adjust this path if necessary
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace ui{
SwarmEffect::SwarmEffect(unsigned int numNodes, sf::Vector2f screenBounds) 
        : bounds(screenBounds), connectionDistance(150.0f) {
        
        // Seed the random number generator (ensure this is only done once ideally)
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            seeded = true;
        }

        // Initialize nodes with random positions across the full screen 
        // and slow starting velocities
        for (unsigned int i = 0; i < numNodes; ++i) {
            SwarmNode node;
            node.position.x = static_cast<float>(std::rand() % static_cast<int>(bounds.x));
            node.position.y = static_cast<float>(std::rand() % static_cast<int>(bounds.y));
            
            // Random initial velocity drifting slowly
            node.velocity.x = static_cast<float>((std::rand() % 60) - 30);
            node.velocity.y = static_cast<float>((std::rand() % 60) - 30);
            
            nodes.push_back(node);
        }
    }

    void SwarmEffect::setBounds(sf::Vector2f newBounds) {
        bounds = newBounds;
    }

    void SwarmEffect::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        sf::VertexArray lines(sf::Lines);

        // 1. Draw connecting lines
        for (size_t i = 0; i < nodes.size(); ++i) {
            for (size_t j = i + 1; j < nodes.size(); ++j) {
                float dx = nodes[i].position.x - nodes[j].position.x;
                float dy = nodes[i].position.y - nodes[j].position.y;
                float distSq = dx * dx + dy * dy;

                // Only draw if within the sparse connection distance
                if (distSq < connectionDistance * connectionDistance) {
                    float dist = std::sqrt(distSq);
                    
                    // Dimmer base alpha (max 100 instead of 255) to keep it unobtrusive
                    sf::Uint8 alpha = static_cast<sf::Uint8>(100.0f * (1.0f - (dist / connectionDistance)));
                    
                    // Sleek, techy cyan color (highly transparent)
                    sf::Color lineColor(100, 200, 255, alpha); 
                    
                    lines.append(sf::Vertex(nodes[i].position, lineColor));
                    lines.append(sf::Vertex(nodes[j].position, lineColor));
                }
            }
        }
        target.draw(lines, states);

        // 2. Draw the actual nodes (dots)
        sf::CircleShape circle(1.5f); // Smaller 1.5 radius for a sparse look
        circle.setOrigin(1.5f, 1.5f);
        circle.setFillColor(sf::Color(150, 220, 255, 80)); // Reduced opacity

        for (const auto& node : nodes) {
            circle.setPosition(node.position);
            target.draw(circle, states);
        }
    }

    void SwarmEffect::update(float dt, sf::Vector2f mousePos) {
        const float repelRadius = 150.0f; // How close the mouse needs to be to push nodes
        const float minSpeed = 15.0f;     // Minimum drift speed
        const float maxSpeed = 150.0f;    // Maximum speed when pushed

        for (auto& node : nodes) {
            // --- MOUSE REPULSION LOGIC ---
            float dx = node.position.x - mousePos.x;
            float dy = node.position.y - mousePos.y;
            float distSq = dx * dx + dy * dy;

            if (distSq < repelRadius * repelRadius && distSq > 1.0f) {
                float dist = std::sqrt(distSq);
                // Stronger push the closer the mouse is
                float pushForce = (repelRadius - dist) / repelRadius; 
                
                node.velocity.x += (dx / dist) * pushForce * 800.0f * dt;
                node.velocity.y += (dy / dist) * pushForce * 800.0f * dt;
            }

            // --- VELOCITY MANAGEMENT ---
            // Apply slight friction to calm them down after being pushed
            node.velocity.x *= 0.98f;
            node.velocity.y *= 0.98f;

            // Clamp speeds (enforce min drift and max burst)
            float currentSpeed = std::sqrt(node.velocity.x * node.velocity.x + node.velocity.y * node.velocity.y);
            if (currentSpeed > 0.1f) {
                if (currentSpeed < minSpeed) {
                    node.velocity.x = (node.velocity.x / currentSpeed) * minSpeed;
                    node.velocity.y = (node.velocity.y / currentSpeed) * minSpeed;
                } else if (currentSpeed > maxSpeed) {
                    node.velocity.x = (node.velocity.x / currentSpeed) * maxSpeed;
                    node.velocity.y = (node.velocity.y / currentSpeed) * maxSpeed;
                }
            }

            // --- MOVEMENT & BOUNDS ---
            node.position += node.velocity * dt;

            // Soft bounce off the screen edges
            if (node.position.x < 0) { node.position.x = 0; node.velocity.x *= -1; }
            if (node.position.x > bounds.x) { node.position.x = bounds.x; node.velocity.x *= -1; }
            if (node.position.y < 0) { node.position.y = 0; node.velocity.y *= -1; }
            if (node.position.y > bounds.y) { node.position.y = bounds.y; node.velocity.y *= -1; }
        }
    }
}