#pragma once

#include "config/log_config.h"
#include "enum/app_enums.h"
#include "macro/singleton_macro.h"

// ImGui
#include <imgui.h>

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
#ifdef __EMSCRIPTEN__
  static void InitIdbfs();  // Mount IdbFS
  static void SaveImGuiIniFile();
  static void LoadImGuiIniFile();
#endif

  void SetInitStyle();
  static void SetStyle(AppStyle style) { Instance().setStyle(style); }
  static AppStyle GetStyle() { return Instance().getStyle(); }

  void InitImGuiWindows();
  void Render();

 private:
  const char* IDBFS_MOUNT_PATH = "/settings";
  const char* IMGUI_SETTING_FILE_PATH = "/settings/imgui.ini";
  const char* APP_STYLE_KEY = "App-Style";

  AppStyle m_Style{AppStyle::DARK};

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
  // newFile: true - create new log file, false - append to the previous log
  // file If useConsole is true, newFile is ignored.
  void initLogger(bool useConsole = true, bool newFile = true);
  void shutdownLogger();

  void setStyle(AppStyle style);
  AppStyle getStyle() const { return m_Style; }

  void renderDockSpaceAndMenu();
  void renderImGuiWindows();
};