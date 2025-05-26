#include "size_config.h"

// Emscripten
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

// MacOS
#ifdef __APPLE__
#include "../platform/mac_helper.h"
#endif

// Windows
#ifdef _WIN32
#include "../platform/win_helper.h"
#endif

double SizeConfig::DevicePixelRatio() {
#ifdef __EMSCRIPTEN__
  return emscripten_get_device_pixel_ratio();
#elif defined(__APPLE__)
  // For macOS, we can use the MacHelper::DevicePixelRatio() function.
  return static_cast<double>(MacHelper::DevicePixelRatio());
#elif defined(_WIN32)
  // For Windows, we can use the WinHelper::DevicePixelRatio() function.
  return static_cast<double>(WinHelper::DevicePixelRatio());
#endif
  // TODO: Define DevicePixelRatio() function for Linux if needed.
  return 1.0;
}

float SizeConfig::TextBaseWidth() { return ImGui::CalcTextSize("A").x; }

float SizeConfig::TextBaseHeight() { return ImGui::GetTextLineHeight(); }

float SizeConfig::TextBaseHeightWithSpacing() {
  return ImGui::GetTextLineHeightWithSpacing();
}

float SizeConfig::TitleBarHeight() { return ImGui::GetFrameHeight(); }
