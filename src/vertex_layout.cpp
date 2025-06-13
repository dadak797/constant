#include "vertex_layout.h"

#include "config/gl_config.h"

VertexLayoutUPtr VertexLayout::New() {
  auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
  vertexLayout->init();
  return std::move(vertexLayout);
}

VertexLayout::~VertexLayout() {
  if (m_VertexArrayObject) {
    glDeleteVertexArrays(1, &m_VertexArrayObject);
  }
}

void VertexLayout::Bind() const { glBindVertexArray(m_VertexArrayObject); }

void VertexLayout::SetAttrib(uint32_t attribIndex, int32_t count, uint32_t type,
                             bool normalized, size_t stride,
                             uint64_t offset) const {
  glEnableVertexAttribArray(attribIndex);  // attribIndex will be used.

  // Define description of attribIndex.
  // If attribIndex is 0, it is used in shader as location 0.
  glVertexAttribPointer(attribIndex, count, type, normalized, stride,
                        (const void*)offset);
}

void VertexLayout::DisableAttrib(uint32_t attribIndex) const {
  glDisableVertexAttribArray(attribIndex);
}

void VertexLayout::init() {
  glGenVertexArrays(1, &m_VertexArrayObject);
  Bind();
}