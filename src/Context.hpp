#ifndef INCLUDED_CONTEXT_HPP
#define INCLUDED_CONTEXT_HPP

#include "Common.hpp"
#include "Shader.hpp"
#include "Program.hpp"

class Context
{
    public:
        ~Context();
        static std::unique_ptr<Context> create();
        void                            render();

    private:
        Context();
        bool init();

        std::unique_ptr<Program>    mProgram;
        unsigned int                mVAO, mVBO, mEBO;
};

#endif