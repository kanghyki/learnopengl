#include "VertexArray.hpp"

VertexArray::VertexArray() {}

VertexArray::~VertexArray() {
  if (mId) {
    glDeleteVertexArrays(1, &mId);
  }
}

std::unique_ptr<VertexArray> VertexArray::create() {
  auto vertexLayout = std::unique_ptr<VertexArray>(new VertexArray());
  vertexLayout->init();

  return std::move(vertexLayout);
}

void VertexArray::setAttrib(uint32_t attribIndex, int count, uint32_t type,
                            bool normalized, size_t stride,
                            uint64_t offset) const {
  glVertexAttribPointer(attribIndex, count, type, normalized, (GLsizei)stride,
                        (const void *)offset);
  glEnableVertexAttribArray(attribIndex);
}

void VertexArray::init() {
  glGenVertexArrays(1, &mId);
  bind();
}

uint32_t VertexArray::getId() const { return mId; }

void VertexArray::bind() const { glBindVertexArray(mId); }