#ifndef INCLUDED_FRAMEBUFFER_HPP
#define INCLUDED_FRAMEBUFFER_HPP

#include "common.hpp"
#include "texture.hpp"

class BaseFramebuffer {
 public:
  inline static void BindToDefault() { glBindFramebuffer(GL_FRAMEBUFFER, 0); };

  BaseFramebuffer() {}
  ~BaseFramebuffer() {
    if (id_) {
      glDeleteFramebuffers(1, &id_);
    }
  }

  virtual inline void Bind() final { glBindFramebuffer(GL_FRAMEBUFFER, id_); }
  virtual inline uint32_t id() const final { return id_; }
  virtual bool Init() final {
    bool ret = true;

    glGenFramebuffers(1, &id_);
    Bind();

    InitTexture();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      SPDLOG_ERROR("failed to create framebuffer");
      ret = false;
    }
    BindToDefault();
    return ret;
  }

 protected:
  uint32_t id_{0};

  virtual void InitTexture() = 0;

 private:
  BaseFramebuffer(const BaseFramebuffer&);
  BaseFramebuffer& operator=(const BaseFramebuffer&);
};

class Framebuffer : public BaseFramebuffer {
 public:
  static std::unique_ptr<Framebuffer> Create(
      const std::shared_ptr<Texture> color_attachment) {
    auto framebuffer = std::unique_ptr<Framebuffer>(new Framebuffer());

    framebuffer->SetColorAttachment(color_attachment);
    if (!framebuffer->Init()) {
      return nullptr;
    }

    return std::move(framebuffer);
  }

  ~Framebuffer() {
    if (depth_stencil_buffer_) {
      glDeleteRenderbuffers(1, &depth_stencil_buffer_);
    }
  }

  inline const std::shared_ptr<Texture> color_attachment() const {
    return color_attachment_;
  };

 private:
  Framebuffer() : BaseFramebuffer() {}

  void SetColorAttachment(const std::shared_ptr<Texture> color_attachment) {
    color_attachment_ = color_attachment;
  }
  void InitTexture() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           color_attachment_->id(), 0);

    glGenRenderbuffers(1, &depth_stencil_buffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          color_attachment_->width(),
                          color_attachment_->height());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, depth_stencil_buffer_);
  }

  uint32_t depth_stencil_buffer_{0};
  std::shared_ptr<Texture> color_attachment_{nullptr};
};

class DepthMap : public BaseFramebuffer {
 public:
  static std::unique_ptr<DepthMap> Create(int width, int height) {
    auto shadowMap = std::unique_ptr<DepthMap>(new DepthMap());

    if (!shadowMap->GenerateDepthMapTexture(width, height) ||
        !shadowMap->Init()) {
      return nullptr;
    }

    return std::move(shadowMap);
  }
  ~DepthMap() {}

  const std::shared_ptr<Texture> depth_map() const { return depth_map_; }

 private:
  DepthMap() : BaseFramebuffer() {}

  bool GenerateDepthMapTexture(int width, int height) {
    depth_map_ = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    if (!depth_map_) {
      return false;
    }
    depth_map_->SetFilter(GL_NEAREST, GL_NEAREST);
    depth_map_->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    depth_map_->SetBorderColor(glm::vec4(1.0f));

    return true;
  }

  void InitTexture() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depth_map_->id(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }

  std::shared_ptr<Texture> depth_map_{nullptr};
};

#endif