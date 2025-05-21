#include "render_material.h"
#include "shader_program.h"

RenderMaterialUPtr RenderMaterial::New(TexturePtr diffuse, TexturePtr specular,
                                 float shininess) {
  auto material = RenderMaterialUPtr(new RenderMaterial());
  material->SetDiffuse(diffuse);
  material->SetSpecular(specular);
  material->SetShininess(shininess);
  return std::move(material);
}

void RenderMaterial::SetToProgram(const ShaderProgram* program) const {
  int textureCount = 0;
  if (m_Diffuse) {
    glActiveTexture(GL_TEXTURE0 + textureCount);
    program->SetUniform("material.diffuse", textureCount);
    m_Diffuse->Bind();
    textureCount++;
  }
  if (m_Specular) {
    glActiveTexture(GL_TEXTURE0 + textureCount);
    program->SetUniform("material.specular", textureCount);
    m_Specular->Bind();
    textureCount++;
  }
  glActiveTexture(GL_TEXTURE0);
  program->SetUniform("material.shininess", m_Shininess);
}

void RenderMaterial::SetDiffuse(TexturePtr diffuse) {
  if (diffuse) {
    m_Diffuse = diffuse;
  }
}

void RenderMaterial::SetSpecular(TexturePtr specular) {
  if (specular) {
    m_Specular = specular;
  }
}

void RenderMaterial::SetShininess(float shininess) {
  if (shininess > 0.0f) {
    m_Shininess = shininess;
  }
}