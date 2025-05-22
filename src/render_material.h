#pragma once

#include "macro/ptr_macro.h"
#include "texture.h"

class ShaderProgram;

DECLARE_PTR(RenderMaterial)
class RenderMaterial {
 public:
  static RenderMaterialUPtr New(TexturePtr diffuse, TexturePtr specular,
                                float shininess = -1.0f);

  ~RenderMaterial() = default;

  // Setters
  void SetDiffuse(TexturePtr diffuse);
  void SetSpecular(TexturePtr specular);
  void SetShininess(float shininess);
  void SetDiffuseColor(const glm::vec3& color) { m_DiffuseColor = color; }
  void SetUseDiffuseTexture(bool use) { m_bUseDiffuseTexture = use; }

  // Getters
  TexturePtr GetDiffuse() const { return m_Diffuse; }
  TexturePtr GetSpecular() const { return m_Specular; }
  float GetShininess() const { return m_Shininess; }

  void SetToProgram(const ShaderProgram* program) const;

 private:
  RenderMaterial() = default;

  TexturePtr m_Diffuse{nullptr};
  TexturePtr m_Specular{nullptr};
  float m_Shininess{32.0f};

  glm::vec3 m_DiffuseColor{glm::vec3(0.8f, 0.8f, 0.8f)};  // Single diffuse color
  bool m_bUseDiffuseTexture{true};  // Use texture or single color
};