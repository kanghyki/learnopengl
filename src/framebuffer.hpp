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

        Initialize();

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            SPDLOG_ERROR("failed to create framebuffer");
            ret = false;
        }
        BindToDefault();
        return ret;
    }

  protected:
    uint32_t id_{0};

    virtual void Initialize() = 0;

  private:
    BaseFramebuffer(const BaseFramebuffer&);
    BaseFramebuffer& operator=(const BaseFramebuffer&);
};

class Framebuffer : public BaseFramebuffer {
  public:
    static std::unique_ptr<Framebuffer>
    Create(const std::vector<std::shared_ptr<Texture2d>> color_attachments) {
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

    void Initialize() {
        for (size_t i = 0; i < color_attachments_.size(); ++i) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                                   color_attachments_[i]->id(), 0);
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
                                  color_attachments_[0]->width(), color_attachments_[0]->height());
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                      depth_stencil_buffer_);
        }
    }

    void set_color_attachments(const std::vector<std::shared_ptr<Texture2d>> color_attachments) {
        color_attachments_ = color_attachments;
    }

    uint32_t depth_stencil_buffer_{0};
    std::vector<std::shared_ptr<Texture2d>> color_attachments_;
};

class DepthMap2d : public BaseFramebuffer {
  public:
    static std::unique_ptr<DepthMap2d> Create(int size) {
        auto depth_map = std::unique_ptr<DepthMap2d>(new DepthMap2d());
        if (!depth_map) {
            return nullptr;
        }
        depth_map->GenerateTexture(size, size);
        if (!depth_map->Init()) {
            return nullptr;
        }

        return std::move(depth_map);
    }
    ~DepthMap2d() {}

    const std::shared_ptr<Texture2d> depth_map() const { return depth_map_; }

  private:
    DepthMap2d() : BaseFramebuffer() {}

    void Initialize() {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_->id(),
                               0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    bool GenerateTexture(int width, int height) {
        depth_map_ =
            Texture2d::Create(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
        if (!depth_map_) {
            return false;
        }
        depth_map_->SetFilter(GL_NEAREST, GL_NEAREST);
        depth_map_->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
        depth_map_->SetBorderColor(glm::vec4(1.0f));

        return true;
    }

    std::shared_ptr<Texture2d> depth_map_{nullptr};
};

class DepthMap3d : public BaseFramebuffer {
  public:
    static std::unique_ptr<DepthMap3d> Create(int size) {
        auto depth_map = std::unique_ptr<DepthMap3d>(new DepthMap3d());
        if (!depth_map) {
            return nullptr;
        }
        depth_map->GenerateTexture(size, size, size);
        if (!depth_map->Init()) {
            return nullptr;
        }

        return std::move(depth_map);
    }
    ~DepthMap3d() {}

    const std::shared_ptr<Texture3d> depth_map() const { return depth_map_; }

  private:
    DepthMap3d() : BaseFramebuffer() {}

    void Initialize() {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map_->id(), 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    bool GenerateTexture(int width, int height, int length) {
        depth_map_ = Texture3d::Create(width, height, length, GL_DEPTH_COMPONENT, GL_FLOAT);
        if (!depth_map_) {
            return false;
        }

        return true;
    }

    std::shared_ptr<Texture3d> depth_map_{nullptr};
};

#endif