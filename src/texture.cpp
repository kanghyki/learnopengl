#include "texture.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

Texture2d::Texture2d() : BaseTexture(GL_TEXTURE_2D) {}

Texture2d::~Texture2d() {}

std::unique_ptr<Texture2d> Texture2d::Create(const Image *image) {
  auto texture = std::unique_ptr<Texture2d>(new Texture2d());
  texture->Bind();
  texture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
  texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  texture->SetTextureFromImage(image);

  return std::move(texture);
}

std::unique_ptr<Texture2d> Texture2d::Create(const std::string &filename) {
  auto image = Image::Load(filename);
  if (!image) {
    return nullptr;
  }
  SPDLOG_INFO("image: {}x{}, {} channels", image->width(), image->height(),
              image->channel_count());
  auto texture = std::unique_ptr<Texture2d>(new Texture2d());
  texture->Bind();
  texture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
  texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  texture->SetTextureFromImage(image.get());

  return std::move(texture);
}

std::unique_ptr<Texture2d> Texture2d::Create(int width, int height,
                                             uint32_t inner_format,
                                             uint32_t format, uint32_t type) {
  auto texture = std::unique_ptr<Texture2d>(new Texture2d());
  texture->Bind();
  texture->SetFilter(GL_LINEAR, GL_LINEAR);
  texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  texture->SetTextureFormat(width, height, inner_format, format, type);

  return std::move(texture);
}

void Texture2d::SetTextureFromImage(const Image *image) {
  width_ = image->width();
  height_ = image->height();
  format_ = ChannelCountToRGBAFormat(image->channel_count());

  glTexImage2D(GL_TEXTURE_2D, 0, inner_format_, width_, height_, 0, format_,
               type_, image->data());
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2d::SetTextureFormat(int width, int height, uint32_t inner_format,
                                 uint32_t format, uint32_t type) {
  width_ = width;
  height_ = height;
  inner_format_ = inner_format;
  format_ = format;
  type_ = type;

  glTexImage2D(GL_TEXTURE_2D, 0, inner_format_, width_, height_, 0, format_,
               type_, nullptr);
}

void Texture2d::SetBorderColor(const glm::vec4 &color) const {
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
                   glm::value_ptr(color));
}

bool Texture2d::SaveAsPng(const std::string &filename) const {
  int channel_count = RGBAFormatToChannelCount(format_);
  unsigned char *data = GetTexImage();
  if (!data) {
    SPDLOG_ERROR("malloc error");
    return false;
  }

  bool result = true;
  stbi_flip_vertically_on_write(true);
  if (!stbi_write_png(filename.c_str(), width_, height_, channel_count, data,
                      width_ * channel_count)) {
    SPDLOG_ERROR("failed to save texture to PNG file");
    result = false;
  }
  delete[] data;

  return result;
}

unsigned char *Texture2d::GetTexImage() const {
  int channel_count = RGBAFormatToChannelCount(format_);
  unsigned char *data = new unsigned char[width_ * height_ * channel_count];
  if (!data) {
    return nullptr;
  }
  Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  return data;
}

std::array<uint8_t, 4> Texture2d::GetTexPixel(int x, int y) const {
  uint8_t pixel[4];

  Bind();
  glReadPixels(x, y, 1, 1, format_, GL_UNSIGNED_BYTE, pixel);

  return {pixel[0], pixel[1], pixel[2], pixel[3]};
}

/*
 * CubeTexture
 */

Texture3d::Texture3d() : BaseTexture(GL_TEXTURE_CUBE_MAP) {}

Texture3d::~Texture3d() {}

std::unique_ptr<Texture3d> Texture3d::Create(
    const std::vector<Image *> &images) {
  auto texture = std::unique_ptr<Texture3d>(new Texture3d());
  texture->Bind();
  texture->SetFilter(GL_LINEAR, GL_LINEAR);
  texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  texture->SetCubeMapFromImages(images);

  return std::move(texture);
}

std::unique_ptr<Texture3d> Texture3d::Create(int width, int height, int length,
                                             uint32_t format, uint32_t type) {
  auto texture = std::unique_ptr<Texture3d>(new Texture3d());
  texture->Bind();
  texture->SetFilter(GL_LINEAR, GL_LINEAR);
  texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  texture->SetCubeMapFormat(width, height, length, format, type);

  return std::move(texture);
}

void Texture3d::SetCubeMapFromImages(const std::vector<Image *> &images) {
  // width_ = -1;
  // height_ = -1;
  // length_ = -1;
  type_ = GL_UNSIGNED_BYTE;
  // format_ = GL_RGBA;

  for (uint32_t i = 0; i < (uint32_t)images.size(); i++) {
    Image *image = images[i];
    GLenum format = ChannelCountToRGBAFormat(image->channel_count());

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image->width(),
                 image->height(), 0, format, type_, image->data());
  }
}

void Texture3d::SetCubeMapFormat(int width, int height, int length,
                                 uint32_t format, uint32_t type) {
  width_ = width;
  height_ = height;
  length_ = length;
  type_ = type;
  format_ = format;

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format_, height_, length_, 0,
               format_, type_, NULL);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format_, height_, length_, 0,
               format_, type_, NULL);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format_, width_, height_, 0,
               format_, type_, NULL);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format_, width_, height_, 0,
               format_, type_, NULL);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format_, length_, width_, 0,
               format_, type_, NULL);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format_, length_, width_, 0,
               format_, type_, NULL);
}