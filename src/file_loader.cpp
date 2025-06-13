#include "file_loader.h"

#include "config/log_config.h"

// Standard library
#include <fstream>

// Emscripten
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#ifdef __APPLE__
#include "platform/mac_helper.h"
#endif

#ifdef _WIN32
#include "platform/win_helper.h"
#endif

FileLoader::FileLoader() {}

FileLoader::~FileLoader() {}

void FileLoader::OpenFileBrowser() {
#ifdef __EMSCRIPTEN__
  EM_ASM({
    let fileInput = document.createElement('input');
    fileInput.type = 'file';
    fileInput.multiple = false;  // Get single file. TODO: Get multiple files
    fileInput.accept = '.obj';   // Set possible extensions
    fileInput.onchange = () => {
      if (fileInput.files.length == 0) {
        return;
      }
      const file = fileInput.files[0];  // Get the first file
      const reader = new FileReader();
      reader.readAsArrayBuffer(file);
      reader.onload = () => {
        const data = new Uint8Array(reader.result);

        // Use ArrayBuffer directly without copying data
        // FS.createDataFile(parent, name, data, canRead, canWrite, canOwn);
        // If canOwn is true, the ownership of data is transferred to MemFS
        Constant.FS.createDataFile('/', file.name, data, true, false, true);

        // deleteFile: The file in MemFS is deleted inside this function.
        Constant.loadArrayBuffer(file.name, true);
      };
    };
    fileInput.click();
  });
#elif defined(__APPLE__)
  std::vector<std::string> filePaths = MacHelper::OpenFileDialog();
  for (const auto& filePath : filePaths) {
    LoadArrayBuffer(filePath, false);
  }
#elif defined(_WIN32)
  std::vector<std::string> filePaths = WinHelper::OpenFileDialog();
  for (const auto& filePath : filePaths) {
    LoadArrayBuffer(filePath, false);
  }
#endif
}

void FileLoader::LoadArrayBuffer(const std::string& fileName, bool deleteFile) {
  std::ifstream file(fileName, std::ios::binary | std::ios::ate);

  std::string s;
  if (file.is_open()) {
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    s.resize(size);
    file.seekg(0, std::ios::beg);
    file.read(&s[0], size);
    SPDLOG_INFO("Read {} bytes from {}", size, fileName);
    SPDLOG_INFO("File content: {}", s);
  }

  // Check to binary or not
  // bool isBinary = isBinaryFile(pData, dataLength);

  // if (isBinary)
  // {
  //     // TODO: Read binary file
  // }
  // else {
  //     // TODO: Read ascii file
  // }

  if (deleteFile) {
    if (std::remove(fileName.c_str()) != 0) {
      SPDLOG_ERROR("Failed to remove {} in MemFs", fileName);
    } else {
      SPDLOG_DEBUG("Removed {} in MemFs", fileName);
    }
  }
}