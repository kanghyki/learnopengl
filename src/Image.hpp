#ifndef INCLUDED_IMAGE_HPP
#define INCLUDED_IMAGE_HPP

#include "Common.hpp"

class Image
{
public:
    static std::unique_ptr<Image> load(const std::string& filepath);
    ~Image();

    const uint8_t*  getData() const;
    int             getWidth() const;
    int             getHeight() const;
    int             getChannelCount() const;

private:
    Image();
    bool loadFile(const std::string& filepath);

    int         mWidth;
    int         mHeight;
    int         mChannelCount;
    uint8_t*    mData;
};

#endif