#include "Shader.hpp"

Shader::Shader() : shader(0)
{}

Shader::~Shader()
{
    if (this->shader)
    {
        glDeleteShader(this->shader);
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

    this->shader = glCreateShader(shaderType);
    glShaderSource(this->shader, 1, &pSource, &codeLength);
    glCompileShader(this->shader);

    int success = 0;
    glGetShaderiv(this->shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(this->shader, 512, nullptr, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::COMPILATION_FAILED: {}", fileName);
        SPDLOG_ERROR("{}", infoLog);
    }

    return success;
}

uint32_t Shader::get() const
{ 
    return this->shader;
}