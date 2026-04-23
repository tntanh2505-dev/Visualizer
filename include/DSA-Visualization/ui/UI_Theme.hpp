#pragma once
#include <SFML/Graphics.hpp>

namespace UITheme {
    // --- Colors ---
    namespace Color {
        
        // =========================================================================
        // 1. GLOBAL BASE VARIABLES (CHANGE THESE TO UPDATE ALL SCREENS AT ONCE)
        // =========================================================================
        const sf::Color GlobalBg          = sf::Color(15, 15, 20);      // Very dark navy/charcoal (near black)
        const sf::Color GlobalPanelBg     = sf::Color(30, 30, 38, 240); // Dark slate gray with slight transparency
        const sf::Color GlobalPanelBorder = sf::Color(80, 80, 100);     // Medium-dark grayish-blue
        
        const sf::Color GlobalNodeFill    = sf::Color(45, 45, 55);      // Dark steel gray
        const sf::Color GlobalNodeOutline = sf::Color(110, 247, 242);   // Bright neon cyan/aqua
        const sf::Color GlobalNodeFocus   = sf::Color(255, 215, 0);     // Vibrant golden yellow
        
        // --- BUTTON THEME (Amethyst Dark Mode) ---
        const sf::Color GlobalButtonFill  = sf::Color(32, 26, 43);      // Amethyst dark (deep eggplant)
        const sf::Color GlobalButtonHover = sf::Color(53, 38, 77);      // Amethyst hover (lighter purple)
        const sf::Color GlobalButtonBorder = sf::Color(181, 58, 199, 120); // Neon purple outline (semi-transparent)
        const sf::Color GlobalButtonHoverBorder = sf::Color(218, 112, 214); // Bright purple glow (orchid)
        const sf::Color GlobalButtonShadow = sf::Color(0, 0, 0, 150);   // Dark, smooth drop shadow
        const sf::Color GlobalButtonBot   = sf::Color(20, 16, 27);      // Very dark purple-black for bottom gradient
        
        const sf::Color GlobalTextPrimary = sf::Color(245, 245, 250);   // Crisp off-white with a hint of cool blue
        const sf::Color GlobalTextMuted   = sf::Color(150, 150, 160);   // Muted silver/gray
        
        const sf::Color GlobalAccent      = sf::Color(181, 58, 199);    // Vibrant electric amethyst/purple
        const sf::Color GlobalGlow        = sf::Color(181, 58, 199, 80); // Transparent electric amethyst for glow effects

        // --- LIGHT THEME ADDITIONS ---
        const sf::Color LightGlobalBg          = sf::Color(192, 192, 192); // Light Grey
        const sf::Color LightPanelBg           = sf::Color(220, 220, 230, 240); // Very light gray-blue with slight transparency
        const sf::Color LightButtonFill        = sf::Color(200, 200, 220); // Darker grey-blue for contrast
        const sf::Color LightButtonBot         = sf::Color(170, 170, 190); // Muted slate gray for bottom gradient     
        const sf::Color LightButtonBorder      = sf::Color(140, 140, 150); // Medium cool gray
        const sf::Color LightTextPrimary       = sf::Color(30, 30, 40);    // Dark charcoal text for light mode

        // --- DANGER / DESTRUCTIVE BUTTONS ---
        const sf::Color ButtonDangerFill       = sf::Color(220, 60, 60);   // Solid crimson red
        const sf::Color ButtonDangerBot        = sf::Color(160, 40, 40);   // Dark blood red
        const sf::Color ButtonDangerBorder     = sf::Color(255, 100, 100); // Bright light red outline
        const sf::Color ButtonInactiveBorder   = sf::Color(100, 100, 100, 150); // Muted semi-transparent gray

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
        const sf::Color ConnectorBase        = sf::Color(255, 255, 255, 150); // Semi-transparent white
        const sf::Color NodeFillColor        = GlobalNodeFill;
        const sf::Color NodeOutlineColor     = GlobalNodeOutline;

        // Standard UI Elements (SLL_UI)
        const sf::Color PanelBg              = GlobalPanelBg;
        const sf::Color PanelBorder          = GlobalPanelBorder;
        const sf::Color ButtonPrimary        = GlobalButtonFill;
        const sf::Color ButtonDanger         = ButtonDangerFill;
        const sf::Color ButtonClose          = ButtonDangerFill;
        const sf::Color TextBoxBg            = sf::Color(250, 250, 255); // Pure white with a tiny tint of blue
        const sf::Color TextBoxBorder        = GlobalPanelBorder;
        const sf::Color TextBoxSelected      = GlobalNodeOutline;

        // Modern UI / Amethyst Theme (button.cpp)
        inline sf::Color ModernBtnTop         = GlobalButtonFill;
        inline sf::Color ModernBtnBottom      = GlobalButtonBot; 
        inline sf::Color ModernBtnHoverT      = GlobalButtonHover;
        inline sf::Color ModernBtnHoverB      = sf::Color(37, 24, 56);   // Deep violet-black
        inline sf::Color ModernBtnBorder      = GlobalButtonBorder;
        inline sf::Color ButtonHoverBorder    = GlobalButtonHoverBorder; 
        inline sf::Color ButtonShadow         = GlobalButtonShadow;
        
        // Code Panel Theme
        const sf::Color CodePanelBg          = GlobalPanelBg;
        const sf::Color CodePanelBorder      = GlobalPanelBorder;
        const sf::Color CodeTitleBar         = sf::Color(20, 20, 25, 255); // Very dark, solid charcoal
        const sf::Color CodeHighlight        = sf::Color(181, 58, 199, 50); // Faint transparent purple highlight
        const sf::Color CodeAccent           = GlobalNodeOutline;
        const sf::Color CodeTextDefault      = sf::Color(230, 235, 240);   // Very light cool gray
        const sf::Color CodeKeyword          = GlobalAccent;
        const sf::Color CodeType             = GlobalNodeOutline;
        const sf::Color CodeFunction         = sf::Color(255, 189, 46);    // Warm orange/gold
        const sf::Color CodeLineNum          = sf::Color(100, 90, 120);    // Muted lavender-gray

        // --- AVL Tree Theme ---
        inline sf::Color AVLBackground        = GlobalBg;
        inline sf::Color AVLPanelBg           = GlobalPanelBg;
        inline sf::Color AVLAccent            = GlobalAccent;
        inline sf::Color AVLGlow              = GlobalGlow;
        inline sf::Color AVLGlowStrong        = sf::Color(181, 58, 199, 140); // Stronger transparent purple
        inline sf::Color AVLSliderFill        = GlobalNodeOutline;
        inline sf::Color AVLSpeedSliderText   = GlobalTextPrimary;
        
        // Custom Node Colors for the Swatches
        const sf::Color AVLNodeCustom1       = sf::Color(65, 105, 225);   // Royal Blue
        const sf::Color AVLNodeCustom2       = sf::Color(140, 140, 140);  // Medium Gray
        const sf::Color AVLNodeCustom3       = sf::Color(220, 20, 60);    // Crimson
        const sf::Color AVLNodeCustom4       = sf::Color(148, 0, 211);    // Dark Violet
        const sf::Color AVLNodeCustom5       = sf::Color(255, 140, 0);    // Dark Orange
        
        // --- Heap Visualizer Theme ---
        const sf::Color HeapBackground       = GlobalBg;
        const sf::Color HeapControlBg        = GlobalPanelBg;
        const sf::Color HeapNodeFill         = GlobalNodeFill;
        const sf::Color HeapNodeOutline      = GlobalNodeOutline;
        const sf::Color HeapCompare          = sf::Color(87, 190, 255);   // Bright sky blue
        const sf::Color HeapSwap             = sf::Color(255, 124, 124);  // Soft pastel red/coral
        const sf::Color HeapFocus            = GlobalNodeFocus;
        const sf::Color HeapTextHighlight    = sf::Color(251, 209, 101);  // Soft yellow/gold
        const sf::Color HeapAccent           = GlobalAccent;

        // --- Graph (Dijkstra) Theme ---
        const sf::Color GraphBackground      = GlobalBg;
        const sf::Color GraphPanelBg         = GlobalPanelBg;
        const sf::Color GraphTabBg           = sf::Color(45, 45, 50);     // Dark gray
        const sf::Color GraphEdge            = sf::Color(100, 100, 100);  // Standard medium gray
        const sf::Color GraphSource          = sf::Color::Green;          // Standard SFML Green
        const sf::Color GraphVisiting        = sf::Color::Yellow;         // Standard SFML Yellow
        const sf::Color GraphProcessed       = sf::Color::Blue;           // Standard SFML Blue
        const sf::Color GraphProcessing      = sf::Color::Red;            // Standard SFML Red
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
        const float PanelOutlineThickness = 4.f; // THICKER BORDER FOR POPUP PANELS

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