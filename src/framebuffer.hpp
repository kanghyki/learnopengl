#ifndef INCLUDED_FRAMEBUFFER_HPP
#define INCLUDED_FRAMEBUFFER_HPP

#include "common.hpp"
#include "texture.hpp"

class BaseFramebuffer {
 public:
  inline static void BindToDefault(uint32_t target = GL_FRAMEBUFFER) {
    glBindFramebuffer(target, 0);
  };

  BaseFramebuffer() {}
  ~BaseFramebuffer() {
    if (id_) {
      glDeleteFramebuffers(1, &id_);
    }
  }

  virtual inline void Bind(uint32_t target = GL_FRAMEBUFFER) final {
    glBindFramebuffer(target, id_);
  }
  virtual inline uint32_t id() const final { return id_; }
  virtual bool Init() final {
    bool ret = true;

    glGenFramebuffers(1, &id_);
    Bind();

    InitSomething();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      SPDLOG_ERROR("failed to create framebuffer");
      ret = false;
    }
    BindToDefault();
    return ret;
  }

 protected:
  uint32_t id_{0};

  virtual void InitSomething() = 0;

 private:
  BaseFramebuffer(const BaseFramebuffer&);
  BaseFramebuffer& operator=(const BaseFramebuffer&);
};

class Framebuffer : public BaseFramebuffer {
 public:
  static std::unique_ptr<Framebuffer> Create(
      const std::vector<std::shared_ptr<Texture2d>> color_attachments) {
    auto framebuffer = std::unique_ptr<Framebuffer>(new Framebuffer());

    framebuffer->set_color_attachments(color_attachments);
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

  inline const std::shared_ptr<Texture2d> color_attachment(int i) const {
    if (i >= 0 && i < color_attachments_.size()) {
      return color_attachments_[i];
    }

    return nullptr;
  };

 private:
  Framebuffer() : BaseFramebuffer() {}

  void InitSomething() {
    for (size_t i = 0; i < color_attachments_.size(); ++i) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                             GL_TEXTURE_2D, color_attachments_[i]->id(), 0);
    }

    if (color_attachments_.size() > 0) {
      std::vector<GLenum> attachments(color_attachments_.size());
      for (size_t i = 0; i < color_attachments_.size(); ++i) {
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
      }
      glDrawBuffers(color_attachments_.size(), attachments.data());

      // depth stencil buffer
      glGenRenderbuffers(1, &depth_stencil_buffer_);
      glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer_);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                            color_attachments_[0]->width(),
                            color_attachments_[0]->height());
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                GL_RENDERBUFFER, depth_stencil_buffer_);
    }
  }

  void set_color_attachments(
      const std::vector<std::shared_ptr<Texture2d>> color_attachments) {
    color_attachments_ = color_attachments;
  }

  uint32_t depth_stencil_buffer_{0};
  std::vector<std::shared_ptr<Texture2d>> color_attachments_;
};

enum DepthMapType {
  k2D,
  k3D,
};

class DepthMap : public BaseFramebuffer {
 public:
  static std::unique_ptr<DepthMap> Create(int size, DepthMapType type) {
    auto depth_map = std::unique_ptr<DepthMap>(new DepthMap(type));

    switch (type) {
      case k2D:
        depth_map->GenerateDepthMap2dTexture(size, size);
        break;
      case k3D:
        depth_map->GenerateDepthMap3dTexture(size, size, size);
        break;
      default:
        return nullptr;
    }

    if (!depth_map->Init()) {
      return nullptr;
    }

    return std::move(depth_map);
  }
  ~DepthMap() {}

  const std::shared_ptr<Texture2d> depth_map() const { return depth_map_2d_; }
  const std::shared_ptr<Texture3d> depth_map_3d() const {
    return depth_map_3d_;
  }

 private:
  DepthMap(DepthMapType type) : BaseFramebuffer(), type_(type) {}

  void InitSomething() {
    if (type_ == k2D) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                             depth_map_2d_->id(), 0);
    } else {
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           depth_map_3d_->id(), 0);
    }
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }

  bool GenerateDepthMap2dTexture(int width, int height) {
    depth_map_2d_ = Texture2d::Create(width, height, GL_DEPTH_COMPONENT,
                                      GL_DEPTH_COMPONENT, GL_FLOAT);
    if (!depth_map_2d_) {
      return false;
    }
    depth_map_2d_->SetFilter(GL_NEAREST, GL_NEAREST);
    depth_map_2d_->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    depth_map_2d_->SetBorderColor(glm::vec4(1.0f));

    return true;
  }

  bool GenerateDepthMap3dTexture(int width, int height, int length) {
    depth_map_3d_ =
        Texture3d::Create(width, height, length, GL_DEPTH_COMPONENT, GL_FLOAT);
    if (!depth_map_3d_) {
      return false;
    }

    return true;
  }

  std::shared_ptr<Texture2d> depth_map_2d_{nullptr};
  std::shared_ptr<Texture3d> depth_map_3d_{nullptr};
  const DepthMapType type_{k2D};
};

#endif