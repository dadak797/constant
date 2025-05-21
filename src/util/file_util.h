#pragma once

// Standard library
#include <string>
#include <optional>

namespace FileUtil {

std::optional<std::string> ReadFileToString(const std::string& filePath);  

}