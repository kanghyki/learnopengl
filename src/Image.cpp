#include "Image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Image::Image()
{}

Image::~Image()
{
    if (mData)
    {
        stbi_image_free(mData);
    }
}

std::unique_ptr<Image> Image::load(const std::string& filepath, bool flipVertical)
{
    auto image = std::unique_ptr<Image>(new Image());
    if (!image->loadFile(filepath, flipVertical))
    {
        return nullptr;
    }

    return std::move(image);
}

std::unique_ptr<Image> Image::create(int width, int height, int channelCount)
{
    auto image = std::unique_ptr<Image>(new Image());
    if (!image->allocate(width, height, channelCount))
    {
        return nullptr;
    }

    return std::move(image);
}

bool Image::allocate(int width, int height, int channelCount)
{
    mWidth = width;
    mHeight = height;
    mChannelCount = channelCount;
    if (!(mData = (uint8_t*)malloc(mWidth * mHeight * mChannelCount)))
    {
        return false;
    }

    return true;
}

std::unique_ptr<Image> Image::createSingleColorImage(int width, int height, const glm::vec4& color)
{
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
    uint8_t rgba[4] = {
        (uint8_t)clamped.r, 
        (uint8_t)clamped.g, 
        (uint8_t)clamped.b, 
        (uint8_t)clamped.a, 
    };
    auto image = create(width, height, 4);
    for (int i = 0; i < width * height; ++i)
    {
        memcpy(image->mData + 4 * i, rgba, 4);
    }

    return std::move(image);
}

bool Image::loadFile(const std::string& filepath, bool flipVertical)
{
    stbi_set_flip_vertically_on_load(flipVertical);
    mData = stbi_load(filepath.c_str(), &mWidth, &mHeight, &mChannelCount, 0);
    if (!mData)
    {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }

    return true;
}

const uint8_t* Image::getData() const
{
    return mData;
}

int Image::getWidth() const
{
    return mWidth;
}

int Image::getHeight() const
{
    return mHeight;
}

int Image::getChannelCount() const
{
    return mChannelCount;
}