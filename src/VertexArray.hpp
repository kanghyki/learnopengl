#ifndef INCLUDED_VERTEX_ARRAY_HPP
#define INCLUDED_VERTEX_ARRAY_HPP

#include "common.hpp"

class VertexArray {
public:
  ~VertexArray();
  static std::unique_ptr<VertexArray> create();

  void bind() const;

  uint32_t getId() const;
  void setAttrib(uint32_t attribIndex, int count, uint32_t type,
                 bool normalized, size_t stride, uint64_t offset) const;

private:
  VertexArray();

  void init();

  uint32_t mId{0};
};

#endif