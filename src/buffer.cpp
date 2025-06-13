#include "buffer.h"

#include "config/gl_config.h"

BufferUPtr Buffer::New(uint32_t bufferType, uint32_t usage, const void* data,
                       size_t stride, size_t count) {
  auto buffer = BufferUPtr(new Buffer());
  if (!buffer->init(bufferType, usage, data, stride, count)) {
    return nullptr;
  }
  return std::move(buffer);
}

Buffer::~Buffer() {
  if (m_Buffer) {
    glDeleteBuffers(1, &m_Buffer);
  }
}

void Buffer::Bind() const { glBindBuffer(m_BufferType, m_Buffer); }

bool Buffer::init(uint32_t bufferType, uint32_t usage, const void* data,
                  size_t stride, size_t count) {
  m_BufferType = bufferType;
  m_Usage = usage;
  m_Stride = stride;
  m_Count = count;

  glGenBuffers(1, &m_Buffer);  // Create buffer
  Bind();                      // Bind this buffer
  glBufferData(m_BufferType, m_Stride * m_Count, data,
               usage);  // Upload data to buffer

  return true;
}