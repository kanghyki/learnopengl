#ifndef INCLUDED_PROGRAM_HPP
#define INCLUDED_PROGRAM_HPP

#include "Common.hpp"
#include "Shader.hpp"

class Program
{
    public:
        ~Program();
        static std::unique_ptr<Program> create(const std::vector<std::shared_ptr<Shader>>& shaders);
        uint32_t                        get() const;
        void                            Use() const;

    private:
        Program();
        bool link(const std::vector<std::shared_ptr<Shader>>& shaders);

        uint32_t program;
};

#endif