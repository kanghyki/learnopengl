#include "Context.hpp"
#include "Image.hpp"
#include <imgui.h>

Context::Context()
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
    ImGui::Begin("Settings");
    updateImGui();

    ImVec2 pos = ImGui::GetWindowPos();
    auto size = ImGui::GetWindowSize();
    updateGUIwindow(pos.x, pos.y, size.x, size.y);
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetWindowSize(ImVec2(mGUIwidth, mHeight));

    ImGui::End();
    ImGui::Render();

    if (mIsWindowUpdated)
    {
        SPDLOG_INFO("changed viewport");
        glViewport(mGUIx + mGUIwidth, 0, mWidth - (mGUIx + mGUIwidth), mHeight);
        mIsWindowUpdated = false;
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

    auto projection = glm::perspective(glm::radians(45.0f), (float)(mWidth - (mGUIx + mGUIwidth)) / (float)mHeight, 0.01f, 30.0f);
    auto view = mCamera.getViewMatrix();

    auto lightModelTransform =
        glm::translate(glm::mat4(1.0), mLight.position) *
        glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
    mSimpleProgram->use();
    mSimpleProgram->setUniform("color", glm::vec4(mLight.ambient + mLight.diffuse, 1.0f));
    mSimpleProgram->setUniform("transform", projection * view * lightModelTransform);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    mProgram->use();
	mProgram->setUniform("viewPos", mCamera.pos);
    mProgram->setUniform("light.position", mLight.position);
    mProgram->setUniform("light.ambient", mLight.ambient);
    mProgram->setUniform("light.diffuse", mLight.diffuse);
    mProgram->setUniform("light.specular", mLight.specular);
    mProgram->setUniform("material.diffuse", 0);
    mProgram->setUniform("material.specular", 1);
    mProgram->setUniform("material.shininess", mMaterial.shininess);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mMaterial.diffuse->get());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mMaterial.specular->get());

    if (mIsAnimationActive)
    {
        mAnimationTime = glfwGetTime();
    }
    for (size_t i = 0; i < cubePositions.size(); i++)
    {
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(
                    glm::rotate(
                        model,
                        glm::radians((float)mAnimationTime * 90.0f + 20.0f * (float)i),
                        glm::vec3(1.0f, 0.5f, 0.0f)
                        ),
                    glm::vec3(0.8f, 0.8f, 0.8f)
                    );
        auto transform = projection * view * model;
        mProgram->setUniform("transform", transform);
        mProgram->setUniform("modelTransform", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void Context::updateImGui()
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

    ImGui::Text("%.3f ms/frame (%dfps)", fps, prevFrames);
    ImGui::Spacing();
    ImGui::Spacing();


    if (ImGui::CollapsingHeader("Background", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::ColorEdit4("color", glm::value_ptr(mClearColor)))
        {
            glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);
        }
    }
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Camera");
        ImGui::SliderFloat3("Position", &mCamera.pos.x, -10.0f, 10.0f, "%.3f");
        ImGui::SliderFloat3("Target", &mCamera.target.x, -10.0f, 10.0f, "%.3f");
        ImGui::SliderFloat3("Up", &mCamera.up.x, -10.0f, 10.0f, "%.3f");
        if (ImGui::Button("Reset camera"))
        {
            mCamera.reset();
        }
    }
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragFloat3("position", glm::value_ptr(mLight.position), 0.01f);
        ImGui::ColorEdit3("l.ambient", glm::value_ptr(mLight.ambient));
        ImGui::ColorEdit3("l.diffuse", glm::value_ptr(mLight.diffuse));
        ImGui::ColorEdit3("l.specular", glm::value_ptr(mLight.specular));
        if (ImGui::ColorEdit3("l.All", glm::value_ptr(mLight.ambient)))
        {
            mLight.diffuse = mLight.ambient;
            mLight.specular = mLight.ambient;
        }
    }
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat("shininess", &mMaterial.shininess, 1.0f, 1.0f, 256.0f);
    }
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen))
    {
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
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Extras", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Checkbox("WireFrame", &mIsWireframeActive))
        {
            if (mIsWireframeActive)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        ImGui::SameLine();
        ImGui::Checkbox("Animation", &mIsAnimationActive);
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
    }
}

bool Context::init()
{
    mProgram = Program::create("shader/lighting.vs", "shader/lighting.fs");
    if (!mProgram)
    {
        return false;
    }
    SPDLOG_INFO("program id: {}", mProgram->get());

    mSimpleProgram = Program::create("shader/simple.vs", "shader/simple.fs");
    if (!mSimpleProgram)
    {
        return false;
    }
    SPDLOG_INFO("simple program id: {}", mSimpleProgram->get());

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    SPDLOG_INFO("Maximum nr of vertex attributes supported: {}", nrAttributes);

    float vertices[] =
    {
    // pos.xyz, normal.xyz, texcoord.uv
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
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
    mVAO->setAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    mVAO->setAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);
    mVAO->setAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);

    mEBO = Buffer::create(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(indices));
    if (!mEBO)
    {
        return false;
    }
    SPDLOG_INFO("EBO id : {}", mEBO->get());

    glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);

    mMaterial.diffuse = Texture::create("./image/box.png");
    mMaterial.specular = Texture::create("./image/box_spec.png");

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

void Context::updateGUIwindow(int x, int y, int width, int height)
{
    if (mGUIx != x || mGUIy != y || mGUIwidth != width || mGUIheight != height)
    {
        mIsWindowUpdated = true;
    }
    mGUIx = x;
    mGUIy = y;
    mGUIwidth = width;
    mGUIheight = height;
}

void Context::updateWindowSize(int width, int height)
{
    if (mWidth != width || mHeight != height)
    {
        mIsWindowUpdated = true;
    }
    mWidth = width;
    mHeight = height;
}