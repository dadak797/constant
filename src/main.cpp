#include "logger_config.h"
#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    #include <emscripten/html5.h>
    #define GLFW_INCLUDE_ES3    // Include OpenGL ES 3.0 headers
    #define GLFW_INCLUDE_GLEXT  // Include to OpenGL ES extension headers
#else  
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#ifdef __EMSCRIPTEN__
    #include <functional>
    static std::function<void()>            MainLoopForEmscriptenP;
    static void MainLoopForEmscripten()     { MainLoopForEmscriptenP(); }
    #define EMSCRIPTEN_MAINLOOP_BEGIN       MainLoopForEmscriptenP = [&]() { do
    #define EMSCRIPTEN_MAINLOOP_END         while (0); }; emscripten_set_main_loop(MainLoopForEmscripten, 0, true)
#else
    #define EMSCRIPTEN_MAINLOOP_BEGIN
    #define EMSCRIPTEN_MAINLOOP_END
#endif

// Scene parameters
GLuint sceneFramebuffer = 0;
GLuint sceneColorTexture = 0;
GLuint sceneDepthTexture = 0;
int sceneWidth = 800;
int sceneHeight = 600;

void InitSceneFramebuffer() {
    if (sceneFramebuffer != 0) {
        glDeleteFramebuffers(1, &sceneFramebuffer);
        glDeleteTextures(1, &sceneColorTexture);
        glDeleteTextures(1, &sceneDepthTexture);
    }

    // Create framebuffer
    glGenFramebuffers(1, &sceneFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);

    // Create color texture
    glGenTextures(1, &sceneColorTexture);
    glBindTexture(GL_TEXTURE_2D, sceneColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sceneWidth, sceneHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColorTexture, 0);

    // Create depth texture
    glGenTextures(1, &sceneDepthTexture);
    glBindTexture(GL_TEXTURE_2D, sceneDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, sceneWidth, sceneHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sceneDepthTexture, 0);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("Scene framebuffer is not complete!");
    }

    // Bind to default buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    SPDLOG_DEBUG("Scene framebuffer initialized: ({} x {})", sceneWidth, sceneHeight);
}

void ResizeSceneFramebuffer(int width, int height) {
    if (width <= 0 || height <= 0)
        return;

    sceneWidth = width;
    sceneHeight = height;
    
    InitSceneFramebuffer();
}

void RenderScene() {
    // Bind scene framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);
    glViewport(0, 0, sceneWidth, sceneHeight);

    // Render background
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Bind to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CleanupSceneFramebuffer() {
    if (sceneFramebuffer != 0) {
        glDeleteFramebuffers(1, &sceneFramebuffer);
        glDeleteTextures(1, &sceneColorTexture);
        glDeleteTextures(1, &sceneDepthTexture);
        sceneFramebuffer = 0;
        sceneColorTexture = 0;
        sceneDepthTexture = 0;
    }
}

void ShowSceneWindow() {
    // Remove padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Scene");
    
    // Resize framebuffer
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    if (contentSize.x > 0 && contentSize.y > 0 && 
        (sceneWidth != (int)contentSize.x || sceneHeight != (int)contentSize.y)) {
        ResizeSceneFramebuffer((int)contentSize.x, (int)contentSize.y);
    }
    
    // Draw scene to framebuffer
    ImGui::Image((ImTextureID)(intptr_t)sceneColorTexture, 
                 ImVec2(sceneWidth, sceneHeight), 
                 ImVec2(0, 1), ImVec2(1, 0));  // Upside down of the texture y-coordinate
    
    ImGui::End();

    ImGui::PopStyleVar();
}

void OnGlfwError(int errorCode, const char* description)
{
    SPDLOG_ERROR("GLFW error (code {}}): {}", errorCode, description);
    
    switch (errorCode)
    {
    case GLFW_NOT_INITIALIZED:
        SPDLOG_ERROR("GLFW has not been initialized");
        break;
    case GLFW_NO_CURRENT_CONTEXT:
        SPDLOG_ERROR("No OpenGL context is current"); 
        break;
    case GLFW_INVALID_ENUM:
        SPDLOG_ERROR("Invalid enum value was passed");
        break;
    case GLFW_INVALID_VALUE:
        SPDLOG_ERROR("Invalid value was passed");
        break;
    case GLFW_OUT_OF_MEMORY:
        SPDLOG_ERROR("Memory allocation failed");
        break;
    case GLFW_API_UNAVAILABLE:
        SPDLOG_ERROR("The requested API is unavailable");
        break;
    case GLFW_VERSION_UNAVAILABLE:
        SPDLOG_ERROR("The requested OpenGL version is unavailable");
        break;
    case GLFW_PLATFORM_ERROR:
        SPDLOG_ERROR("A platform-specific error occurred");
        break;
    case GLFW_FORMAT_UNAVAILABLE:
        SPDLOG_ERROR("The requested format is unavailable");
        break;
    }
}

void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_DEBUG("framebuffer size changed: ({} x {})", width, height);
    glViewport(0, 0, width, height);
}

void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    SPDLOG_DEBUG("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
}

void OnCharEvent(GLFWwindow* window, unsigned int ch) {
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScrollEvent(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void OnMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void OnMouseMoveEvent(GLFWwindow* window, double xpos, double ypos)
{
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

void SetupDockSpace(bool* p_open) {
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    } else {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("New", NULL, &opt_fullscreen);
            ImGui::MenuItem("Load", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
                *p_open = false;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}

int main()
{
    App::InitLogger();

    SPDLOG_DEBUG("Start program");

    // Initialize glfw
    SPDLOG_DEBUG("Initialize glfw");
    if (!glfwInit()) {
        glfwSetErrorCallback(OnGlfwError);
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef __EMSCRIPTEN__
    glfwWindowHint(GLFW_SAMPLES, 4);  // For MSAA
#endif

    SPDLOG_DEBUG("Create glfw window");
    int width, height;
#ifdef __EMSCRIPTEN__ 
    // For wasm, width and height can be obtained from canvas.
    emscripten_get_canvas_element_size("canvas", &width, &height);
    auto window = glfwCreateWindow(width, height, WINDOW_NAME, nullptr, nullptr);
#else        
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
        nullptr, nullptr);
#endif
    if (!window) {
        SPDLOG_ERROR("Failed to create glfw window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

#ifndef __EMSCRIPTEN__
    // Load OpenGL functions using glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("Failed to initialize glad");
        glfwTerminate();
        return -1;
    }
#endif
    auto glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    SPDLOG_DEBUG("OpenGL context version: {}", glVersion);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoAutoMerge = false;
    io.ConfigViewportsNoTaskBarIcon = true;
    io.ConfigDockingTransparentPayload = true;

    io.Fonts->AddFontFromFileTTF("./resources/font/NotoSansKR-Light.ttf", 15.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.FramePadding = ImVec2(5.0f, 3.0f);
        style.FrameRounding = 3.0f;
        style.TabRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabRounding = 3.0f;
    }

#ifdef __EMSCRIPTEN__
    const float dpRatio = emscripten_get_device_pixel_ratio();
    io.FontGlobalScale = dpRatio;
    style.ScaleAllSizes(dpRatio);
#endif

#ifdef __EMSCRIPTEN__
    const char* glsl_version = "#version 300 es";    // OpenGL ES for WebGL
#else
    const char* glsl_version = "#version 330 core";  // Native OpenGL
#endif

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Scene 프레임버퍼 초기화
    InitSceneFramebuffer();

#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
#endif

    // For the first window, run this callback once
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Set glfw callbacks
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetCharCallback(window, OnCharEvent);
    glfwSetCursorPosCallback(window, OnMouseMoveEvent);
    glfwSetMouseButtonCallback(window, OnMouseButtonEvent);
    glfwSetScrollCallback(window, OnScrollEvent);

    // Main loop
    SPDLOG_DEBUG("Start main loop");
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool dockspaceOpen = true;
        SetupDockSpace(&dockspaceOpen);
        
        RenderScene();
        ShowSceneWindow();

        static bool show_demo_window = true;
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Begin("My Window");
        ImGui::Text("Hello, world!");
        ImGui::End();

        ImGui::Render();
        
        // Clear default framebuffer to black
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    CleanupSceneFramebuffer();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    App::ShutdownLogger();

    return 0;
}