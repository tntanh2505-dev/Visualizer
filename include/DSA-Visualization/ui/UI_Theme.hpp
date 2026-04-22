#pragma once
#include <SFML/Graphics.hpp>

namespace UITheme {
    // --- Colors ---
    namespace Color {
        
        // =========================================================================
        // 1. GLOBAL BASE VARIABLES (CHANGE THESE TO UPDATE ALL SCREENS AT ONCE)
        // =========================================================================
        const sf::Color GlobalBg          = sf::Color(15, 15, 20);      
        const sf::Color GlobalPanelBg     = sf::Color(30, 30, 38, 240); 
        const sf::Color GlobalPanelBorder = sf::Color(80, 80, 100);     
        
        const sf::Color GlobalNodeFill    = sf::Color(45, 45, 55);      
        const sf::Color GlobalNodeOutline = sf::Color(110, 247, 242);   
        const sf::Color GlobalNodeFocus   = sf::Color(255, 215, 0);     
        
        // --- BUTTON THEME (Amethyst Dark Mode) ---
        const sf::Color GlobalButtonFill  = sf::Color(32, 26, 43);      // Amethyst dark
        const sf::Color GlobalButtonHover = sf::Color(53, 38, 77);      // Amethyst hover
        const sf::Color GlobalButtonBorder = sf::Color(181, 58, 199, 120); // Neon purple outline
        const sf::Color GlobalButtonHoverBorder = sf::Color(218, 112, 214); // Bright purple glow
        const sf::Color GlobalButtonShadow = sf::Color(0, 0, 0, 150);   // Dark, smooth drop shadow
        
        const sf::Color GlobalTextPrimary = sf::Color(245, 245, 250);   
        const sf::Color GlobalTextMuted   = sf::Color(150, 150, 160);   
        
        const sf::Color GlobalAccent      = sf::Color(181, 58, 199);    
        const sf::Color GlobalGlow        = sf::Color(181, 58, 199, 80); // Intensified Base Glow

        // =========================================================================
        // 2. SPECIFIC MAPPINGS (LINKED TO GLOBALS ABOVE)
        // =========================================================================

        // Core Palette
        const sf::Color TextWhite            = GlobalTextPrimary;
        const sf::Color TextMuted            = GlobalTextMuted;
        const sf::Color TextDark             = sf::Color::Black;

        // Visual Nodes (SLL)
        const sf::Color NodeTransparent      = sf::Color::Transparent;
        const sf::Color NodeHighlightColor   = GlobalNodeFocus;
        const sf::Color ConnectorBase        = sf::Color(255, 255, 255, 150);
        const sf::Color NodeFillColor        = GlobalNodeFill;
        const sf::Color NodeOutlineColor     = GlobalNodeOutline;

        // Standard UI Elements (SLL_UI)
        const sf::Color PanelBg              = GlobalPanelBg;
        const sf::Color PanelBorder          = GlobalPanelBorder;
        const sf::Color ButtonPrimary        = GlobalButtonFill;
        const sf::Color ButtonDanger         = sf::Color(200, 50, 50);
        const sf::Color ButtonClose          = sf::Color(200, 50, 50);
        const sf::Color TextBoxBg            = sf::Color(250, 250, 255); 
        const sf::Color TextBoxBorder        = GlobalPanelBorder;
        const sf::Color TextBoxSelected      = GlobalNodeOutline;

        // Modern UI / Amethyst Theme (button.cpp)
        const sf::Color ModernBtnTop         = GlobalButtonFill;
        const sf::Color ModernBtnBottom      = sf::Color(20, 16, 27); 
        const sf::Color ModernBtnHoverT      = GlobalButtonHover;
        const sf::Color ModernBtnHoverB      = sf::Color(37, 24, 56);
        const sf::Color ModernBtnBorder      = GlobalButtonBorder;
        const sf::Color ButtonHoverBorder    = GlobalButtonHoverBorder; 
        const sf::Color ButtonShadow         = GlobalButtonShadow;
        
        // Code Panel Theme
        const sf::Color CodePanelBg          = GlobalPanelBg;
        const sf::Color CodePanelBorder      = GlobalPanelBorder;
        const sf::Color CodeTitleBar         = sf::Color(20, 20, 25, 255); 
        const sf::Color CodeHighlight        = sf::Color(181, 58, 199, 50); // Code select
        const sf::Color CodeAccent           = GlobalNodeOutline;
        const sf::Color CodeTextDefault      = sf::Color(230, 235, 240);
        const sf::Color CodeKeyword          = GlobalAccent;
        const sf::Color CodeType             = GlobalNodeOutline;
        const sf::Color CodeFunction         = sf::Color(255, 189, 46);
        const sf::Color CodeLineNum          = sf::Color(100, 90, 120);

        // --- AVL Tree Theme ---
        const sf::Color AVLBackground        = GlobalBg;
        const sf::Color AVLPanelBg           = GlobalPanelBg;
        const sf::Color AVLAccent            = GlobalAccent;
        const sf::Color AVLGlow              = GlobalGlow;
        const sf::Color AVLGlowStrong        = sf::Color(181, 58, 199, 140); 
        const sf::Color AVLSliderFill        = GlobalNodeOutline;
        const sf::Color AVLSpeedSliderText   = GlobalTextPrimary;
        
        // Custom Node Colors for the Swatches
        const sf::Color AVLNodeCustom1       = sf::Color(65, 105, 225);   // Royal Blue
        const sf::Color AVLNodeCustom2       = sf::Color(140, 140, 140);  // Grey
        const sf::Color AVLNodeCustom3       = sf::Color(220, 20, 60);    // Crimson
        const sf::Color AVLNodeCustom4       = sf::Color(148, 0, 211);    // Dark Violet
        const sf::Color AVLNodeCustom5       = sf::Color(255, 140, 0);    // Dark Orange
        
        // --- Heap Visualizer Theme ---
        const sf::Color HeapBackground       = GlobalBg;
        const sf::Color HeapControlBg        = GlobalPanelBg;
        const sf::Color HeapNodeFill         = GlobalNodeFill;
        const sf::Color HeapNodeOutline      = GlobalNodeOutline;
        const sf::Color HeapCompare          = sf::Color(87, 190, 255);
        const sf::Color HeapSwap             = sf::Color(255, 124, 124);
        const sf::Color HeapFocus            = GlobalNodeFocus;
        const sf::Color HeapTextHighlight    = sf::Color(251, 209, 101);
        const sf::Color HeapAccent           = GlobalAccent;

        // --- Graph (Dijkstra) Theme ---
        const sf::Color GraphBackground      = GlobalBg;
        const sf::Color GraphPanelBg         = GlobalPanelBg;
        const sf::Color GraphTabBg           = sf::Color(45, 45, 50);
        const sf::Color GraphEdge            = sf::Color(100, 100, 100);
        const sf::Color GraphSource          = sf::Color::Green;
        const sf::Color GraphVisiting        = sf::Color::Yellow;
        const sf::Color GraphProcessed       = sf::Color::Blue;
        const sf::Color GraphProcessing      = sf::Color::Red;
        const sf::Color GraphNodeFill        = GlobalNodeFill;
        const sf::Color GraphHighlightBg     = GlobalButtonHover; 
        
        // Sliders & General Screen
        const sf::Color SliderTrack          = GlobalPanelBorder;
        const sf::Color SliderHandle         = GlobalTextPrimary;
    }

    // --- Dimensions & Sizes ---
    namespace Size {
        const float NodeRadius           = 28.f;
        const float NodeOutlineThickness = 3.f;

        const sf::Vector2f ButtonDefault = sf::Vector2f(100.f, 40.f);
        const sf::Vector2f TextBoxDefault= sf::Vector2f(140.f, 40.f);
        const sf::Vector2f PanelDefault  = sf::Vector2f(360.f, 160.f);
        const float BoxOutlineThickness  = 2.f;

        const unsigned int FontCode      = 12;
        const unsigned int FontSmall     = 14;
        const unsigned int FontNormal    = 18;
        const unsigned int FontLarge     = 20;
        const unsigned int FontTitle     = 22;
    }

    namespace Window {
        const float ReferenceWidth  = 1280.f;
        const float ReferenceHeight = 720.f;
        const float WidthRatio    = 0.95f; 
        const float HeightRatio   = 0.90f; 
        const int   CenterYOffset = 50;
        const unsigned int Framerate = 60;
    }

    namespace Layout {
        const sf::Vector2f CodePanelPos    = {20.f, 100.f};
        const sf::Vector2f ControlPanelPos = {1020.f, 80.f};
        const sf::Vector2f SliderPos       = {1025.f, 385.f};
        const sf::Vector2f BottomRowPos    = {50.f, 650.f};
    }
}