#include "Context.hpp"
#include "Image.hpp"
#include <imgui.h>

Context::Context() :
    mFragType(0),
    mIsActiveWireFrame(false),
    mIsEnableDepthBuffer(false),
    mProgram(nullptr),
    mVAO(nullptr),
    mVBO(nullptr),
    mEBO(nullptr),
    mTexture(nullptr),
    mTexture2(nullptr)
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

    if (ImGui::Begin("Settings"))
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
        ImGui::SameLine();
        if (ImGui::Checkbox("DepthBuffer", &mIsEnableDepthBuffer))
        {
            if (mIsEnableDepthBuffer)
            {
                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }
        }
        if (ImGui::RadioButton("texture 1", &mFragType, 0))
        {
            mProgram->setUniform("type", mFragType);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Texture 2", &mFragType, 1))
        {
            mProgram->setUniform("type", mFragType);
        }
    }
    ImGui::End();


    if (mIsEnableDepthBuffer)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    std::vector<glm::vec3> cubePositions =
    {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f),
    };

    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    auto projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);

    for (size_t i = 0; i < cubePositions.size(); i++)
    {
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(
                    glm::rotate(
                        model,
                        glm::radians((float)glfwGetTime() * 90.0f + 20.0f * (float)i),
                        glm::vec3(1.0f, 0.5f, 0.0f)
                        ),
                    glm::vec3(0.6f, 0.6f, 0.6f)
                    );
        auto transform = projection * view * model;
        mProgram->setUniform("transform", transform);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

bool Context::init()
{
    auto vertexShader = Shader::createFromFile("shader/my_shader.vs", GL_VERTEX_SHADER);
    auto fragmentShader = Shader::createFromFile("shader/my_shader.fs", GL_FRAGMENT_SHADER);
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
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
    };

    uint32_t indices[] =
    {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    mVAO = VertexArray::create();
    mVBO = Buffer::create(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(vertices));
    if (!mVBO)
    {
        return false;
    }
    SPDLOG_INFO("VBO id : {}", mVBO->get());
    mVAO->setAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    mVAO->setAttrib(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

    mEBO = Buffer::create(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(indices));
    if (!mEBO)
    {
        return false;
    }
    SPDLOG_INFO("EBO id : {}", mEBO->get());

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    auto image = Image::load("./image/1.png");
    if (!image)
    {
        return false;
    }
    SPDLOG_INFO("image: {}x{}, {} channels", image->getWidth(), image->getHeight(), image->getChannelCount());
    mTexture = Texture::createFromImage(image.get());

    auto image2 = Image::load("./image/2.png");
    if (!image2)
    {
        return false;
    }
    SPDLOG_INFO("image2: {}x{}, {} channels", image2->getWidth(), image2->getHeight(), image2->getChannelCount());
    mTexture2 = Texture::createFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture->get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTexture2->get());
    mProgram->use();
    glUniform1i(glGetUniformLocation(mProgram->get(), "tex"), 0);
    glUniform1i(glGetUniformLocation(mProgram->get(), "tex2"), 1);

    return true;
}
