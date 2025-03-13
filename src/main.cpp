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

    // For the first window, run this callback once
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Set glfw callbacks
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);

    // Main loop
    SPDLOG_DEBUG("Start main loop");
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        glfwPollEvents();
        Render();
        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    glfwTerminate();

    App::ShutdownLogger();

    return 0;
}
