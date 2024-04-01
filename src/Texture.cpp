#include "Texture.hpp"

Texture::Texture()
{}

Texture::~Texture()
{
    if (mTexture)
    {
        glDeleteTextures(1, &mTexture);
    }
}

std::unique_ptr<Texture> Texture::create(const Image* image)
{
    auto texture = std::unique_ptr<Texture>(new Texture());
    texture->createTexture();
    texture->setTextureFromImage(image);

    return std::move(texture);
}

std::unique_ptr<Texture> Texture::create(const std::string& filename)
{
    auto image = Image::load(filename);
    if (!image)
    {
        return nullptr;
    }
    SPDLOG_INFO("image: {}x{}, {} channels", image->getWidth(), image->getHeight(), image->getChannelCount());
    auto texture = std::unique_ptr<Texture>(new Texture());
    texture->createTexture();
    texture->setTextureFromImage(image.get());

    return std::move(texture);
}

void Texture::active(uint32_t n)
{
    switch (n) {
        case 0:
            glActiveTexture(GL_TEXTURE0);
            break;
        case 1:
            glActiveTexture(GL_TEXTURE1);
            break;
        case 2:
            glActiveTexture(GL_TEXTURE2);
            break;
        case 3:
            glActiveTexture(GL_TEXTURE3);
            break;
        case 4:
            glActiveTexture(GL_TEXTURE4);
            break;
        case 5:
            glActiveTexture(GL_TEXTURE5);
            break;
        case 6:
            glActiveTexture(GL_TEXTURE6);
            break;
        case 7:
            glActiveTexture(GL_TEXTURE7);
            break;
        case 8:
            glActiveTexture(GL_TEXTURE8);
            break;
        case 9:
            glActiveTexture(GL_TEXTURE9);
            break;
        case 10:
            glActiveTexture(GL_TEXTURE10);
            break;
        case 11:
            glActiveTexture(GL_TEXTURE11);
            break;
        case 12:
            glActiveTexture(GL_TEXTURE12);
            break;
        case 13:
            glActiveTexture(GL_TEXTURE13);
            break;
        case 14:
            glActiveTexture(GL_TEXTURE14);
            break;
        case 15:
            glActiveTexture(GL_TEXTURE15);
            break;
        case 16:
            glActiveTexture(GL_TEXTURE16);
            break;
        case 17:
            glActiveTexture(GL_TEXTURE17);
            break;
        case 18:
            glActiveTexture(GL_TEXTURE18);
            break;
        case 19:
            glActiveTexture(GL_TEXTURE19);
            break;
        default:
            break;
    }
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, mTexture);
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