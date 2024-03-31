#ifndef INCLUDED_BUFFER_HPP
#define INCLUDED_BUFFER_HPP

#include "common.hpp"

class Buffer
{
    public:
        ~Buffer();
        static std::unique_ptr<Buffer>  create(uint32_t bufferType, uint32_t usage, const void* data, size_t dataSize);
        uint32_t                        get() const;

    private:
        Buffer();
        void init(uint32_t bufferType, uint32_t usage, const void *data, size_t dataSize);

        uint32_t mBuffer;
        uint32_t mBufferType;
        uint32_t mUsage;
};

#endif
