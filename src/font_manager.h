#pragma once

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

#ifdef DEBUG_BUILD
    void DrawAllFontIcons(bool* openWindow);
#endif

private:
    // ImFont* variables are managed by ImFontAtlas.
    // Do not delete it in the destructor.
    ImFont* m_FontAwesome6S { nullptr };  // Solid (Default font)
    ImFont* m_FontAwesome6R { nullptr };  // Regular
    ImFont* m_FontAwesome6B { nullptr };  // Brands
    ImFont* m_FontAwesome5S { nullptr };  // Solid
    ImFont* m_FontAwesome5R { nullptr };  // Regular
    ImFont* m_FontAwesome5B { nullptr };  // Brands
    ImFont* m_FontAwesome4 { nullptr };
    ImFont* m_CodIcons { nullptr };
    ImFont* m_FontAudio { nullptr };
    ImFont* m_ForkAwesome { nullptr };
    ImFont* m_Kenney { nullptr };
    ImFont* m_Lucide { nullptr };
    ImFont* m_MaterialDesign { nullptr };
    // ImFont* m_MaterialDesignIcons { nullptr };
    ImFont* m_MaterialSymbolsO { nullptr };  // Outlined
    // ImFont* m_MaterialSymbolsR { nullptr };  // Rounded
    // ImFont* m_MaterialSymbolsS { nullptr };  // Sharp

    FontManager();
    ~FontManager() = default;

    // Delete copy/move constructor/assignment operations
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    void init();
    void loadDefaultFont();
    void loadAllAdditionalFonts();  // Load all fonts except the default font

#ifdef DEBUG_BUILD    
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