#ifndef INCLUDED_VERTEX_ARRAY_HPP
#define INCLUDED_VERTEX_ARRAY_HPP

#include "common.hpp"

class VertexArray {
  public:
    static std::unique_ptr<VertexArray> Create();
    ~VertexArray();

    inline void Bind() const { glBindVertexArray(id_); }
    void SetAttrib(uint32_t attrib_index, int count, uint32_t type, bool normalized, size_t stride,
                   uint64_t offset) const;

    inline uint32_t id() const { return id_; }

  private:
    VertexArray();

    void Init();

    uint32_t id_{0};
};

#endif