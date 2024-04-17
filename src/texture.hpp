#ifndef INCLUDED_TEXTURE_HPP
#define INCLUDED_TEXTURE_HPP

#include "image.hpp"
class BaseTexture {
 public:
  BaseTexture(uint32_t texture_type) : texture_type_(texture_type) {
    glGenTextures(1, &id_);
  }
  virtual ~BaseTexture() {
    if (id_) {
      glDeleteTextures(1, &id_);
    }
  }

  virtual inline void Bind() const final { glBindTexture(texture_type_, id_); };

  virtual void SetFilter(uint32_t min_filter, uint32_t mag_filter) const final {
    glTexParameteri(texture_type_, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(texture_type_, GL_TEXTURE_MAG_FILTER, mag_filter);
  }

  virtual void SetWrap(uint32_t s_wrap, uint32_t t_wrap,
                       uint32_t r_wrap = GL_NONE) const final {
    glTexParameteri(texture_type_, GL_TEXTURE_WRAP_S, s_wrap);
    glTexParameteri(texture_type_, GL_TEXTURE_WRAP_T, t_wrap);
    if (r_wrap != GL_NONE) {
      glTexParameteri(texture_type_, GL_TEXTURE_WRAP_T, r_wrap);
    }
  }

  inline const uint32_t id() const { return id_; }
  inline const uint32_t texture_type() const { return texture_type_; }

 protected:
  uint32_t id_{0};

  GLenum ChannelCountToRGBAFormat(uint32_t channel_count) const {
    GLenum format = GL_RGBA;

    switch (channel_count) {
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

    return format;
  }
  uint32_t RGBAFormatToChannelCount(GLenum format) const {
    int channel_count = 4;

    switch (format) {
      case GL_RED:
        channel_count = 1;
        break;
      case GL_RG:
        channel_count = 2;
        break;
      case GL_RGB:
        channel_count = 3;
        break;
      default:
        break;
    }

    return channel_count;
  }

 private:
  uint32_t texture_type_{GL_TEXTURE_2D};
};

class Texture2d : public BaseTexture {
 public:
  static std::unique_ptr<Texture2d> Create(const Image *image);
  static std::unique_ptr<Texture2d> Create(const std::string &filename);
  static std::unique_ptr<Texture2d> Create(int width, int height,
                                           uint32_t format,
                                           uint32_t type = GL_UNSIGNED_BYTE);
  ~Texture2d();

  bool SaveAsPng(const std::string &filename) const;

  void SetTextureFormat(int width, int height, uint32_t format, uint32_t type);
  void SetBorderColor(const glm::vec4 &color) const;
  unsigned char *GetTexImage() const;
  std::array<uint8_t, 4> GetTexPixel(int x, int y) const;

  inline int width() const { return width_; }
  inline int height() const { return height_; }
  inline uint32_t format() const { return format_; }
  inline uint32_t type() const { return type_; }

 private:
  Texture2d();

  void SetTextureFromImage(const Image *image);

  int width_{0};
  int height_{0};
  uint32_t format_{GL_RGBA};
  uint32_t type_{GL_UNSIGNED_BYTE};
};

class Texture3d : public BaseTexture {
 public:
  static std::unique_ptr<Texture3d> Create(const std::vector<Image *> &images);
  static std::unique_ptr<Texture3d> Create(int width, int height, int length,
                                           uint32_t format, uint32_t type);
  ~Texture3d();

  inline int width() const { return width_; }
  inline int height() const { return height_; }
  inline int length() const { return length_; }
  inline int type() const { return type_; }
  inline int format() const { return format_; }

 private:
  Texture3d();

  void SetCubeMapFromImages(const std::vector<Image *> &images);
  void SetCubeMapFormat(int width, int height, int length, uint32_t format,
                        uint32_t type);

  int width_{0};
  int height_{0};
  int length_{0};
  uint32_t type_{0};
  uint32_t format_{0};
};

#endif
