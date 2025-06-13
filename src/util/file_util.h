#pragma once

// Standard library
#include <optional>
#include <string>

namespace FileUtil {

std::optional<std::string> ReadFileToString(const std::string& filePath);

}