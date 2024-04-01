#ifndef INCLUDED_TEXTURE_HPP
#define INCLUDED_TEXTURE_HPP

#include "Image.hpp"

class Texture
{
    public:
        ~Texture();
        static std::unique_ptr<Texture> create(const Image* image);
        static std::unique_ptr<Texture> create(const std::string& filename);

        void            active(uint32_t n);
        void            bind();

        void            setFilter(uint32_t minFilter, uint32_t magFilter) const;
        void            setWrap(uint32_t sWrap, uint32_t tWrap) const;
        const uint32_t  get() const;

    private:
        Texture();

        void createTexture();
        void setTextureFromImage(const Image* image);

        uint32_t mTexture { 0 };
};

#endif