#include "Image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Image::Image() : mWidth(0), mHeight(0), mChannelCount(0), mData(nullptr)
{}

Image::~Image()
{
    if (mData)
    {
        stbi_image_free(mData);
    }
}

std::unique_ptr<Image> Image::load(const std::string& filepath)
{
    auto image = std::unique_ptr<Image>(new Image());
    if (!image->loadFile(filepath))
    {
        return nullptr;
    }

    return std::move(image);
}

bool Image::loadFile(const std::string& filepath)
{
    stbi_set_flip_vertically_on_load(true);
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