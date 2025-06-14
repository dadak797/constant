#include "mesh.h"

#include "config/log_config.h"
#include "shader_program.h"

MeshUPtr Mesh::New(std::vector<Vertex>&& vertices,
                   std::vector<uint32_t>&& indices, uint32_t primitiveType) {
  auto mesh = MeshUPtr(new Mesh());
  if (vertices.empty() || indices.empty()) {
    SPDLOG_ERROR("Vertices or indices are empty");
    return nullptr;
  }
  mesh->init(std::move(vertices), std::move(indices), primitiveType);
  return std::move(mesh);
}

Mesh::~Mesh() {}

void Mesh::init(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices,
                uint32_t primitiveType) {
  m_Vertices = std::move(vertices);
  m_Indices = std::move(indices);
  m_PrimitiveType = primitiveType;

  if (m_PrimitiveType == GL_TRIANGLES) {
    ComputeTangents(m_Vertices, m_Indices);
  }

  // NOTE: The order should be as follows:
  // 1. Vertex layout binding
  // 2. Vertex buffer binding
  // 3. Vertex attribute setting
  m_VertexLayout = VertexLayout::New();
  m_VertexBuffer =
      Buffer::New(GL_ARRAY_BUFFER, GL_STATIC_DRAW, m_Vertices.data(),
                  sizeof(Vertex), m_Vertices.size());
  m_IndexBuffer =
      Buffer::New(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, m_Indices.data(),
                  sizeof(uint32_t), m_Indices.size());
  m_VertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
  m_VertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex),
                            offsetof(Vertex, normal));
  m_VertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex),
                            offsetof(Vertex, texCoord));
  m_VertexLayout->SetAttrib(3, 3, GL_FLOAT, false, sizeof(Vertex),
                            offsetof(Vertex, tangent));
}

void Mesh::ComputeTangents(std::vector<Vertex>& vertices,
                           const std::vector<uint32_t>& indices) {
  auto compute = [](const glm::vec3& pos1, const glm::vec3& pos2,
                    const glm::vec3& pos3, const glm::vec2& uv1,
                    const glm::vec2& uv2, const glm::vec2& uv3) -> glm::vec3 {
    auto edge1 = pos2 - pos1;
    auto edge2 = pos3 - pos1;
    auto deltaUV1 = uv2 - uv1;
    auto deltaUV2 = uv3 - uv1;
    float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    if (det != 0.0f) {
      auto invDet = 1.0f / det;
      return invDet * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    } else {
      return glm::vec3(0.0f, 0.0f, 0.0f);
    }
  };

  // Initialize
  std::vector<glm::vec3> tangents(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

  // accumulate triangle tangents to each vertex
  for (size_t i = 0; i < indices.size(); i += 3) {
    auto v1 = indices[i];
    auto v2 = indices[i + 1];
    auto v3 = indices[i + 2];

    tangents[v1] += compute(vertices[v1].position, vertices[v2].position,
                            vertices[v3].position, vertices[v1].texCoord,
                            vertices[v2].texCoord, vertices[v3].texCoord);

    tangents[v2] = compute(vertices[v2].position, vertices[v3].position,
                           vertices[v1].position, vertices[v2].texCoord,
                           vertices[v3].texCoord, vertices[v1].texCoord);

    tangents[v3] = compute(vertices[v3].position, vertices[v1].position,
                           vertices[v2].position, vertices[v3].texCoord,
                           vertices[v1].texCoord, vertices[v2].texCoord);
  }

  // Normalize
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertices[i].tangent = glm::normalize(tangents[i]);
  }
}

void Mesh::Draw(const ShaderProgram* program) const {
  m_VertexLayout->Bind();
  if (m_Material) {
    m_Material->SetToProgram(program);
  }

  glDrawElements(m_PrimitiveType, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT,
                 0);
}

MeshUPtr Mesh::CreateBox() {
  std::vector<Vertex> vertices = {
      Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),
             glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),
             glm::vec2(0.0f, 1.0f)},

      Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(0.0f, 1.0f)},

      Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),
             glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),
             glm::vec2(0.0f, 0.0f)},

      Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f),
             glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f),
             glm::vec2(0.0f, 0.0f)},

      Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f),
             glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f),
             glm::vec2(0.0f, 0.0f)},

      Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(0.0f, 0.0f)},
  };

  std::vector<uint32_t> indices = {
      0,  2,  1,  2,  0,  3,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
      12, 14, 13, 14, 12, 15, 16, 17, 18, 18, 19, 16, 20, 22, 21, 22, 20, 23,
  };

  return New(std::move(vertices), std::move(indices), GL_TRIANGLES);
}

MeshUPtr Mesh::CreateSphere(uint32_t latiSegmentCount,
                            uint32_t longiSegmentCount) {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  uint32_t circleVertCount = longiSegmentCount + 1;
  vertices.resize((latiSegmentCount + 1) * circleVertCount);
  for (uint32_t i = 0; i <= latiSegmentCount; ++i) {
    float v = (float)i / (float)latiSegmentCount;
    float phi = (v - 0.5f) * glm::pi<float>();
    auto cosPhi = cosf(phi);
    auto sinPhi = sinf(phi);
    for (uint32_t j = 0; j <= longiSegmentCount; j++) {
      float u = (float)j / (float)longiSegmentCount;
      float theta = u * glm::pi<float>() * 2.0f;
      auto cosTheta = cosf(theta);
      auto sinTheta = sinf(theta);
      auto point = glm::vec3(cosPhi * cosTheta, sinPhi, -cosPhi * sinTheta);

      vertices[i * circleVertCount + j] =
          Vertex{point * 0.5f, point, glm::vec2(u, v), glm::vec3(0.0f)};
    }
  }

  indices.resize(latiSegmentCount * longiSegmentCount * 6);
  for (uint32_t i = 0; i < latiSegmentCount; ++i) {
    for (uint32_t j = 0; j < longiSegmentCount; j++) {
      uint32_t vertexOffset = i * circleVertCount + j;
      uint32_t indexOffset = (i * longiSegmentCount + j) * 6;
      indices[indexOffset] = vertexOffset;
      indices[indexOffset + 1] = vertexOffset + 1;
      indices[indexOffset + 2] = vertexOffset + 1 + circleVertCount;
      indices[indexOffset + 3] = vertexOffset;
      indices[indexOffset + 4] = vertexOffset + 1 + circleVertCount;
      indices[indexOffset + 5] = vertexOffset + circleVertCount;
    }
  }

  return New(std::move(vertices), std::move(indices), GL_TRIANGLES);
}
