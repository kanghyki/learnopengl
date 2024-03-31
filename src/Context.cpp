#include "Context.hpp"
#include "Image.hpp"
#include <imgui.h>

Context::Context() :
    mWidth(WINDOW_WIDTH),
    mHeight(WINDOW_HEIGHT),
    mFragType(0),
    mIsActiveWireFrame(false),
    mIsEnableDepthBuffer(true),
    mProgram(nullptr),
    mVAO(nullptr),
    mVBO(nullptr),
    mEBO(nullptr),
    mTexture(nullptr),
    mTexture2(nullptr),
    mCamera(),
    mPrevMousePos(0.0f),
    mCameraDirectionControl(false),
    mClearColor(0.2f, 0.3f, 0.3f, 1.0f)
{
    glEnable(GL_DEPTH_TEST);
}

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
    /*
     * *********
     * * ImGui *
     * *********
     */
    {
    static float    prevTime = 0;
    static int      frames = 0;
    static float    fps = 0.0f;
    static int      prevFrames = 0;

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
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Background");
        if (ImGui::ColorEdit4("color", glm::value_ptr(mClearColor)))
        {
            SPDLOG_INFO("color r: {} g : {} b : {} a : {}", mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);
            glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        /*
         * Camera
         */
        ImGui::Text("Camera");
        ImGui::SliderFloat3("Position", &mCamera.pos.x, -10.0f, 10.0f, "%.3f");
        ImGui::SliderFloat3("Target", &mCamera.target.x, -10.0f, 10.0f, "%.3f");
        ImGui::SliderFloat3("Up", &mCamera.up.x, -10.0f, 10.0f, "%.3f");
        if (ImGui::Button("Reset camera"))
        {
            mCamera.reset();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset resolution (test)"))
        {
            mWidth = WINDOW_WIDTH;
            mHeight = WINDOW_HEIGHT;
            reshape(mHeight, mHeight);
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        /*
         * Extra
         */
        ImGui::Text("Extras");
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
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        /*
         * Texture
         */
        ImGui::Text("Texture");
        if (ImGui::RadioButton("1", &mFragType, 0))
        {
            mProgram->setUniform("type", mFragType);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("2", &mFragType, 1))
        {
            mProgram->setUniform("type", mFragType);
        }
    }
    ImGui::End();
    }


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

    auto projection = glm::perspective(glm::radians(45.0f), (float)mWidth / (float)mHeight, 0.01f, 30.0f);
    auto view = mCamera.getViewMatrix();

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
                    glm::vec3(0.8f, 0.8f, 0.8f)
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

    glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);

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

void Context::processKeyboardInput(GLFWwindow* window)
{
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) mCamera.pos += cameraSpeed * mCamera.target;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) mCamera.pos -= cameraSpeed * mCamera.target;
    auto cameraRight = glm::normalize(glm::cross(mCamera.up, -mCamera.target));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) mCamera.pos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) mCamera.pos -= cameraSpeed * cameraRight;
    auto cameraUp = glm::cross(-mCamera.target, cameraRight);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) mCamera.pos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) mCamera.pos -= cameraSpeed * cameraUp;
}

void Context::processMouseMove(double x, double y)
{
    if (mCameraDirectionControl)
    {
        auto pos = glm::vec2((float)x, (float)y);
        auto deltaPos = pos - mPrevMousePos;

        const float cameraRotSpeed = 0.25f;
        mCamera.yaw -= deltaPos.x * cameraRotSpeed;
        mCamera.pitch -= deltaPos.y * cameraRotSpeed;

        if (mCamera.yaw < 0.0f)   mCamera.yaw += 360.0f;
        if (mCamera.yaw > 360.0f) mCamera.yaw -= 360.0f;

        if (mCamera.pitch > 89.0f)  mCamera.pitch = 89.0f;
        if (mCamera.pitch < -89.0f) mCamera.pitch = -89.0f;

        mPrevMousePos = pos;
    }
}

void Context::processMouseButton(int button, int action, double x, double y)
{
    mPrevMousePos = glm::vec2((float)x, (float)y);
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            mCameraDirectionControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mCameraDirectionControl = false;
        }
    }
}

void Context::processMouseScroll(double xoffset, double yoffset)
{
    if (yoffset > 0)
    {
        mCamera.pos += mCamera.target * (float)abs(yoffset) * 0.6f;
    }
    else if (yoffset < 0)
    {
        mCamera.pos -= mCamera.target * (float)abs(yoffset) * 0.6f;
    }

}

void Context::reshape(int width, int height)
{
    mWidth = width;
    mHeight = height;
    glViewport(0, 0, mWidth, mHeight);
}
