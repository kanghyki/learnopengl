#ifndef INCLUDED_BUFFER_HPP
#define INCLUDED_BUFFER_HPP

#include "common.hpp"

class Buffer {
public:
  ~Buffer();
  static std::unique_ptr<Buffer> create(uint32_t bufferType, uint32_t usage,
                                        const void *data, size_t stride,
                                        size_t count);
  uint32_t getId() const;
  size_t getStride() const;
  size_t getCount() const;

private:
  Buffer();
  void init(uint32_t bufferType, uint32_t usage, const void *data,
            size_t stride, size_t count);

  uint32_t mId{0};
  uint32_t mBufferType{0};
  uint32_t mUsage{0};
  size_t mStride{0};
  size_t mCount{0};
};

#endif
