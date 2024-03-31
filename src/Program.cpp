#include "Program.hpp"

Program::Program()
{}

Program::~Program() {
    if (mProgram) {
        glDeleteProgram(mProgram);
    }
}

std::unique_ptr<Program> Program::create(const std::vector<std::shared_ptr<Shader>>& shaders)
{
    auto program = std::unique_ptr<Program>(new Program());
    if (!program->link(shaders))
    {
        return nullptr;
    }

    return std::move(program);
}

bool Program::link(const std::vector<std::shared_ptr<Shader>>& shaders)
{
    mProgram = glCreateProgram();
    for (auto& shader: shaders)
    {
        glAttachShader(mProgram, shader->get());
    }
    glLinkProgram(mProgram);

    int success = 0;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(mProgram, 512, nullptr, infoLog);
        SPDLOG_ERROR("ERROR::PROGRAM::LINKING_FAILED");
        SPDLOG_ERROR("{}", infoLog);
    }

    return success;
}

void Program::use() const
{
    glUseProgram(mProgram);
}

uint32_t Program::get() const
{
    return mProgram;
}

uint32_t Program::getUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(mProgram, name.c_str());
}

void Program::setUniform(const std::string& name, int value) const
{
    glUniform1i(getUniformLocation(name), value);
}

void Program::setUniform(const std::string& name, float value) const
{
    glUniform1f(getUniformLocation(name), value);
}

void Program::setUniform(const std::string& name, const glm::vec3& value) const 
{
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const std::string& name, const glm::mat4& value) const 
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
