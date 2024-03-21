#include "Program.hpp"

Program::Program() : program(0)
{}

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
    this->program = glCreateProgram();
    for (auto& shader: shaders)
    {
        glAttachShader(this->program, shader->get());
    }
    glLinkProgram(this->program);

    int success = 0;
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(this->program, 512, nullptr, infoLog);
        SPDLOG_ERROR("ERROR::PROGRAM::LINKING_FAILED");
        SPDLOG_ERROR("{}", infoLog);
    }

    return success;
}

void Program::Use() const
{
    glUseProgram(this->program);
}

uint32_t Program::get() const
{
    return this->program;
}

Program::~Program() {
    if (this->program) {
        glDeleteProgram(this->program);
    }
}