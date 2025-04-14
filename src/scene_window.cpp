#include "scene_window.h"
#include "config/log_config.h"
#include "font_manager.h"

// GLFW
#ifdef __EMSCRIPTEN__
#define GLFW_INCLUDE_ES3    // Include OpenGL ES 3.0 headers
#define GLFW_INCLUDE_GLEXT  // Include to OpenGL ES extension headers
#else  
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

// ImGui
#include <imgui.h>


SceneWindow::SceneWindow() {
    init();
}

SceneWindow::~SceneWindow() {
    if (m_Framebuffer != 0) {
        glDeleteFramebuffers(1, &m_Framebuffer);
        glDeleteTextures(1, &m_ColorTexture);
        // glDeleteTextures(1, &m_DepthTexture);
    }
}

void SceneWindow::init() {
    // Load background color from local storage
#ifdef __EMSCRIPTEN__
    // loadBgColorFromLocalStorage(m_BgColor.data());
#else
    // TODO: Load background color from file
#endif
}

void SceneWindow::initFramebuffer() {
    if (m_Framebuffer != 0) {
        glDeleteFramebuffers(1, &m_Framebuffer);
        glDeleteTextures(1, &m_ColorTexture);
        // glDeleteTextures(1, &m_DepthTexture);
    }

    // Create framebuffer
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    // Create color texture
    glGenTextures(1, &m_ColorTexture);
    glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture, 0);

    // Create depth texture
    // glGenTextures(1, &m_DepthTexture);
    // glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("Scene framebuffer is not complete!");
    }

    // Bind to default buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    SPDLOG_DEBUG("Scene framebuffer initialized: ({} x {})", m_Width, m_Height);  
}

void SceneWindow::resizeFramebuffer(int32_t width, int32_t height) {
    if (width <= 0 || height <= 0)
        return;

    m_Width = width;
    m_Height = height;
    
    initFramebuffer();
}

void SceneWindow::clearFramebuffer() {
    // Bind scene framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glViewport(0, 0, m_Width, m_Height);

    // Render background
    glClearColor(m_BgColor.at(0), m_BgColor.at(1), m_BgColor.at(2), 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Bind to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

        float titleBarHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.y;
        
        if (io.MousePos.y >= windowPos.y && io.MousePos.y <= windowPos.y + titleBarHeight &&
            io.MousePos.x >= windowPos.x && io.MousePos.x <= windowPos.x + ImGui::GetWindowWidth()) {
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

    int xpos = static_cast<int>(io.MousePos.x - windowPos.x - viewportPos.x);
    int ypos = static_cast<int>(io.MousePos.y - windowPos.y - viewportPos.y);

    int ctrl = static_cast<int>(io.KeyCtrl);
    int shift = static_cast<int>(io.KeyShift);
    bool dclick = io.MouseDoubleClicked[0] || io.MouseDoubleClicked[1] || io.MouseDoubleClicked[2];

    if (ImGui::IsWindowHovered()) {
        if (io.MouseClicked[ImGuiMouseButton_Left]) {
            SPDLOG_DEBUG("Mouse position: ({}, {})", xpos, ypos);
        }
        else if (io.MouseClicked[ImGuiMouseButton_Right]) {
            ImGui::SetWindowFocus();  // make right-clicks bring window into focus
        }
        else if (io.MouseWheel > 0) {
        }
        else if (io.MouseWheel < 0) {
        }
    }

    if (io.MouseReleased[ImGuiMouseButton_Left]) {
    }
    else if (io.MouseReleased[ImGuiMouseButton_Right]) {
    }
}

void SceneWindow::Render(bool* openWindow) {
    // Remove padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Scene");

    // Resize framebuffer
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    if (contentSize.x > 0 && contentSize.y > 0 && 
        (m_Width != (int32_t)contentSize.x || m_Height != (int32_t)contentSize.y)) {
        resizeFramebuffer((int32_t)contentSize.x, (int32_t)contentSize.y);
    }

    // Clear framebuffer with background color
    clearFramebuffer();

    // Draw scene to framebuffer
    ImGui::Image((ImTextureID)(intptr_t)m_ColorTexture, 
                  ImVec2(m_Width, m_Height), 
                  ImVec2(0, 1), ImVec2(1, 0));  // Upside down of the texture y-coordinate

    processEvents();

    ImGui::End();

    ImGui::PopStyleVar();
}

void SceneWindow::RenderBgColorPopup(bool* openWindow) {
    const char* popupTitle = ICON_FA6_PAINT_ROLLER"  Background Color";

    ImGui::OpenPopup(popupTitle);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    static std::array<float, 3> imguiColor = m_BgColor;

    if (ImGui::BeginPopupModal(popupTitle, openWindow, ImGuiWindowFlags_AlwaysAutoResize)) {
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
            m_BgColor = { 0.0f, 0.0f, 0.0f };
            imguiColor = m_BgColor;

            // saveBgColorToLocalStorage(imguiColor);
        }
        ImGui::SameLine();
        if (ImGui::Button("Skyblue", btnSize))
        {
            m_BgColor = { 0.2f, 0.4f, 0.9f };
            imguiColor = m_BgColor;

            // saveBgColorToLocalStorage(imguiColor);
        }
        ImGui::SameLine();
        if (ImGui::Button("White", btnSize))
        {
            m_BgColor = { 1.0f, 1.0f, 1.0f };
            imguiColor = m_BgColor;

            // saveBgColorToLocalStorage(imguiColor);
        }
        ImGui::SameLine();
        if (ImGui::Button("Gray", btnSize))
        {
            m_BgColor = { 0.625f, 0.625f, 0.625f };
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