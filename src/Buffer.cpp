#include "Buffer.hpp"

Buffer::Buffer() {}

Buffer::~Buffer() {
  if (mId) {
    glDeleteBuffers(1, &mId);
  }
}

std::unique_ptr<Buffer> Buffer::create(uint32_t bufferType, uint32_t usage,
                                       const void *data, size_t stride,
                                       size_t count) {
  auto buffer = std::unique_ptr<Buffer>(new Buffer());
  buffer->init(bufferType, usage, data, stride, count);

  return std::move(buffer);
}

void Buffer::init(uint32_t bufferType, uint32_t usage, const void *data,
                  size_t stride, size_t count) {
  mBufferType = bufferType;
  mUsage = usage;
  mStride = stride;
  mCount = count;
  glGenBuffers(1, &mId);
  glBindBuffer(mBufferType, mId);
  glBufferData(mBufferType, mStride * mCount, data, usage);
}

uint32_t Buffer::getId() const { return mId; }

size_t Buffer::getStride() const { return mStride; }

size_t Buffer::getCount() const { return mCount; }