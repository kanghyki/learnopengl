#include "Buffer.hpp"

Buffer::Buffer()
{}

Buffer::~Buffer()
{
    if (mBuffer)
    {
        glDeleteBuffers(1, &mBuffer);
    }
}

std::unique_ptr<Buffer> Buffer::create(uint32_t bufferType, uint32_t usage, const void* data, size_t stride, size_t count)
{
    auto buffer = std::unique_ptr<Buffer>(new Buffer());
    buffer->init(bufferType, usage, data, stride, count);

    return std::move(buffer);
}

void Buffer::init(uint32_t bufferType, uint32_t usage, const void *data, size_t stride, size_t count)
{
    mBufferType = bufferType;
    mUsage = usage;
    mStride = stride;
    mCount = count;
    glGenBuffers(1, &mBuffer);
    glBindBuffer(mBufferType, mBuffer);
    glBufferData(mBufferType, mStride * mCount, data, usage);
}

uint32_t Buffer::get() const
{
    return mBuffer;
}

size_t Buffer::getStride() const
{
    return mStride;
}

size_t Buffer::getCount() const
{
    return mCount;
}