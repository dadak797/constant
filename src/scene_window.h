#pragma once

#include "framebuffer.h"
#include "macro/singleton_macro.h"
#include "mesh.h"
#include "shader_program.h"

// Standard library
#include <array>
#include <cstdint>

class SceneWindow {
  DECLARE_SINGLETON(SceneWindow)

 public:
  void Render(bool* openWindow = nullptr);
  void RenderBgColorPopup(bool* openWindow = nullptr);

 private:
  FramebufferUPtr m_Framebuffer{nullptr};
  TexturePtr m_ColorTexture{nullptr};

  // In WebAssembly, the framebuffer and scene size are the same as the canvas size.
  // In native applications, the framebuffer size can be different from the scene size.
  // The framebuffer size is the size of the texture, and the scene size is the size of the ImGui window.
  int32_t m_FramebufferWidth{960};
  int32_t m_FramebufferHeight{640};
  int32_t m_SceneWidth{960};
  int32_t m_SceneHeight{640};

  std::array<float, 3> m_BgColor{0.1f, 0.2f, 0.3f};

  const char* IMGUI_BGCOLOR_KEY = "Constant-BgColor";

  MeshUPtr m_Box;
  ShaderProgramUPtr m_SimpleProgram;

  void init();
  void initFramebuffer();
  void resizeFramebuffer(int32_t width, int32_t height);
  void clearFramebuffer();
  void processEvents();

  void renderMesh();

#ifdef __EMSCRIPTEN__
  // void saveBgColorToLocalStorage(const float* color);
  // void loadBgColorFromLocalStorage(float* color);
#endif
};