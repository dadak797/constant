#pragma once

#include "buffer.h"
#include "macro/ptr_macro.h"
#include "render_material.h"
#include "vertex_layout.h"

// Standard library
#include <vector>

class ShaderProgram;

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;
  glm::vec3 tangent;
};

DECLARE_PTR(Mesh)
class Mesh {
 public:
  static MeshUPtr New(std::vector<Vertex>&& vertices,
                      std::vector<uint32_t>&& indices,
                      uint32_t primitiveType);
  static MeshUPtr CreateBox();
  static MeshUPtr CreateSphere(uint32_t latiSegmentCount = 16,
                               uint32_t longiSegmentCount = 32);

  ~Mesh();

  const VertexLayout* GetVertexLayout() const { return m_VertexLayout.get(); }
  BufferPtr GetVertexBuffer() const { return m_VertexBuffer; }
  BufferPtr GetIndexBuffer() const { return m_IndexBuffer; }
  RenderMaterialPtr GetMaterial() const { return m_Material; }

  void SetMaterial(RenderMaterialPtr material) { m_Material = material; }

  void Draw(const ShaderProgram* program) const;

  static void ComputeTangents(std::vector<Vertex>& vertices,
                              const std::vector<uint32_t>& indices);

 private:
  Mesh() = default;

  uint32_t m_PrimitiveType{GL_TRIANGLES};
  VertexLayoutUPtr m_VertexLayout;
  BufferPtr m_VertexBuffer;
  BufferPtr m_IndexBuffer;

  RenderMaterialPtr m_Material;

  std::vector<Vertex> m_Vertices;
  std::vector<uint32_t> m_Indices;

  void init(std::vector<Vertex>&& vertices,
            std::vector<uint32_t>&& indices, uint32_t primitiveType);
};