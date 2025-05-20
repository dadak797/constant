#pragma once

#include "macro/singleton_macro.h"
#include "texture.h"

// Standard library
#include <cstdint>
#include <array>


class SceneWindow {
    DECLARE_SINGLETON(SceneWindow)

public:
    void Render(bool* openWindow = nullptr);
    void RenderBgColorPopup(bool* openWindow = nullptr);

private:
    uint32_t m_Framebuffer { 0 };
    TextureUPtr m_ColorTexture { nullptr };

    int32_t m_Width { 960 };
    int32_t m_Height { 640 };

    std::array<float, 3> m_BgColor { 0.1f, 0.2f, 0.3f};

    const char* IMGUI_BGCOLOR_KEY = "Constant-BgColor";

    void init();
    void initFramebuffer();
    void resizeFramebuffer(int32_t width, int32_t height);
    void clearFramebuffer();
    void processEvents();

#ifdef __EMSCRIPTEN__
    // void saveBgColorToLocalStorage(const float* color);
    // void loadBgColorFromLocalStorage(float* color);
#endif
};