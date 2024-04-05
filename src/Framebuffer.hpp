#ifndef INCLUDED_FRAMEBUFFER_HPP
#define INCLUDED_FRAMEBUFFER_HPP

#include "common.hpp"
#include "Texture.hpp"

class Framebuffer
{
    public:
        ~Framebuffer();

        static std::unique_ptr<Framebuffer> create(const std::shared_ptr<Texture> colorAttachment);
        static void                         bindToDefault();

        void bind() const;

        const uint32_t                  getId() const;
        const std::shared_ptr<Texture>  getColorAttachment() const;

    private:
        Framebuffer();

        bool initWithColorAttachment(const std::shared_ptr<Texture> colorAttachment);

        uint32_t                    mId                 { 0 };
        uint32_t                    mDepthStencilBuffer { 0 };
        std::shared_ptr<Texture>    mColorAttachment    { nullptr };
};

#endif