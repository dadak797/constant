#include "texture.h"
#include "image.h"
#include "config/log_config.h"


TextureUPtr Texture::New(const Image* image) {
    auto texture = TextureUPtr(new Texture());
    texture->createTexture();
    texture->setTextureFromImage(image);
    return std::move(texture);
}

TextureUPtr Texture::New(int32_t width, int32_t height, uint32_t format, uint32_t type) {
    auto texture = TextureUPtr(new Texture());
    texture->createTexture();
    texture->setTextureFormat(width, height, format, type);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);  // Set filter for empty texture
    return std::move(texture);
}

Texture::Texture() {
}

Texture::~Texture() {
    if (m_Texture) {
        glDeleteTextures(1, &m_Texture);
    }
}

void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, m_Texture);
}

void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);  // X-direction of texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);  // Y-direction of texture
}
    
void Texture::createTexture() {
    glGenTextures(1, &m_Texture);
    Bind();    
    SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);  // Set default filter
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::setTextureFromImage(const Image* image) {
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount()) {
        default: break;
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
    }

    m_Width = image->GetWidth();
    m_Height = image->GetHeight();
#ifdef __EMSCRIPTEN__
    if (format == GL_RED) {
        m_Format = GL_R8;  // In OpenGL-ES, internal format should be GL_R8 instead of GL_RED.
    }
    else {
        m_Format = format;  // Internal format and image format should be identical for WebGL.
    }
#else
    m_Format = GL_RGBA;
#endif
    m_Type = GL_UNSIGNED_BYTE;

    if (image->GetBytePerChannel() == 4) {
        m_Type = GL_FLOAT;
        switch (image->GetChannelCount()) {
            default: break;
            case 1: m_Format = GL_R16F; break;
            case 2: m_Format = GL_RG16F; break;
            case 3: m_Format = GL_RGB16F; break;
            case 4: m_Format = GL_RGBA16F; break;
        }
    }

    // Copy image data to GPU
    // GL_RGBA: If image->GetChannelCount() is 3, alpha channel is automatically filled with 255.
    glTexImage2D(GL_TEXTURE_2D,
        0, m_Format, m_Width, m_Height,  // Describe data on GPU. 0: Mipmap level
        0, format, m_Type, image->GetData());  // Describe data on CPU. 0: Border size

    // Generate mipmap
    // - Use small texture image for small window size
    // - Require small size texture images. Its size becomes half for the next mipmap level. 
    // - Mipmap level 0's width and height(1024x1024) are twice of 1's width and height.
#ifdef __EMSCRIPTEN__
    // WebGL doesn't support GL_FLOAT for mipmap.
    if (m_Type == GL_FLOAT) {
        SetFilter(GL_LINEAR, GL_LINEAR);
    }
    else {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
#else
    glGenerateMipmap(GL_TEXTURE_2D);
#endif
}

static GLenum GetImageFormat(uint32_t internalFormat) {
    GLenum imageFormat = GL_RGBA;

    // For depth buffer
    // GL_DEPTH_COMPONENT24 is for OpenGL-ES.
    // GL_DEPTH_COMPONENT is for OpenGL.
    if (internalFormat == GL_DEPTH_COMPONENT24 || internalFormat == GL_DEPTH_COMPONENT) {
        imageFormat = GL_DEPTH_COMPONENT;        
    }
    else if (internalFormat == GL_RGB || internalFormat == GL_RGB16F || internalFormat == GL_RGB32F) {
        imageFormat = GL_RGB;
    }
    else if (internalFormat == GL_RG || internalFormat == GL_RG16F || internalFormat == GL_RG32F) {
        imageFormat = GL_RG;
    }
    else if (internalFormat == GL_RED || internalFormat == GL_R8 || internalFormat == GL_R16F || internalFormat == GL_R32F) {
        imageFormat = GL_RED;
    }

    return imageFormat;
}

void Texture::setTextureFormat(int32_t width, int32_t height, uint32_t format, uint32_t type) {
    m_Width = width;
    m_Height = height;
    m_Format = format;
    m_Type = type;

    GLenum imageFormat = GetImageFormat(m_Format);

    // m_Format: Internal format
    // imageFormat: Image format
    glTexImage2D(GL_TEXTURE_2D,
        0, m_Format, m_Width, m_Height,
        0, imageFormat, m_Type, nullptr);  // nullptr: Memory allocation only
}

#ifndef __EMSCRIPTEN__
void Texture::SetBorderColor(const glm::vec4& color) const {
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
}
#endif