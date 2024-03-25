#include "Context.hpp"
#include <imgui.h>

Context::Context() : mVAO(nullptr), mVBO(nullptr), mEBO(nullptr), mIsActiveWireFrame(false)
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
    static float prevTime = 0;
    static int frames = 0;
    static float fps = 0.0f;
    static int prevFrames = 0;

    frames++;
    float currTime = glfwGetTime();
    if (currTime - prevTime >= 1.0)
    {
        prevFrames = frames;
        fps = 1000.0f / frames;
        prevTime = currTime;
        frames = 0;
    }
    if (ImGui::Begin("Hello, ImGui"))
    {
        ImGui::Text("%.3f ms/frame (%dfps)", fps, prevFrames);
        if (ImGui::Checkbox("WireFrame", &mIsActiveWireFrame))
        {
            if (mIsActiveWireFrame)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    }
    ImGui::End();

    float alpha = (sin(glfwGetTime()) / 2.0f) + 0.5f;
    mProgram->setUniformValue("alpha", alpha);

    glClear(GL_COLOR_BUFFER_BIT);
    mProgram->use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Context::init()
{
    auto vertexShader = Shader::createFromFile("shader/triangle.vs", GL_VERTEX_SHADER);
    auto fragmentShader = Shader::createFromFile("shader/triangle.fs", GL_FRAGMENT_SHADER);
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

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    SPDLOG_INFO("Maximum nr of vertex attributes supported: {}", nrAttributes);

    float vertices[] =
    {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    unsigned int indices[] =
    {
        0, 1, 2,
    };

    mVAO = VertexArray::create();
    mVBO = Buffer::create(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(vertices));
    if (!mVBO)
    {
        return false;
    }
    SPDLOG_INFO("VBO id : {}", mVBO->get());
    mVAO->setAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    mVAO->setAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, sizeof(float) * 3);

    mEBO = Buffer::create(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(indices));
    if (!mEBO)
    {
        return false;
    }
    SPDLOG_INFO("EBO id : {}", mEBO->get());

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    return true;
}
