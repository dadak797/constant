#pragma once

// Standard library
#include <unordered_map>

// Dependencies
#include <imgui.h>

#include "icon/FontAwesome6.h"
#include "icon/FontAwesome6Brands.h"
#include "icon/FontAwesome5.h"
#include "icon/FontAwesome5Brands.h"
#include "icon/FontAwesome4.h"
#include "icon/CodIcons.h"
#include "icon/FontAudio.h"
#include "icon/ForkAwesome.h"
#include "icon/Kenney.h"
#include "icon/Lucide.h"
#include "icon/MaterialDesign.h"
#include "icon/MaterialDesignIcons.h"
#include "icon/MaterialSymbols.h"

// Forward declarations
class ImFont;


enum class FontIcon
{
    FontAwesome6S,  // Solid
    FontAwesome6R,  // Regular
    FontAwesome6B,  // Brands
    FontAwesome5S,  // Solid
    FontAwesome5R,  // Regular
    FontAwesome5B,  // Brands
    FontAwesome4,
    CodIcons,
    FontAudio,
    ForkAwesome,
    Kenney,
    Lucide,
    MaterialDesign,
    // MaterialDesignIcons,  // Not working
    MaterialSymbolsO,  // Outlined
    // MaterialSymbolsR,  // Rounded - No difference with MaterialSymbolsO
    // MaterialSymbolsS   // Sharp - No difference with MaterialSymbolsO
};

class FontManager
{
public:
    static FontManager& Instance();

    void SetFontIcon(FontIcon fontIcon);
    void SetDefaultFontIcon();

// #ifdef DEBUG_BUILD
    void DrawAllFontIcons(bool* openWindow);
// #endif

private:
    // ImFont* variables are managed by ImFontAtlas.
    // Do not delete it in the destructor.
    std::unordered_map<FontIcon, ImFont*> m_FontIcons;
    ImFontConfig m_DefaultFontConfig;

    FontManager();
    ~FontManager() = default;

    // Delete copy/move constructor/assignment operations
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    void init();

    void loadFontAwesome6S();
    void loadFontAwesome6R();
    void loadFontAwesome6B();
    void loadFontAwesome5S();
    void loadFontAwesome5R();
    void loadFontAwesome5B();
    void loadFontAwesome4();
    void loadCodIcons();
    void loadFontAudio();
    void loadForkAwesome();
    void loadKenney();
    void loadLucide();
    void loadMaterialDesign();
    // void loadMaterialDesignIcons();
    void loadMaterialSymbolsO();
    // void loadMaterialSymbolsR();
    // void loadMaterialSymbolsS();

#ifdef SHOW_FONT_ICONS
    void drawFontAwesome6S(bool* openWindow);
    void drawFontAwesome6R(bool* openWindow);
    void drawFontAwesome6B(bool* openWindow);
    void drawFontAwesome5S(bool* openWindow);
    void drawFontAwesome5R(bool* openWindow);
    void drawFontAwesome5B(bool* openWindow);
    void drawFontAwesome4(bool* openWindow);
    void drawCodIcons(bool* openWindow);
    void drawFontAudio(bool* openWindow);
    void drawForkAwesome(bool* openWindow);
    void drawKenney(bool* openWindow);
    void drawLucide(bool* openWindow);
    void drawMaterialDesign(bool* openWindow);
    // void drawMaterialDesignIcons(bool* openWindow);
    void drawMaterialSymbolsO(bool* openWindow);
    // void drawMaterialSymbolsR(bool* openWindow);
    // void drawMaterialSymbolsS(bool* openWindow);
#endif
};