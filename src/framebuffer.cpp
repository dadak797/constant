#include "framebuffer.h"
#include "config/log_config.h"


FramebufferUPtr Framebuffer::New(const std::vector<TexturePtr>& colorAttachments) {
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->initWithColorAttachments(colorAttachments)) {
        return nullptr;
    }
    return std::move(framebuffer);
}

Framebuffer::Framebuffer() {
}

Framebuffer::~Framebuffer() {
    if (m_DepthStencilBuffer) {
        glDeleteRenderbuffers(1, &m_DepthStencilBuffer);
    }
    if (m_Framebuffer) {
        glDeleteFramebuffers(1, &m_Framebuffer);
    }
}

void Framebuffer::BindToDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Bind to default framebuffer
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
}

bool Framebuffer::initWithColorAttachments(const std::vector<TexturePtr>& colorAttachments) {
    m_ColorAttachments = colorAttachments;
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    for (size_t i = 0; i < m_ColorAttachments.size(); ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
            m_ColorAttachments[i]->Get(), 0);
    }

    // If size is 1, GL_COLOR_ATTACHMENT0 is used automatically. So no need to set it.
    if (m_ColorAttachments.size() > 1) {
        std::vector<GLenum> attachments;
        attachments.resize(m_ColorAttachments.size());
        for (size_t i = 0; i < m_ColorAttachments.size(); ++i) {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(m_ColorAttachments.size(), attachments.data());
    }

    int32_t width = m_ColorAttachments[0]->GetWidth();
    int32_t height = m_ColorAttachments[0]->GetHeight();

    glGenRenderbuffers(1, &m_DepthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);  // Return to default renderbuffer

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_DepthStencilBuffer);

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();

    return true;
}