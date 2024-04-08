#ifndef INCLUDED_FRAMEBUFFER_HPP
#define INCLUDED_FRAMEBUFFER_HPP

#include "common.hpp"
#include "texture.hpp"

class Framebuffer {
 public:
  static std::unique_ptr<Framebuffer> Create(
      const std::shared_ptr<Texture> color_attachment);
  inline static void BindToDefault() { glBindFramebuffer(GL_FRAMEBUFFER, 0); };
  ~Framebuffer();

  inline void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, id_); };

  inline const uint32_t id() const { return id_; };
  inline const std::shared_ptr<Texture> color_attachment() const {
    return color_attachment_;
  };

 private:
  Framebuffer();

  bool InitWithColorAttachment(const std::shared_ptr<Texture> color_attachment);

  uint32_t id_{0};
  uint32_t depth_stencil_buffer_{0};
  std::shared_ptr<Texture> color_attachment_{nullptr};
};

#endif