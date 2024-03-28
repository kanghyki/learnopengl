#include "Texture.hpp"

Texture::Texture() : mTexture(0)
{}

Texture::~Texture()
{
    if (mTexture)
    {
        glDeleteTextures(1, &mTexture);
    }
}

std::unique_ptr<Texture> Texture::createFromImage(const Image* image)
{
    auto texture = std::unique_ptr<Texture>(new Texture());
    texture->createTexture();
    texture->setTextureFromImage(image);

    return std::move(texture);
}

void Texture::setFilter(uint32_t minFilter, uint32_t magFilter) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::setWrap(uint32_t sWrap, uint32_t tWrap) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

const uint32_t Texture::get() const
{
    return mTexture;
}

void Texture::createTexture() {
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::setTextureFromImage(const Image* image) {
    GLenum format = GL_RGBA;

    switch (image->getChannelCount())
    {
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
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        image->getWidth(), image->getHeight(), 0,
        format, GL_UNSIGNED_BYTE,
        image->getData());
    glGenerateMipmap(GL_TEXTURE_2D);
}