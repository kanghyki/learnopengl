#ifndef INCLUDED_PROGRAM_HPP
#define INCLUDED_PROGRAM_HPP

#include "common.hpp"
#include "Shader.hpp"

class Program
{
    public:
        ~Program();
        static std::unique_ptr<Program> create(const std::vector<std::shared_ptr<Shader>>& shaders);
        uint32_t                        get() const;
        void                            use() const;

        void        setUniform(const std::string& name, int value) const;
        void        setUniform(const std::string& name, float value) const;
        void        setUniform(const std::string& name, glm::mat4 value) const;
        uint32_t    getUniformLocation(const std::string& name) const;

    private:
        Program();
        bool link(const std::vector<std::shared_ptr<Shader>>& shaders);

        uint32_t mProgram;
};

#endif
