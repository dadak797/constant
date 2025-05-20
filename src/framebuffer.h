#pragma once

#include "macro/ptr_macro.h"
#include "texture.h"

// Standard library
#include <memory>
#include <vector>


DECLARE_PTR(Framebuffer);
class Framebuffer {
public:
    static FramebufferUPtr New(const std::vector<TexturePtr>& colorAttachments);
    static void BindToDefault();

    ~Framebuffer();

    const uint32_t Get() const { return m_Framebuffer; }
    void Bind() const;
    int32_t GetColorAttachmentCount() const { return (int32_t)m_ColorAttachments.size(); }
    const TexturePtr GetColorAttachment(int32_t index = 0) const { return m_ColorAttachments.at(index); }

private:
    Framebuffer();

    bool initWithColorAttachments(const std::vector<TexturePtr>& colorAttachments);

    uint32_t m_Framebuffer { 0 };
    uint32_t m_DepthStencilBuffer { 0 };
    std::vector<TexturePtr> m_ColorAttachments;
};