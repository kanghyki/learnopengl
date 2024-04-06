#ifndef INCLUDED_IMAGE_HPP
#define INCLUDED_IMAGE_HPP

#include "common.hpp"

class Image {
 public:
  static std::unique_ptr<Image> load(const std::string &filepath,
                                     bool flipVertical = true);
  static std::unique_ptr<Image> create(int width, int height,
                                       int channelCount = 4);
  static std::unique_ptr<Image> createSingleColorImage(int width, int height,
                                                       const glm::vec4 &color);
  ~Image();

  const uint8_t *getData() const;
  int getWidth() const;
  int getHeight() const;
  int getChannelCount() const;

 private:
  Image();
  bool loadFile(const std::string &filepath, bool flipVertical);
  bool allocate(int width, int height, int channelCount);

  int mWidth{0};
  int mHeight{0};
  int mChannelCount{0};
  uint8_t *mData{nullptr};
};

#endif
