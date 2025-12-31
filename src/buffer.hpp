#ifndef INCLUDED_BUFFER_HPP
#define INCLUDED_BUFFER_HPP

#include "common.hpp"

class Buffer {
  public:
    ~Buffer();
    static std::unique_ptr<Buffer> Create(uint32_t buffer_type, uint32_t usage, const void* data,
                                          size_t stride, size_t count);

    inline void Bind() const { glBindBuffer(buffer_type_, id_); }

    inline const uint32_t id() const { return id_; }
    inline size_t stride() const { return stride_; }
    inline size_t count() const { return count_; }

  private:
    Buffer();
    void Init(uint32_t buffer_type, uint32_t usage, const void* data, size_t stride, size_t count);

    uint32_t id_{0};
    uint32_t buffer_type_{0};
    uint32_t usage_{0};
    size_t stride_{0};
    size_t count_{0};
};

#endif
