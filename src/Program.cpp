#include "Program.hpp"

Program::Program()
{}

Program::~Program() {
    if (mId) {
        glDeleteProgram(mId);
    }
}

std::unique_ptr<Program> Program::create(
    const std::vector<std::shared_ptr<Shader>>& shaders)
{
    auto program = std::unique_ptr<Program>(new Program());
    if (!program->link(shaders))
    {
        return nullptr;
    }

    return std::move(program);
}

std::unique_ptr<Program> Program::create(
    const std::string& vsFilename,
    const std::string& fsFilename) {
    std::shared_ptr<Shader> vs = Shader::createFromFile(vsFilename, GL_VERTEX_SHADER);
    std::shared_ptr<Shader> fs = Shader::createFromFile(fsFilename, GL_FRAGMENT_SHADER);
    if (!vs || !fs)
    {
        return nullptr;
    }
    SPDLOG_INFO("vertex shader id : {}", vs->getId());
    SPDLOG_INFO("fragment shader id : {}", vs->getId());

    return create({vs, fs});
}

bool Program::link(const std::vector<std::shared_ptr<Shader>>& shaders)
{
    mId = glCreateProgram();
    for (auto& shader: shaders)
    {
        glAttachShader(mId, shader->getId());
    }
    glLinkProgram(mId);

    int success = 0;
    glGetProgramiv(mId, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(mId, 512, nullptr, infoLog);
        SPDLOG_ERROR("ERROR::PROGRAM::LINKING_FAILED");
        SPDLOG_ERROR("{}", infoLog);
    }

    return success;
}

void Program::use() const
{
    glUseProgram(mId);
}

uint32_t Program::getId() const
{
    return mId;
}

uint32_t Program::getUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(mId, name.c_str());
}

void Program::setUniform(const std::string& name, int value) const
{
    glUniform1i(getUniformLocation(name), value);
}

void Program::setUniform(const std::string& name, float value) const
{
    glUniform1f(getUniformLocation(name), value);
}

void Program::setUniform(const std::string& name, const glm::vec2& value) const 
{
    glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const std::string& name, const glm::vec3& value) const 
{
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const std::string& name, const glm::vec4& value) const 
{
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const std::string& name, const glm::mat4& value) const 
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
