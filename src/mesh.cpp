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
  mesh->m_Vertices = std::move(vertices);
  mesh->m_Indices = std::move(indices);
  mesh->m_PrimitiveType = primitiveType;
  mesh->init();
  return std::move(mesh);
}

MeshUPtr Mesh::New(MeshType meshType) {
  auto mesh = MeshUPtr(new Mesh());
  switch (meshType) {
    case MeshType::BOX:
      mesh->createBox();
      break;
    case MeshType::SPHERE:
      mesh->createSphere();
      break;
    case MeshType::CYLINDER:
      mesh->createCylinder();
      break;
    case MeshType::CONE:
      mesh->createCone();
      break;
    case MeshType::PLANE:
      mesh->createPlane();
      break;
    case MeshType::TORUS:
      mesh->createTorus();
      break;
    case MeshType::DISK:
      mesh->createDisk();
      break;
    default:
      SPDLOG_ERROR("Unknown mesh type");
      break;
  }
  return std::move(mesh);
}

Mesh::~Mesh() {}

void Mesh::init() {
  if (m_PrimitiveType == GL_TRIANGLES) {
    ComputeTangents(m_Vertices, m_Indices);
  }

  m_VertexLayout = VertexLayout::New();
  m_VertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
  m_VertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex),
                            offsetof(Vertex, normal));
  m_VertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex),
                            offsetof(Vertex, texCoord));
  m_VertexLayout->SetAttrib(3, 3, GL_FLOAT, false, sizeof(Vertex),
                            offsetof(Vertex, tangent));

  m_VertexBuffer =
      Buffer::New(GL_ARRAY_BUFFER, GL_STATIC_DRAW, m_Vertices.data(),
                  sizeof(Vertex), m_Vertices.size());
  m_IndexBuffer =
      Buffer::New(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, m_Indices.data(),
                  sizeof(uint32_t), m_Indices.size());
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

void Mesh::createBox() {
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

  m_Vertices = std::move(vertices);
  m_Indices = std::move(indices);
  m_PrimitiveType = GL_TRIANGLES;
  init();
}

void Mesh::createSphere() {}

void Mesh::createCylinder() {}

void Mesh::createCone() {}

void Mesh::createPlane() {}

void Mesh::createTorus() {}

void Mesh::createDisk() {}
