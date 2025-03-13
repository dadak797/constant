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
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
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

void Render() {
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.Fonts->AddFontFromFileTTF("./resources/font/Roboto-Light.ttf", 13.0f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
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
    ImGui_ImplGlfw_InitForOpenGL(window, false);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

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

        Render();

        static bool show_demo_window = true;
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Render();
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

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    App::ShutdownLogger();

    return 0;
}
