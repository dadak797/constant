#include "shader.h"

#include "config/gl_config.h"
#include "config/log_config.h"
#include "util/file_util.h"

// Standard library
#include <fstream>
#include <sstream>

ShaderUPtr Shader::New(const std::string& filename, GLenum shaderType) {
  auto shader = ShaderUPtr(new Shader());
  if (!shader->loadFile(filename, shaderType)) {
    return nullptr;
  }
  return std::move(shader);
}

Shader::~Shader() {
  if (m_Shader) {
    glDeleteShader(m_Shader);
  }
}

bool Shader::loadFile(const std::string& filename, GLenum shaderType) {
  auto content = FileUtil::ReadFileToString(filename);
  if (!content.has_value()) {
    return false;
  }

  auto& code = content.value();
#ifdef __EMSCRIPTEN__
  // Convert GLSL header
  // #version 330 core -> #version 300 es\nprecision mediump float;
  std::string version = "#version 330 core";
  int start = code.find(version);
  code.replace(start, version.size(),
               "#version 300 es\nprecision highp float;\n"
               "precision mediump int;\n"
               "precision highp sampler2D;");  // For better quality,
                                               // change mediump into
                                               // highp
#endif
  const char* codePtr = code.c_str();
  int32_t codeLength = (int32_t)code.length();

  // Create and compile shader
  m_Shader = glCreateShader(shaderType);
  glShaderSource(m_Shader, 1, (const GLchar* const*)&codePtr, &codeLength);
  glCompileShader(m_Shader);

  // Check compile error
  int32_t success = 0;
  glGetShaderiv(m_Shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(m_Shader, 1024, nullptr, infoLog);
    SPDLOG_ERROR("Failed to compile shader: {}", filename);
    SPDLOG_ERROR("Reason: {}", infoLog);
    return false;
  }

  return true;
}