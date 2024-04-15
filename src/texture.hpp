#ifndef INCLUDED_TEXTURE_HPP
#define INCLUDED_TEXTURE_HPP

#include "image.hpp"

class Texture {
 public:
  static std::unique_ptr<Texture> Create(const Image *image);
  static std::unique_ptr<Texture> Create(const std::string &filename);
  static std::unique_ptr<Texture> Create(int width, int height, uint32_t format,
                                         uint32_t type = GL_UNSIGNED_BYTE);
  ~Texture();

  inline void Bind() const { glBindTexture(GL_TEXTURE_2D, id_); }
  bool SaveAsPng(const std::string &filename) const;

  void SetFilter(uint32_t min_filter, uint32_t mag_filter) const;
  void SetWrap(uint32_t s_wrap, uint32_t t_wrap) const;
  void SetTextureFormat(int width, int height, uint32_t format, uint32_t type);
  void SetBorderColor(const glm::vec4 &color) const;
  unsigned char *GetTexImage() const;
  std::array<uint8_t, 4> GetTexPixel(int x, int y) const;
  uint32_t GetChannelCount() const;

  inline const uint32_t id() const { return id_; }
  inline int width() const { return width_; }
  inline int height() const { return height_; }
  inline uint32_t format() const { return format_; }
  inline uint32_t type() const { return type_; }

 private:
  Texture();

  void CreateTexture();
  void SetTextureFromImage(const Image *image);

  uint32_t id_{0};
  int width_{0};
  int height_{0};
  uint32_t format_{GL_RGBA};
  uint32_t type_{GL_UNSIGNED_BYTE};
};

class CubeTexture {
 public:
  static std::unique_ptr<CubeTexture> Create(
      const std::vector<Image *> &images);
  ~CubeTexture();

  inline void Bind() const { glBindTexture(GL_TEXTURE_CUBE_MAP, id_); }

  inline const uint32_t id() const { return id_; }

 private:
  CubeTexture();

  bool InitFromImages(const std::vector<Image *> &images);

  uint32_t id_{0};
};

#endif
