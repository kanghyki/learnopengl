#include "image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Image::Image() {}

Image::~Image() {
  if (data_) {
    stbi_image_free(data_);
  }
}

std::unique_ptr<Image> Image::Load(const std::string &filepath,
                                   bool flip_vertical) {
  auto image = std::unique_ptr<Image>(new Image());
  if (!image->LoadFile(filepath, flip_vertical)) {
    return nullptr;
  }

  return std::move(image);
}

std::unique_ptr<Image> Image::Create(int width, int height, int channel_count) {
  auto image = std::unique_ptr<Image>(new Image());
  if (!image->Allocate(width, height, channel_count)) {
    return nullptr;
  }

  return std::move(image);
}

bool Image::Allocate(int width, int height, int channelCount) {
  width_ = width;
  height_ = height;
  channel_count_ = channelCount;
  if (!(data_ = (uint8_t *)malloc(width_ * height_ * channel_count_))) {
    return false;
  }

  return true;
}

std::unique_ptr<Image> Image::CreateSingleColorImage(int width, int height,
                                                     const glm::vec4 &color) {
  glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
  uint8_t rgba[4] = {
      (uint8_t)clamped.r,
      (uint8_t)clamped.g,
      (uint8_t)clamped.b,
      (uint8_t)clamped.a,
  };
  auto image = Create(width, height, 4);
  for (int i = 0; i < width * height; ++i) {
    memcpy(image->data_ + 4 * i, rgba, 4);
  }

  return std::move(image);
}

bool Image::LoadFile(const std::string &filepath, bool flip_vertical) {
  stbi_set_flip_vertically_on_load(flip_vertical);
  data_ = stbi_load(filepath.c_str(), &width_, &height_, &channel_count_, 0);
  if (!data_) {
    SPDLOG_ERROR("failed to load image: {}", filepath);
    return false;
  }

  return true;
}