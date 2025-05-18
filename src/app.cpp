#include "app.h"
#include "font_manager.h"
#include "file_loader.h"
#include "scene_window.h"

// ImGui
#include <imgui.h>

// Standard library
#include <fstream>

// Emscripten
#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#endif

// MacOS
#ifdef __APPLE__
    #include "mac_helper.h"
#endif

// Third-party libraries
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>


App::App() {
    initLogger();
}

App::~App() {
    shutdownLogger();
}

void App::initLogger(bool useConsole, bool newFile) {
    if (useConsole) {
        auto console = spdlog::stdout_color_mt("console");
        spdlog::set_default_logger(console);
    }
    else {
        auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/vtk-workbench.log", newFile);
        spdlog::set_default_logger(file_logger);
    }
#ifdef DEBUG_BUILD
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
#else
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%^%l%$] %v");
#endif
    SPDLOG_DEBUG("Logger initialized");
}

void App::shutdownLogger() {
    spdlog::shutdown();
    SPDLOG_DEBUG("Logger shutdown");
}

#ifdef __EMSCRIPTEN__
void App::InitIdbfs() {
    const char* mountPath = Instance().IDBFS_MOUNT_PATH;

    EM_ASM({
        const path = UTF8ToString($0);
        Constant.FS.mkdir(path);
        Constant.FS.mount(IDBFS, {}, path); 
        console.log(`IDBFS mounted on "${path}"`);
    }, mountPath);
}

void App::SaveImGuiIniFile() {
    const char* filePath = Instance().IMGUI_SETTING_FILE_PATH;

    std::ofstream outfile;
    outfile.open(filePath, std::ios::out);

    // Get ImGui setting data
    size_t dataSize = 0;
    const char* data = ImGui::SaveIniSettingsToMemory(&dataSize);

    outfile.write(data, dataSize);
    outfile.close();
}

void App::LoadImGuiIniFile() {
    const char* filePath = Instance().IMGUI_SETTING_FILE_PATH;

    std::ifstream fin(filePath, std::ifstream::in);
    if (!fin) {
        SPDLOG_ERROR("Failed to find ImGui setting file!");
        return;
    }

    std::ostringstream ss;
    ss << fin.rdbuf();
    fin.close();
    std::string fileContents = ss.str();

    ImGui::LoadIniSettingsFromMemory(fileContents.c_str(), fileContents.size());
}
#endif

void App::setStyle(ImGuiStyle style) {
    m_ImGuiStyle = style;

    switch (style) {
        case ImGuiStyle::Dark:
            ImGui::StyleColorsDark();
            break;
        case ImGuiStyle::Light:
            ImGui::StyleColorsLight();
            break;
        case ImGuiStyle::Classic:
            ImGui::StyleColorsClassic();
            break;
    }

#ifdef __EMSCRIPTEN__
    // Save viewer style on local storage
    EM_ASM({
        const styleKey = UTF8ToString($0);
        const style = $1;
        localStorage.setItem(styleKey, style);
    }, IMGUI_STYLE_KEY, static_cast<int>(m_ImGuiStyle));
#endif
}

void App::SetInitStyle() {
#ifdef __EMSCRIPTEN__
    int viewerStyle = EM_ASM_INT({
        const styleKey = UTF8ToString($0);
        const style = localStorage.getItem(styleKey);
        if (style) {
            return style;
        }
        else {         // If not found in local storage, set default style.
            return 0;  // 0 is dark style.
        }
    }, IMGUI_STYLE_KEY);

    ImGuiStyle initStyle = static_cast<ImGuiStyle>(viewerStyle);
    setStyle(initStyle);
#endif
}

void App::Render() {
    renderDockSpaceAndMenu();
    renderImGuiWindows();
}

void App::renderDockSpaceAndMenu() {
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    FontManager& fontManager = FontManager::Instance();

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (m_bFullDockSpace) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    } else {
        dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        windowFlags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, windowFlags);
    ImGui::PopStyleVar();

    if (m_bFullDockSpace)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu(ICON_FA6_COPYRIGHT)) {
            ImGui::MenuItem("About Constant", nullptr, false, false);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem(ICON_FA6_FILE"  New", nullptr, false, false);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Mesh")) {
            if (ImGui::MenuItem(ICON_FA6_FILE_IMPORT"  Import")) {
                FileLoader::Instance().OpenFileBrowser();
            };
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            if (ImGui::BeginMenu("Style"))
            {
                if (ImGui::MenuItem("Dark", nullptr, m_ImGuiStyle == ImGuiStyle::Dark))
                {
                    SetStyle(ImGuiStyle::Dark);
                }
                if (ImGui::MenuItem("Light", nullptr, m_ImGuiStyle == ImGuiStyle::Light))
                {
                    SetStyle(ImGuiStyle::Light);
                }
                if (ImGui::MenuItem("Classic", nullptr, m_ImGuiStyle == ImGuiStyle::Classic))
                {
                    SetStyle(ImGuiStyle::Classic);
                }
                ImGui::EndMenu();
            }
            ImGui::MenuItem("Background Color", nullptr, &m_bShowBgColorPopup);
            
        #ifdef __EMSCRIPTEN__
            ImGui::Separator();

            if (ImGui::MenuItem("Full Screen")) {
                EM_ASM({
                    const canvas = document.getElementById("canvas");
                    canvas.requestFullscreen();
                });
            }
        #endif
            ImGui::EndMenu();
        }
        bool openWindows = ImGui::BeginMenu("Windows");
        if (openWindows) {
            ImGui::MenuItem("Scene", nullptr, &m_bShowSceneWindow);
        #ifdef DEBUG_BUILD
            ImGui::MenuItem("Full Dockspace", nullptr, &m_bFullDockSpace);
        #endif
        #ifdef SHOW_IMGUI_DEMO
            ImGui::MenuItem("Show Demo Window", nullptr, &m_bShowDemoWindow);
        #endif
        #ifdef SHOW_FONT_ICONS
            ImGui::MenuItem("Show Font Icons", nullptr, &m_bShowFontIcons);
        #endif
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

void App::InitImGuiWindows() {
    SceneWindow& sceneWindow = SceneWindow::Instance();
}

void App::renderImGuiWindows() {
    if (m_bShowSceneWindow) SceneWindow::Instance().Render();
#ifdef SHOW_IMGUI_DEMO
    if (m_bShowDemoWindow) ImGui::ShowDemoWindow(&m_bShowDemoWindow);
#endif
#ifdef SHOW_FONT_ICONS
    if (m_bShowFontIcons) FontManager::Instance().Render();
#endif
    if (m_bShowBgColorPopup) SceneWindow::Instance().RenderBgColorPopup(&m_bShowBgColorPopup);
}


double App::DevicePixelRatio() {
#ifdef __EMSCRIPTEN__
    return emscripten_get_device_pixel_ratio();
#elif defined(__APPLE__)
    // For macOS, we can use the MacHelper::DevicePixelRatio() function.
    return MacHelper::DevicePixelRatio();
#endif
    // TODO: Define this function for other platforms if needed.
}

float App::TextBaseWidth() {
    return ImGui::CalcTextSize("A").x;
}

float App::TextBaseHeight() {
    return ImGui::GetTextLineHeight();
}

float App::TextBaseHeightWithSpacing() {
    return ImGui::GetTextLineHeightWithSpacing();
}

float App::TitleBarHeight() {
    return ImGui::GetFrameHeight();
}