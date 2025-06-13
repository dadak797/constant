#include "path_util.h"

std::string PathUtil::GetExtension(const std::string& path) {
  size_t pos = path.find_last_of('.');
  if (pos == std::string::npos) {
    return "";
  }
  return path.substr(pos);
}