#include "Context.hpp"

Context::Context()
{}

Context::~Context()
{}

std::unique_ptr<Context> Context::create()
{
    auto context = std::unique_ptr<Context>(new Context());
    if (!context->init())
    {
        return nullptr;
    }

    return std::move(context);
}

void Context::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    mProgram->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Context::init()
{
    auto vertexShader = Shader::createFromFile("shader/vs.glsl", GL_VERTEX_SHADER);
    auto fragmentShader = Shader::createFromFile("shader/fs.glsl", GL_FRAGMENT_SHADER);
    if (!vertexShader || !fragmentShader)
    {
        return false;
    }
    SPDLOG_INFO("vertex shader id : {}", vertexShader->get());
    SPDLOG_INFO("fragment shader id : {}", fragmentShader->get());

    mProgram = Program::create({vertexShader, fragmentShader});
    if (!mProgram)
    {
        return false;
    }
    SPDLOG_INFO("program id: {}", mProgram->get());

    float vertices[] = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    mVAO = VertexArray::create();
    mVBO = Buffer::create(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(vertices));
    if (!mVBO)
    {
        return false;
    }
    SPDLOG_INFO("VBO id : {}", mVBO->get());
    mVAO->setAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    mEBO = Buffer::create(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(indices));
    if (!mEBO)
    {
        return false;
    }
    SPDLOG_INFO("EBO id : {}", mEBO->get());

    // wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    return true;
}