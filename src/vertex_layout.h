#pragma once

#include "macro/ptr_macro.h"


DECLARE_PTR(VertexLayout)
class VertexLayout {
public:
    static VertexLayoutUPtr New();

    // Destructor
    ~VertexLayout();

    uint32_t Get() const { return m_VertexArrayObject; }
    void Bind() const;
    void SetAttrib(uint32_t attribIndex, int32_t count, uint32_t type, bool normalized,
        size_t stride, uint64_t offset) const;
    void DisableAttrib(uint32_t attribIndex) const;

private:
    // Constructor
    VertexLayout() = default;

    void init();

    uint32_t m_VertexArrayObject { 0 };
};