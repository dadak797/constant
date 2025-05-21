#include "shader_program.h"

#include "config/log_config.h"

ShaderProgramUPtr ShaderProgram::New(const std::vector<ShaderPtr>& shaders) {
  auto program = ShaderProgramUPtr(new ShaderProgram());
  if (!program->link(shaders)) {
    return nullptr;
  }
  return std::move(program);
}

ShaderProgramUPtr ShaderProgram::New(const std::string& vertShaderFilename,
                                     const std::string& fragShaderFilename) {
  ShaderPtr vs = Shader::New(vertShaderFilename, GL_VERTEX_SHADER);
  ShaderPtr fs = Shader::New(fragShaderFilename, GL_FRAGMENT_SHADER);
  if (!vs || !fs) {
    return nullptr;
  }
  return New({vs, fs});
}

ShaderProgram::~ShaderProgram() {
  if (m_Program) glDeleteProgram(m_Program);
}

bool ShaderProgram::link(const std::vector<ShaderPtr>& shaders) {
  m_Program = glCreateProgram();
  for (const auto& shader : shaders) {
    glAttachShader(m_Program, shader->Get());
  }
  glLinkProgram(m_Program);

  int success = 0;
  glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetProgramInfoLog(m_Program, 1024, nullptr, infoLog);
    SPDLOG_ERROR("Failed to link program: {}", infoLog);
    return false;
  }

  return true;
}

void ShaderProgram::Use() const { glUseProgram(m_Program); }

void ShaderProgram::SetUniform(const std::string& name, int value) const {
  auto loc = glGetUniformLocation(m_Program, name.c_str());
  glUniform1i(loc, value);
}

void ShaderProgram::SetUniform(const std::string& name, float value) const {
  auto loc = glGetUniformLocation(m_Program, name.c_str());
  glUniform1f(loc, value);
}

void ShaderProgram::SetUniform(const std::string& name,
                               const glm::vec2& value) const {
  auto loc = glGetUniformLocation(m_Program, name.c_str());
  glUniform2fv(loc, 1, glm::value_ptr(value));
}

void ShaderProgram::SetUniform(const std::string& name,
                               const glm::vec3& value) const {
  auto loc = glGetUniformLocation(m_Program, name.c_str());
  glUniform3fv(loc, 1, glm::value_ptr(value));
}

void ShaderProgram::SetUniform(const std::string& name,
                               const glm::vec4& value) const {
  auto loc = glGetUniformLocation(m_Program, name.c_str());
  glUniform4fv(loc, 1, glm::value_ptr(value));
}

void ShaderProgram::SetUniform(const std::string& name,
                               const glm::mat4& value) const {
  auto loc = glGetUniformLocation(m_Program, name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}