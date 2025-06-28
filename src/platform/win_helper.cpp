// Windows api headers
#include <windows.h>
#include <commdlg.h>
#include <shellscalingapi.h>  // for GetDpiForMonitor

#include "win_helper.h"

#pragma comment(lib, "Shcore.lib")

std::vector<std::string> WinHelper::OpenFileDialog() {
  std::vector<std::string> fileList;

  char buffer[4096] = {0};

  OPENFILENAMEA ofn;
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFile = buffer;
  ofn.nMaxFile = sizeof(buffer);
  ofn.lpstrFilter =
      "All Files\0*.*\0Text Files\0*.txt\0Images\0*.jpg;*.png;*.bmp\0";
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

  if (GetOpenFileNameA(&ofn)) {
    std::string directory = buffer;
    char* ptr = buffer + directory.length() + 1;

    if (*ptr == '\0') {
      // For single file selection
      fileList.push_back(directory);
    } else {
      // For multiple file selection
      while (*ptr) {
        std::string filename = ptr;
        fileList.push_back(directory + "\\" + filename);
        ptr += filename.length() + 1;
      }
    }
  }

  return fileList;
}

float WinHelper::DevicePixelRatio() {
  HMONITOR hMonitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
  UINT dpiX = 96, dpiY = 96;

  HRESULT hr = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
  if (SUCCEEDED(hr)) {
    return dpiX / 96.0f;
  }
  return 1.0f;
}