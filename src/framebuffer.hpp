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

enum DepthMapType {
  kTwoDimensional,
  kThreeDimensional,
};

union uTexture {
  std::shared_ptr<Texture> two_d;
  std::shared_ptr<CubeTexture> three_d;
};

class DepthMap : public BaseFramebuffer {
 public:
  static std::unique_ptr<DepthMap> Create(int width, int height,
                                          DepthMapType type = kTwoDimensional) {
    auto shadowMap = std::unique_ptr<DepthMap>(new DepthMap(type));

    switch (type) {
      case kTwoDimensional:
        shadowMap->GenerateDepthMap2dTexture(width, height);
        break;
      case kThreeDimensional:
        shadowMap->GenerateDepthMap3dTexture(width, height);
        break;
      default:
        return nullptr;
    }

    if (!shadowMap->Init()) {
      return nullptr;
    }

    return std::move(shadowMap);
  }
  ~DepthMap() {}

  const std::shared_ptr<Texture> depth_map() const { return depth_map_2d_; }
  const std::shared_ptr<CubeTexture> depth_map_3d() const {
    return depth_map_3d_;
  }

 private:
  DepthMap(DepthMapType type) : BaseFramebuffer(), type_(type) {}

  bool GenerateDepthMap2dTexture(int width, int height) {
    depth_map_2d_ =
        Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    if (!depth_map_2d_) {
      return false;
    }
    depth_map_2d_->SetFilter(GL_NEAREST, GL_NEAREST);
    depth_map_2d_->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    depth_map_2d_->SetBorderColor(glm::vec4(1.0f));

    return true;
  }

  bool GenerateDepthMap3dTexture(int width, int height) {
    depth_map_3d_ = CubeTexture::CreateDepthCubeMap(width, height);
    if (!depth_map_3d_) {
      return false;
    }

    return true;
  }

  void InitTexture() {
    if (type_ == kTwoDimensional) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                             depth_map_2d_->id(), 0);
    } else {
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           depth_map_3d_->id(), 0);
    }
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }

  std::shared_ptr<Texture> depth_map_2d_{nullptr};
  std::shared_ptr<CubeTexture> depth_map_3d_{nullptr};
  const DepthMapType type_{kTwoDimensional};
};

#endif