#pragma once

#include "macro/singleton_macro.h"
#include "config/log_config.h"


// Application class
// - Initialize logger
// - Initialize IdbFS
// - Render Dockspace & Menu
// - Render all ImGui windows
// - Save/Load ImGui settings
// - Set ImGui style
class App {
    DECLARE_SINGLETON(App)

public:
    enum class ImGuiStyle {
        Dark = 0,
        Light = 1,
        Classic = 2
    };

#ifdef __EMSCRIPTEN__
    static void InitIdbfs();  // Mount IdbFS
    static void SaveImGuiIniFile();
    static void LoadImGuiIniFile();
#endif

    void SetInitStyle();
    static void SetStyle(ImGuiStyle style) { Instance().setStyle(style); }
    static ImGuiStyle GetStyle() { return Instance().getStyle(); }

    void InitImGuiWindows();
    void Render();

private:
    const char* IDBFS_MOUNT_PATH = "/settings";
    const char* IMGUI_SETTING_FILE_PATH = "/settings/imgui.ini";
    const char* IMGUI_STYLE_KEY = "App-ImguiStyle";

    ImGuiStyle m_ImGuiStyle { ImGuiStyle::Dark };

    bool m_bFullDockSpace = true;
#ifdef SHOW_IMGUI_DEMO
    bool m_bShowDemoWindow = false;
#endif
#ifdef SHOW_FONT_ICONS
    bool m_bShowFontIcons = true;
#endif
    bool m_bShowSceneWindow = true;
    bool m_bShowBgColorPopup = false;

    // useConsole: true - log to console, false - log to file
    // newFile: true - create new log file, false - append to the previous log file
    // If useConsole is true, newFile is ignored.
    void initLogger(bool useConsole = true, bool newFile = true);
    void shutdownLogger();

    void setStyle(ImGuiStyle style);
    ImGuiStyle getStyle() const { return m_ImGuiStyle; }

    void renderDockSpaceAndMenu();
    void renderImGuiWindows();
};