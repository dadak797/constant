#pragma once

#include "config/gl_config.h"
#include "macro/ptr_macro.h"

// Standard library
#include <string>

DECLARE_PTR(Shader)
class Shader {
 public:
  static ShaderUPtr New(const std::string& fileName, GLenum shaderType);

  ~Shader();

  uint32_t Get() const { return m_Shader; }

 private:
  Shader() = default;

  bool loadFile(const std::string& filename, GLenum shaderType);

  uint32_t m_Shader{0};
};