#include "file_util.h"
#include "../config/log_config.h"

// Standard library
#include <fstream>
#include <sstream>

std::optional<std::string> FileUtil::ReadFileToString(
    const std::string& filePath) {
  std::ifstream file(filePath);
  std::string content;

  if (!file.is_open()) {
    SPDLOG_ERROR("Failed to open file: {}", filePath);
    return std::nullopt;  // Return an empty optional
  }

  file.seekg(0, std::ios::end);   // Move to the end of the file
  content.reserve(file.tellg());  // Allocate memory for the content
  file.seekg(0, std::ios::beg);   // Move back to the beginning of the file

  content.assign(std::istreambuf_iterator<char>(file),
                 std::istreambuf_iterator<char>());
  file.close();

  return content;
}