#include "Shader.hpp"

Shader::Shader()
{}

Shader::~Shader()
{
    if (mShader)
    {
        glDeleteShader(mShader);
    }
}

std::shared_ptr<Shader> Shader::createFromFile(const std::string& fileName, GLenum shaderType)
{
    auto shader = std::unique_ptr<Shader>(new Shader());
    if (!shader->loadFile(fileName, shaderType))
    {
        return nullptr;
    }
    return std::move(shader);
}

bool Shader::loadFile(const std::string& fileName, GLenum shaderType)
{
    auto result = loadTextFile(fileName);
    if (!result.has_value())
    {
        return false;
    }

    std::string& code = result.value();
    const char* pSource = code.c_str();
    int32_t codeLength = code.length();

    mShader = glCreateShader(shaderType);
    glShaderSource(mShader, 1, &pSource, &codeLength);
    glCompileShader(mShader);

    int success = 0;
    glGetShaderiv(mShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(mShader, 512, nullptr, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::COMPILATION_FAILED: {}", fileName);
        SPDLOG_ERROR("{}", infoLog);
    }

    return success;
}

uint32_t Shader::get() const
{ 
    return mShader;
}