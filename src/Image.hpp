#ifndef INCLUDED_IMAGE_HPP
#define INCLUDED_IMAGE_HPP

#include "common.hpp"

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

    int         mWidth          { 0 };
    int         mHeight         { 0 };
    int         mChannelCount   { 0 };
    uint8_t*    mData           { nullptr };
};

#endif