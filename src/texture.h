#pragma once

#include "macro/ptr_macro.h"
#include "config/gl_config.h"

// Standard library
#include <memory>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Image;


DECLARE_PTR(Texture)
class Texture {
public:
    static TextureUPtr New(const Image* image);
    
    // Create empty texture
    static TextureUPtr New(int32_t width, int32_t height, 
        uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);

    ~Texture();

    // Getter
    const uint32_t Get() const { return m_Texture; }
    int32_t GetWidth() const { return m_Width; }
    int32_t GetHeight() const { return m_Height; }
    uint32_t GetFormat() const { return m_Format; }
    uint32_t GetType() const { return m_Type; }

    void Bind() const;
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;
#ifndef __EMSCRIPTEN__
    // Does not work on OpenGL ES
    void SetBorderColor(const glm::vec4& color) const;
#endif
    
private:
    Texture();

    void createTexture();
    void setTextureFromImage(const Image* image);
    void setTextureFormat(int32_t width, int32_t height, uint32_t format, uint32_t type);

    uint32_t m_Texture { 0 };
    int32_t m_Width { 0 };
    int32_t m_Height { 0 };
    uint32_t m_Format { GL_RGBA };
    uint32_t m_Type { GL_UNSIGNED_BYTE };
};