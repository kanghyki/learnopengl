#include "Texture.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

Texture::Texture() {}

Texture::~Texture() {
  if (mId) {
    glDeleteTextures(1, &mId);
  }
}

std::unique_ptr<Texture> Texture::create(const Image *image) {
  auto texture = std::unique_ptr<Texture>(new Texture());
  texture->createTexture();
  texture->setTextureFromImage(image);

  return std::move(texture);
}

std::unique_ptr<Texture> Texture::create(const std::string &filename) {
  auto image = Image::load(filename);
  if (!image) {
    return nullptr;
  }
  SPDLOG_INFO("image: {}x{}, {} channels", image->getWidth(),
              image->getHeight(), image->getChannelCount());
  auto texture = std::unique_ptr<Texture>(new Texture());
  texture->createTexture();
  texture->setTextureFromImage(image.get());

  return std::move(texture);
}

std::unique_ptr<Texture> Texture::create(int width, int height,
                                         uint32_t format) {
  auto texture = std::unique_ptr<Texture>(new Texture());
  texture->createTexture();
  texture->setTextureFormat(width, height, format);
  texture->setFilter(GL_LINEAR, GL_LINEAR);

  return std::move(texture);
}

void Texture::bind() { glBindTexture(GL_TEXTURE_2D, mId); }

void Texture::setFilter(uint32_t minFilter, uint32_t magFilter) const {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::setWrap(uint32_t sWrap, uint32_t tWrap) const {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void Texture::createTexture() {
  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_2D, mId);
  setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
  setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::setTextureFromImage(const Image *image) {
  GLenum format = GL_RGBA;

  switch (image->getChannelCount()) {
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

  mWidth = image->getWidth();
  mHeight = image->getHeight();
  mFormat = format;

  glTexImage2D(GL_TEXTURE_2D, 0, mFormat, mWidth, mHeight, 0, format,
               GL_UNSIGNED_BYTE, image->getData());
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::setTextureFormat(int width, int height, uint32_t format) {
  mWidth = width;
  mHeight = height;
  mFormat = format;

  glTexImage2D(GL_TEXTURE_2D, 0, mFormat, mWidth, mHeight, 0, mFormat,
               GL_UNSIGNED_BYTE, nullptr);
}

const uint32_t Texture::getId() const { return mId; }

int Texture::getWidth() const { return mWidth; }

int Texture::getHeight() const { return mHeight; }

uint32_t Texture::getFormat() const { return mFormat; }

/*
 * CubeTexture
 */

CubeTexture::CubeTexture() {}

CubeTexture::~CubeTexture() {
  if (mId) {
    glDeleteTextures(1, &mId);
  }
}

std::unique_ptr<CubeTexture> CubeTexture::create(
    const std::vector<Image *> &images) {
  auto texture = std::unique_ptr<CubeTexture>(new CubeTexture());
  if (!texture->initFromImages(images)) {
    return nullptr;
  }

  return std::move(texture);
}

void CubeTexture::bind() const { glBindTexture(GL_TEXTURE_CUBE_MAP, mId); }

bool CubeTexture::initFromImages(const std::vector<Image *> &images) {
  glGenTextures(1, &mId);
  bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  for (uint32_t i = 0; i < (uint32_t)images.size(); i++) {
    auto image = images[i];
    GLenum format = GL_RGBA;
    switch (image->getChannelCount()) {
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

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                 image->getWidth(), image->getHeight(), 0, format,
                 GL_UNSIGNED_BYTE, image->getData());
  }

  return true;
}

const uint32_t CubeTexture::getId() const { return mId; }

bool Texture::saveAsPng(const std::string &filename) const {
  int channelCount = 0;

  switch (mFormat) {
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

  unsigned char *data = new unsigned char[mWidth * mHeight * channelCount];
  if (!data) {
    SPDLOG_ERROR("malloc error");
    return false;
  }
  memset(data, 0, mWidth * mHeight * channelCount);

  glBindTexture(GL_TEXTURE_2D, mId);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  bool result = true;
  stbi_flip_vertically_on_write(true);
  if (!stbi_write_png(filename.c_str(), mWidth, mHeight, channelCount, data,
                      mWidth * channelCount)) {
    SPDLOG_ERROR("failed to save texture to PNG file");
    result = false;
  }
  delete[] data;

  return result;
}
