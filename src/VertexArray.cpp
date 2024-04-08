#include "VertexArray.hpp"

VertexArray::VertexArray() {}

VertexArray::~VertexArray() {
  if (id_) {
    glDeleteVertexArrays(1, &id_);
  }
}

std::unique_ptr<VertexArray> VertexArray::Create() {
  auto vertexLayout = std::unique_ptr<VertexArray>(new VertexArray());
  vertexLayout->Init();

  return std::move(vertexLayout);
}

void VertexArray::SetAttrib(uint32_t attrib_index, int count, uint32_t type,
                            bool normalized, size_t stride,
                            uint64_t offset) const {
  glVertexAttribPointer(attrib_index, count, type, normalized, (GLsizei)stride,
                        (const void *)offset);
  glEnableVertexAttribArray(attrib_index);
}

void VertexArray::Init() {
  glGenVertexArrays(1, &id_);
  Bind();
}