#include "FrameBuffer.hpp"

Framebuffer::Framebuffer()
{}

Framebuffer::~Framebuffer()
{
    if (mDepthStencilBuffer) {
        glDeleteRenderbuffers(1, &mDepthStencilBuffer);
    }
    if (mId) {
        glDeleteFramebuffers(1, &mId);
    }
}

std::unique_ptr<Framebuffer> Framebuffer::create(const std::shared_ptr<Texture> colorAttachment)
{
    auto framebuffer = std::unique_ptr<Framebuffer>(new Framebuffer());
    if (!framebuffer->initWithColorAttachment(colorAttachment))
    {
        return nullptr;
    }

    return std::move(framebuffer);
}

void Framebuffer::bindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, mId);
}

bool Framebuffer::initWithColorAttachment(const std::shared_ptr<Texture> colorAttachment)
{
    mColorAttachment = colorAttachment;
    glGenFramebuffers(1, &mId);
    glBindFramebuffer(GL_FRAMEBUFFER, mId);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment->getId(), 0);

    glGenRenderbuffers(1, &mDepthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, colorAttachment->getWidth(), colorAttachment->getHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilBuffer);

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create framebuffer: {}", status);
        return false;
    }
    bindToDefault();

    return true;
}

const uint32_t Framebuffer::getId() const
{
    return mId;
}

const std::shared_ptr<Texture> Framebuffer::getColorAttachment() const
{
    return mColorAttachment;
}