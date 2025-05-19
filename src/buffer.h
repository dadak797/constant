#pragma once

#include "macro/ptr_macro.h"

// Standard library
#include <memory>
#include <cstdint>


DECLARE_PTR(Buffer)
class Buffer {
public:
    static BufferUPtr New(uint32_t bufferType, uint32_t usage, 
        const void* data, size_t stride, size_t count);

    // Destructor
    ~Buffer();

    uint32_t Get() const { return m_Buffer; }
    size_t GetStride() const { return m_Stride; }
    size_t GetCount() const { return m_Count; }
    void Bind() const;

private:
    // Constructor
    Buffer() {}

    bool init(uint32_t bufferType, uint32_t usage, const void* data, size_t stride, size_t count);

    uint32_t m_Buffer { 0 };
    uint32_t m_BufferType { 0 };  // Vertex buffer or index buffer
    uint32_t m_Usage { 0 };
    size_t m_Stride { 0 };
    size_t m_Count { 0 };
};