#ifndef INCLUDED_CONTEXT_HPP
#define INCLUDED_CONTEXT_HPP

#include "Common.hpp"
#include "Shader.hpp"
#include "Program.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"

class Context
{
    public:
        ~Context();
        static std::unique_ptr<Context> create();
        void                            render();

    private:
        Context();
        bool init();

        std::unique_ptr<Program>        mProgram;
        std::unique_ptr<Buffer>         mVBO, mEBO;
        std::unique_ptr<VertexArray>    mVAO;
};

#endif