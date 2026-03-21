#include "DataVisualizer/App.hpp"
#include <stdexcept>

App::App()
    : m_window(sf::VideoMode(1280, 720), "Data Visualizer")
{
    m_window.setFramerateLimit(60);

    if (!m_bgTexture.loadFromFile("assets/textures/background.png"))
        throw std::runtime_error("Failed to load background.png");

    m_bgSprite.setTexture(m_bgTexture);

    m_rect.setSize(sf::Vector2f(200.f, 100.f));
    m_rect.setFillColor(sf::Color(100, 180, 255));
    m_rect.setPosition(540.f, 310.f);
}

void App::run()
{
    while (m_window.isOpen())
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                m_window.close();
        }

        m_window.clear(sf::Color(30, 30, 30));
        m_window.draw(m_bgSprite);
        m_window.draw(m_rect);
        m_window.display();
    }
}