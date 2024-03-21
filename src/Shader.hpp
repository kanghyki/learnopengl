#ifndef INCLUDED_SHADER_HPP
#define INCLUDED_SHADER_HPP

#include "Common.hpp"

class Shader
{
    public:
        ~Shader();
        static std::shared_ptr<Shader>  createFromFile(const std::string& fileName, GLenum shaderType);
        uint32_t                        get() const;

      private:
        Shader();
        bool loadFile(const std::string& fileName, GLenum shaderType);

        uint32_t mShader;
};

#endif