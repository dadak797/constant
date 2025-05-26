#include "scene_window.h"

#include "config/gl_config.h"
#include "config/log_config.h"
#include "font_manager.h"

// ImGui
#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>

SceneWindow::SceneWindow() { init(); }

SceneWindow::~SceneWindow() {}

void SceneWindow::init() {
  // Load background color from local storage
#ifdef __EMSCRIPTEN__
  // loadBgColorFromLocalStorage(m_BgColor.data());
#else
  // TODO: Load background color from file
#endif
  m_Box = Mesh::New(MeshType::BOX);
  m_SimpleProgram = ShaderProgram::New("resources/shader/simple.vs",
                                       "resources/shader/simple.fs");
}

void SceneWindow::initFramebuffer() {
  // Create color texture. The old texture is deleted by losing the reference.
  m_ColorTexture = Texture::New(m_Width, m_Height, GL_RGBA);
  if (!m_ColorTexture) {
    SPDLOG_ERROR("Failed to create color texture!");
    return;
  }

  // Create framebuffer. The old framebuffer is deleted by losing the reference.
  m_Framebuffer = Framebuffer::New({m_ColorTexture});
  if (!m_Framebuffer) {
    SPDLOG_ERROR("Failed to create framebuffer!");
    return;
  }

  SPDLOG_DEBUG("Scene framebuffer initialized: ({} x {})", m_Width, m_Height);
}

void SceneWindow::resizeFramebuffer(int32_t width, int32_t height) {
  if (width <= 0 || height <= 0) return;

  m_Width = width;
  m_Height = height;

  initFramebuffer();
}

void SceneWindow::renderMesh() {
  glEnable(GL_DEPTH_TEST);

  m_SimpleProgram->Use();

  // Projection matrix
  float aspectRatio =
      static_cast<float>(m_Width) / static_cast<float>(m_Height);
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

  // View matrix
  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),  // Camera position
                               glm::vec3(0.0f, 0.0f, 0.0f),  // Focal point
                               glm::vec3(0.0f, 1.0f, 0.0f)   // Up vector
  );

  // Model matrix
  float rotationAngle = 30.0f;
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, rotationAngle, glm::vec3(0.5f, 1.0f, 0.0f));

  // Transform matrix
  glm::mat4 transform = projection * view * model;

  // Set uniforms
  m_SimpleProgram->SetUniform("transform", transform);
  m_SimpleProgram->SetUniform("color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));

  // Render the box mesh
  m_Box->Draw(m_SimpleProgram.get());

  glDisable(GL_DEPTH_TEST);
}

void SceneWindow::clearFramebuffer() {
  // Bind scene framebuffer
  m_Framebuffer->Bind();

  glViewport(0, 0, m_Width, m_Height);
  glClearColor(m_BgColor.at(0), m_BgColor.at(1), m_BgColor.at(2), 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderMesh();

  // Bind to default framebuffer
  m_Framebuffer->BindToDefault();
}

void SceneWindow::processEvents() {
  ImGuiIO& io = ImGui::GetIO();

  static bool isDraggingWindow = false;
  static ImVec2 dragStartPos;

  // Position of window orientation
  ImVec2 windowPos = ImGui::GetWindowPos();

  // Viewport position relative to window orientation
  ImVec2 viewportPos = ImGui::GetCursorStartPos();

  // Dragging check
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    dragStartPos = io.MousePos;

    float titleBarHeight =
        ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.y;

    if (io.MousePos.y >= windowPos.y &&
        io.MousePos.y <= windowPos.y + titleBarHeight &&
        io.MousePos.x >= windowPos.x &&
        io.MousePos.x <= windowPos.x + ImGui::GetWindowWidth()) {
      isDraggingWindow = true;
    }
  }

  if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
    isDraggingWindow = false;
  }

  // If the window moves, the event does not occur.
  if (isDraggingWindow) {
    return;
  }

  if (!ImGui::IsWindowFocused() && !ImGui::IsWindowHovered()) {
    return;
  }

  io.ConfigWindowsMoveFromTitleBarOnly = true;

  int32_t xpos =
      static_cast<int32_t>(io.MousePos.x - windowPos.x - viewportPos.x);
  int32_t ypos =
      static_cast<int32_t>(io.MousePos.y - windowPos.y - viewportPos.y);

  int32_t ctrl = static_cast<int32_t>(io.KeyCtrl);
  int32_t shift = static_cast<int32_t>(io.KeyShift);
  bool dclick = io.MouseDoubleClicked[0] || io.MouseDoubleClicked[1] ||
                io.MouseDoubleClicked[2];

  if (ImGui::IsWindowHovered()) {
    if (io.MouseClicked[ImGuiMouseButton_Left]) {
      SPDLOG_DEBUG("Mouse position: ({}, {})", xpos, ypos);
    } else if (io.MouseClicked[ImGuiMouseButton_Right]) {
      ImGui::SetWindowFocus();  // make right-clicks bring window into focus
    } else if (io.MouseWheel > 0) {
    } else if (io.MouseWheel < 0) {
    }
  }

  if (io.MouseReleased[ImGuiMouseButton_Left]) {
  } else if (io.MouseReleased[ImGuiMouseButton_Right]) {
  }
}

void SceneWindow::Render(bool* openWindow) {
  // Remove padding
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

  ImGui::Begin("Scene");

  // Resize framebuffer
  ImVec2 contentSize = ImGui::GetContentRegionAvail();
  if (contentSize.x > 0 && contentSize.y > 0 &&
      (m_Width != (int32_t)contentSize.x ||
       m_Height != (int32_t)contentSize.y)) {
    resizeFramebuffer((int32_t)contentSize.x, (int32_t)contentSize.y);
  }

  // Clear framebuffer with background color
  clearFramebuffer();

  // Draw scene to framebuffer
  ImGui::Image((ImTextureID)(intptr_t)m_ColorTexture->Get(),
               ImVec2(m_Width, m_Height), ImVec2(0, 1),
               ImVec2(1, 0));  // Upside down of the texture y-coordinate

  processEvents();

  ImGui::End();

  ImGui::PopStyleVar();
}

void SceneWindow::RenderBgColorPopup(bool* openWindow) {
  const char* popupTitle = ICON_FA6_PAINT_ROLLER "  Background Color";

  ImGui::OpenPopup(popupTitle);
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  static std::array<float, 3> imguiColor = m_BgColor;

  if (ImGui::BeginPopupModal(popupTitle, openWindow,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    const ImVec2 btnSize = ImVec2(TEXT_BASE_WIDTH * 12, TEXT_BASE_HEIGHT * 1.5);

    ImGui::ColorEdit3("Color", imguiColor.data());

    if (ImGui::Button("Change Color", btnSize)) {
      m_BgColor = imguiColor;

      // saveBgColorToLocalStorage(imguiColor);
    }
    ImGui::SetItemDefaultFocus();  // "OK" button is default focus.

    ImGui::Separator();

    if (ImGui::Button("Black", btnSize)) {
      m_BgColor = {0.0f, 0.0f, 0.0f};
      imguiColor = m_BgColor;

      // saveBgColorToLocalStorage(imguiColor);
    }
    ImGui::SameLine();
    if (ImGui::Button("Skyblue", btnSize)) {
      m_BgColor = {0.2f, 0.4f, 0.9f};
      imguiColor = m_BgColor;

      // saveBgColorToLocalStorage(imguiColor);
    }
    ImGui::SameLine();
    if (ImGui::Button("White", btnSize)) {
      m_BgColor = {1.0f, 1.0f, 1.0f};
      imguiColor = m_BgColor;

      // saveBgColorToLocalStorage(imguiColor);
    }
    ImGui::SameLine();
    if (ImGui::Button("Gray", btnSize)) {
      m_BgColor = {0.625f, 0.625f, 0.625f};
      imguiColor = m_BgColor;

      // saveBgColorToLocalStorage(imguiColor);
    }

    ImGui::EndPopup();
  }

  // If the window is closed by 'X' button,
  // return imguiColor to the current background color.
  if (*openWindow == false) {
    imguiColor = m_BgColor;

    ImGui::CloseCurrentPopup();
  }
}