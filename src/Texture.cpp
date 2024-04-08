#include "Texture.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

Texture::Texture() {}

Texture::~Texture() {
  if (id_) {
    glDeleteTextures(1, &id_);
  }
}

std::unique_ptr<Texture> Texture::Create(const Image *image) {
  auto texture = std::unique_ptr<Texture>(new Texture());
  texture->CreateTexture();
  texture->SetTextureFromImage(image);

  return std::move(texture);
}

std::unique_ptr<Texture> Texture::CreateFromFile(const std::string &filename) {
  auto image = Image::Load(filename);
  if (!image) {
    return nullptr;
  }
  SPDLOG_INFO("image: {}x{}, {} channels", image->width(), image->height(),
              image->channel_count());
  auto texture = std::unique_ptr<Texture>(new Texture());
  texture->CreateTexture();
  texture->SetTextureFromImage(image.get());

  return std::move(texture);
}

std::unique_ptr<Texture> Texture::CreateEmpty(int width, int height,
                                              uint32_t format) {
  auto texture = std::unique_ptr<Texture>(new Texture());
  texture->CreateTexture();
  texture->SetTextureFormat(width, height, format);
  texture->SetFilter(GL_LINEAR, GL_LINEAR);

  return std::move(texture);
}

void Texture::SetFilter(uint32_t min_filter, uint32_t mag_filter) const {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
}

void Texture::SetWrap(uint32_t s_wrap, uint32_t t_wrap) const {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s_wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t_wrap);
}

void Texture::CreateTexture() {
  glGenTextures(1, &id_);
  glBindTexture(GL_TEXTURE_2D, id_);
  SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
  SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image *image) {
  GLenum format = GL_RGBA;

  switch (image->channel_count()) {
    case 1:
      format = GL_RED;
      break;
    case 2:
      format = GL_RG;
      break;
    case 3:
      format = GL_RGB;
      break;
    default:
      break;
  }

  width_ = image->width();
  height_ = image->height();
  format_ = format;

  glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, format,
               GL_UNSIGNED_BYTE, image->data());
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::SetTextureFormat(int width, int height, uint32_t format) {
  width_ = width;
  height_ = height;
  format_ = format;

  glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, format_,
               GL_UNSIGNED_BYTE, nullptr);
}

/*
 * CubeTexture
 */

CubeTexture::CubeTexture() {}

CubeTexture::~CubeTexture() {
  if (id_) {
    glDeleteTextures(1, &id_);
  }
}

std::unique_ptr<CubeTexture> CubeTexture::Create(
    const std::vector<Image *> &images) {
  auto texture = std::unique_ptr<CubeTexture>(new CubeTexture());
  if (!texture->InitFromImages(images)) {
    return nullptr;
  }

  return std::move(texture);
}

bool CubeTexture::InitFromImages(const std::vector<Image *> &images) {
  glGenTextures(1, &id_);
  Bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  for (uint32_t i = 0; i < (uint32_t)images.size(); i++) {
    auto image = images[i];
    GLenum format = GL_RGBA;
    switch (image->channel_count()) {
      case 1:
        format = GL_RED;
        break;
      case 2:
        format = GL_RG;
        break;
      case 3:
        format = GL_RGB;
        break;
      default:
        break;
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image->width(),
                 image->height(), 0, format, GL_UNSIGNED_BYTE, image->data());
  }

  return true;
}

bool Texture::SaveAsPng(const std::string &filename) const {
  int channelCount = 0;

  switch (format_) {
    case GL_R:
      channelCount = 1;
      break;
    case GL_RG:
      channelCount = 2;
      break;
    case GL_RGB:
      channelCount = 3;
      break;
    case GL_RGBA:
      channelCount = 4;
      break;
    default:
      SPDLOG_ERROR("channel count error");
      return false;
  }

  unsigned char *data = new unsigned char[width_ * height_ * channelCount];
  if (!data) {
    SPDLOG_ERROR("malloc error");
    return false;
  }
  memset(data, 0, width_ * height_ * channelCount);

  glBindTexture(GL_TEXTURE_2D, id_);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  bool result = true;
  stbi_flip_vertically_on_write(true);
  if (!stbi_write_png(filename.c_str(), width_, height_, channelCount, data,
                      width_ * channelCount)) {
    SPDLOG_ERROR("failed to save texture to PNG file");
    result = false;
  }
  delete[] data;

  return result;
}
