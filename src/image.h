#pragma once

#include "macro/ptr_macro.h"

// Standard library
#include <string>

// glm
#include <glm/glm.hpp>

DECLARE_PTR(Image)
class Image {
 public:
  // Create image from file
  static ImageUPtr New(const std::string& filepath, bool flipVertical = true);

  // Create empty image
  static ImageUPtr New(int32_t width, int32_t height, int32_t channelCount = 4,
                       int32_t bytePerChannel = 1);

  // Create image with single color
  static ImageUPtr New(int32_t width, int32_t height, const glm::vec4& color);

  ~Image();

  const uint8_t* GetData() const { return m_Data; }
  int32_t GetWidth() const { return m_Width; }
  int32_t GetHeight() const { return m_Height; }
  int32_t GetChannelCount() const { return m_ChannelCount; }
  int32_t GetBytePerChannel() const { return m_BytePerChannel; }

 private:
  Image() = default;

  bool loadWithStb(const std::string& filepath, bool flipVertical);
  bool allocate(int32_t width, int32_t height, int32_t channelCount,
                int32_t bytePerChannel);

  // Exponential form of 2 is the most efficient for texture size.
  int32_t m_Width{0};
  int32_t m_Height{0};
  int32_t m_ChannelCount{0};
  int32_t m_BytePerChannel{1};
  uint8_t* m_Data{nullptr};
};