#pragma once

#include "macro/ptr_macro.h"
#include "shader.h"

// Standard library
#include <vector>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

DECLARE_PTR(ShaderProgram)
class ShaderProgram {
 public:
  static ShaderProgramUPtr New(const std::vector<ShaderPtr>& shaders);
  static ShaderProgramUPtr New(const std::string& vertShaderFilename,
                               const std::string& fragShaderFilename);

  ~ShaderProgram();

  uint32_t Get() const { return m_Program; }
  void Use() const;

  void SetUniform(const std::string& name, int value) const;
  void SetUniform(const std::string& name, float value) const;
  void SetUniform(const std::string& name, const glm::vec2& value) const;
  void SetUniform(const std::string& name, const glm::vec3& value) const;
  void SetUniform(const std::string& name, const glm::vec4& value) const;
  void SetUniform(const std::string& name, const glm::mat4& value) const;

 private:
  // Constructor
  ShaderProgram() = default;

  bool link(const std::vector<ShaderPtr>& shaders);

  uint32_t m_Program{0};
};