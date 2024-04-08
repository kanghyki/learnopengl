#include "Framebuffer.hpp"

Framebuffer::Framebuffer() {}

Framebuffer::~Framebuffer() {
  if (depth_stencil_buffer_) {
    glDeleteRenderbuffers(1, &depth_stencil_buffer_);
  }
  if (id_) {
    glDeleteFramebuffers(1, &id_);
  }
}

std::unique_ptr<Framebuffer> Framebuffer::Create(
    const std::shared_ptr<Texture> color_attachment) {
  auto framebuffer = std::unique_ptr<Framebuffer>(new Framebuffer());
  if (!framebuffer->InitWithColorAttachment(color_attachment)) {
    return nullptr;
  }

  return std::move(framebuffer);
}

bool Framebuffer::InitWithColorAttachment(
    const std::shared_ptr<Texture> color_attachment) {
  color_attachment_ = color_attachment;
  glGenFramebuffers(1, &id_);
  Bind();

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         color_attachment->id(), 0);

  glGenRenderbuffers(1, &depth_stencil_buffer_);
  glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                        color_attachment->width(), color_attachment->height());
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, depth_stencil_buffer_);

  auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    SPDLOG_ERROR("failed to create framebuffer: {}", status);
    return false;
  }
  BindToDefault();

  return true;
}