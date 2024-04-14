#include "buffer.hpp"

Buffer::Buffer() {}

Buffer::~Buffer() {
  if (id_) {
    glDeleteBuffers(1, &id_);
  }
}

std::unique_ptr<Buffer> Buffer::Create(uint32_t buffer_type, uint32_t usage,
                                       const void *data, size_t stride,
                                       size_t count) {
  auto buffer = std::unique_ptr<Buffer>(new Buffer());
  buffer->Init(buffer_type, usage, data, stride, count);

  return std::move(buffer);
}

void Buffer::Init(uint32_t buffer_type, uint32_t usage, const void *data,
                  size_t stride, size_t count) {
  buffer_type_ = buffer_type;
  usage_ = usage;
  stride_ = stride;
  count_ = count;
  glGenBuffers(1, &id_);
  Bind();
  glBufferData(buffer_type_, stride_ * count_, data, usage);
}