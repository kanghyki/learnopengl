#include "VertexArray.hpp"

VertexArray::VertexArray() : mVAO(0)
{}

VertexArray::~VertexArray()
{
    if (mVAO)
    {
        glDeleteVertexArrays(1, &mVAO);
    }
}

std::unique_ptr<VertexArray> VertexArray::create()
{
    auto vertexLayout = std::unique_ptr<VertexArray>(new VertexArray());
    vertexLayout->init();

    return std::move(vertexLayout);
}

void VertexArray::setAttrib(uint32_t attribIndex, int count, uint32_t type, bool normalized, size_t stride, uint64_t offset) const
{
    glEnableVertexAttribArray(attribIndex);
    glVertexAttribPointer(attribIndex, count, type, normalized, (GLsizei)stride, (const void*)offset);
}

void VertexArray::init()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
}


uint32_t VertexArray::get() const
{
    return mVAO;
}