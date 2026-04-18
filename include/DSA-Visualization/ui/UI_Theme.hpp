#pragma once
#include <SFML/Graphics.hpp>

namespace UITheme {
    // --- Colors ---
    namespace Color {
        // Core Palette
        const sf::Color TextWhite       = sf::Color::White;
        const sf::Color TextMuted       = sf::Color(150, 150, 150);
        const sf::Color TextDark        = sf::Color::Black;

        // Visual Nodes
        const sf::Color NodeTransparent = sf::Color(0, 255, 255, 0);
        const sf::Color NodeHighlightColor   = sf::Color(255, 255, 0);
        const sf::Color ConnectorBase   = sf::Color(255, 255, 255, 0);
        const sf::Color NodeFillColor   = sf::Color::Transparent;
        const sf::Color NodeOutlineColor   = sf::Color::Cyan;

        // Standard UI Elements (SLL_UI)
        const sf::Color PanelBg         = sf::Color(40, 40, 40, 240);
        const sf::Color PanelBorder     = sf::Color::White;
        const sf::Color ButtonPrimary   = sf::Color(0, 150, 0);
        const sf::Color ButtonDanger    = sf::Color(200, 0, 0);
        const sf::Color ButtonClose     = sf::Color::Red;
        const sf::Color TextBoxBg       = sf::Color::White;
        const sf::Color TextBoxBorder   = sf::Color::Cyan;
        const sf::Color TextBoxSelected = sf::Color::Yellow;

        // Modern UI / Amethyst Theme (button.cpp & codepanel.cpp)
        const sf::Color ModernBtnTop    = sf::Color(32, 26, 43);
        const sf::Color ModernBtnBottom = sf::Color(20, 16, 27);
        const sf::Color ModernBtnHoverT = sf::Color(53, 38, 77);
        const sf::Color ModernBtnHoverB = sf::Color(37, 24, 56);
        const sf::Color ModernBtnBorder = sf::Color(181, 58, 199, 120);
        
        const sf::Color CodePanelBg     = sf::Color(16, 13, 20, 240);
        const sf::Color CodePanelBorder = sf::Color(40, 30, 50, 180);
        const sf::Color CodeTitleBar    = sf::Color(30, 25, 38, 255);
        const sf::Color CodeHighlight   = sf::Color(181, 58, 199, 40);
        const sf::Color CodeAccent      = sf::Color(33, 238, 252);
    }

    // --- Dimensions & Sizes ---
    namespace Size {
        // Nodes
        const float NodeRadius           = 30.f;
        const float NodeOutlineThickness = 3.f;

        // Buttons & Boxes
        const sf::Vector2f ButtonDefault = sf::Vector2f(100.f, 40.f);
        const sf::Vector2f TextBoxDefault= sf::Vector2f(140.f, 40.f);
        const sf::Vector2f PanelDefault  = sf::Vector2f(360.f, 160.f);
        const float BoxOutlineThickness  = 2.f;

        // Fonts
        const unsigned int FontCode      = 12;
        const unsigned int FontSmall     = 14;
        const unsigned int FontNormal    = 18;
        const unsigned int FontLarge     = 20;
        const unsigned int FontTitle     = 22;


        
    }
    namespace Window {
        const float WidthRatio    = 0.95f; // 95% of desktop width
        const float HeightRatio   = 0.90f; // 90% of desktop height
        const int   CenterYOffset = 50;    // pixels to shift up when centering
        const unsigned int Framerate = 60;
    }
}

