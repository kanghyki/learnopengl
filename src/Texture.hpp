#ifndef INCLUDED_TEXTURE_HPP
#define INCLUDED_TEXTURE_HPP

#include "Image.hpp"

class Texture {
public:
  ~Texture();
  static std::unique_ptr<Texture> create(const Image *image);
  static std::unique_ptr<Texture> create(const std::string &filename);
  static std::unique_ptr<Texture> create(int width, int height,
                                         uint32_t format);

  void bind();

  void setFilter(uint32_t minFilter, uint32_t magFilter) const;
  void setWrap(uint32_t sWrap, uint32_t tWrap) const;
  void setTextureFormat(int width, int height, uint32_t format);

  const uint32_t getId() const;
  int getWidth() const;
  int getHeight() const;
  uint32_t getFormat() const;

private:
  Texture();

  void createTexture();
  void setTextureFromImage(const Image *image);

  uint32_t mId{0};
  int m_width{0};
  int m_height{0};
  uint32_t m_format{GL_RGBA};
};

class CubeTexture {
public:
  ~CubeTexture();

  static std::unique_ptr<CubeTexture>
  create(const std::vector<Image *> &images);

  const uint32_t getId() const;
  void bind() const;

private:
  CubeTexture();

  bool initFromImages(const std::vector<Image *> &images);

  uint32_t mId{0};
};

#endif