#ifndef INCLUDED_VERTEX_ARRAY_HPP
#define INCLUDED_VERTEX_ARRAY_HPP

#include "common.hpp"

class VertexArray
{
    public:
        ~VertexArray();
        static std::unique_ptr<VertexArray> create();

        uint32_t    get() const;
        void        bind() const;
        void        setAttrib(uint32_t attribIndex, int count, uint32_t type, bool normalized, size_t stride, uint64_t offset) const;
        void        disableAttrib(int attribIndex) const;

    private:
        VertexArray();
        void init();

        uint32_t mVAO { 0 };
};

#endif