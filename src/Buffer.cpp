#include "Buffer.hpp"

Buffer::Buffer() : mBuffer(0), mBufferType(0), mUsage(0)
{}

Buffer::~Buffer()
{
    if (mBuffer)
    {
        glDeleteBuffers(1, &mBuffer);
    }
}

std::unique_ptr<Buffer> Buffer::create(uint32_t bufferType, uint32_t usage, const void* data, size_t dataSize)
{
    auto buffer = std::unique_ptr<Buffer>(new Buffer());
    if (!buffer->init(bufferType, usage, data, dataSize))
    {
        return nullptr;
    }

    return std::move(buffer);
}

bool Buffer::init(uint32_t bufferType, uint32_t usage, const void* data, size_t dataSize)
{
    mBufferType = bufferType;
    mUsage = usage;
    glGenBuffers(1, &mBuffer);
    glBindBuffer(mBufferType, mBuffer);
    glBufferData(mBufferType, dataSize, data, usage);

    return true;
}

uint32_t Buffer::get() const
{
    return mBuffer;
}