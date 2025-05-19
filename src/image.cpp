#include "image.h"
#include "config/log_config.h"
#include "util/path_util.h"

// stb image
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


ImageUPtr Image::Load(const std::string& filepath, bool flipVertical) {
    auto image = ImageUPtr(new Image());
    if (!image->loadWithStb(filepath, flipVertical)) {
        return nullptr;
    }
    return std::move(image);
}

ImageUPtr Image::New(int32_t width, int32_t height, int32_t channelCount, int32_t bytePerChannel) {
    auto image = ImageUPtr(new Image());
    if (!image->allocate(width, height, channelCount, bytePerChannel)) {
        return nullptr;
    }
    return std::move(image);
}

ImageUPtr Image::New(int32_t width, int32_t height, const glm::vec4& color) {
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
    uint8_t rgba[4] = {
        (uint8_t)clamped.r, 
        (uint8_t)clamped.g, 
        (uint8_t)clamped.b, 
        (uint8_t)clamped.a, 
    };
    auto image = New(width, height, 4);
    for (int i = 0; i < width * height; i++) {
        memcpy(image->m_Data + 4 * i, rgba, 4);
    }
    return std::move(image);
}

Image::~Image() {
    if (m_Data) {
        stbi_image_free(m_Data);
    }
}

bool Image::loadWithStb(const std::string& filepath, bool flipVertical) {
    stbi_set_flip_vertically_on_load(flipVertical);  // Make image upside down    
    
    std::string ext = PathUtil::GetExtension(filepath);
    if (ext.empty()) {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }

    if (ext == ".hdr" || ext == ".HDR") {
        m_Data = (uint8_t*)stbi_loadf(filepath.c_str(),
            &m_Width, &m_Height, &m_ChannelCount, 0);
        m_BytePerChannel = 4;
    }
    else {
        m_Data = stbi_load(filepath.c_str(),
            &m_Width, &m_Height, &m_ChannelCount, 0);
        m_BytePerChannel = 1;
    }

    if (!m_Data) {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }

    return true;
}

bool Image::allocate(int32_t width, int32_t height, 
    int32_t channelCount, int32_t bytePerChannel) {
    m_Width = width;
    m_Height = height;
    m_ChannelCount = channelCount;
    m_BytePerChannel = bytePerChannel;
    m_Data = (uint8_t*)malloc(m_Width * m_Height * m_ChannelCount);
    return m_Data ? true : false;
}