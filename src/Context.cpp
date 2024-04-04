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

    if (mIsCoord) {
        auto coordModelTransform = glm::scale(glm::mat4(1.0), mCamera.pos - glm::vec3(30.0f));
        mCoordProgram->use();
        mCoordProgram->setUniform("projection", projection);
        mCoordProgram->setUniform("view", view);
        mCoordProgram->setUniform("model", coordModelTransform);
        mCoord.draw();
    }

    auto lightModelTransform =
        glm::translate(glm::mat4(1.0), mLight.position) *
        glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
    mSimpleProgram->use();
    mSimpleProgram->setUniform("color", glm::vec4(mLight.ambient + mLight.diffuse, 1.0f));
    mSimpleProgram->setUniform("transform", projection * view * lightModelTransform);
    mBox->draw(mSimpleProgram.get());


    mProgram->use();
    mProgram->setUniform("lightType", mLightType);
    mProgram->setUniform("viewPos", mCamera.pos);
    mProgram->setUniform("light.position", mLight.position);
    mProgram->setUniform("light.direction", mLight.direction);
    mProgram->setUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(mLight.cutoff[0])),
        cosf(glm::radians(mLight.cutoff[0] + mLight.cutoff[1]))));
    mProgram->setUniform("light.constant", mLight.constant);
    mProgram->setUniform("light.linear", mLight.linear);
    mProgram->setUniform("light.quadratic", mLight.quadratic);
    mProgram->setUniform("light.ambient", mLight.ambient);
    mProgram->setUniform("light.diffuse", mLight.diffuse);
    mProgram->setUniform("light.specular", mLight.specular);

    if (mIsAnimationActive)
    {
        mAnimationTime = glfwGetTime();
    }
    for (size_t i = 0; i < cubePositions.size() / 2; i++)
    {
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(
                    glm::rotate(
                        model,
                        glm::radians((float)mAnimationTime * 90.0f + 20.0f * (float)i),
                        glm::vec3(1.0f, 0.5f, 0.0f)
                        ),
                    glm::vec3(0.8f)
                    );
        auto transform = projection * view * model;
        mProgram->setUniform("transform", transform);
        mProgram->setUniform("modelTransform", model);
        mBox->draw(mProgram.get());
    }
    for (size_t i = cubePositions.size() / 2; i < cubePositions.size(); i++)
    {
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(
                    glm::rotate(
                        model,
                        glm::radians((float)mAnimationTime * 90.0f + 20.0f * (float)i),
                        glm::vec3(1.0f, 0.5f, 0.0f)
                        ),
                    glm::vec3(0.3f)
                    );
        auto transform = projection * view * model;
        mProgram->setUniform("transform", transform);
        mProgram->setUniform("modelTransform", model);
        mSphere->draw(mProgram.get());
    }
    auto model2 = 
        glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 2.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0), glm::vec3(0.5f));
    mProgram->setUniform("transform", projection * view * model2);
    mProgram->setUniform("modelTransform", model2);
    mModel->draw(mProgram.get());
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
        ImGui::DragFloat3("Position", glm::value_ptr(mCamera.pos), 0.01f);
        ImGui::Text("Front : x(%.3f), y(%.3f), z(%.3f)", mCamera.front.x, mCamera.front.y, mCamera.front.z);
        ImGui::Text("Up    : x(%.3f), y(%.3f), z(%.3f)", mCamera.up.x, mCamera.up.y, mCamera.up.z);
        if (ImGui::Button("Reset camera"))
        {
            mCamera.reset();
        }
        ImGui::SameLine();
    }
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::RadioButton("Directional", &mLightType, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Point", &mLightType, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Spot", &mLightType, 2);
        if (mLightType == 0) {
            ImGui::DragFloat3("direction", glm::value_ptr(mLight.direction), 0.01f);
            if (ImGui::Button("Sync the camera"))
            {
                mLight.direction = mCamera.front;
            }
        }
        else if (mLightType == 1) {
            ImGui::DragFloat3("position", glm::value_ptr(mLight.position), 0.01f);
            if (ImGui::Button("Sync the camera"))
            {
                mLight.position = mCamera.pos - (mCamera.front * 0.2f);
            }
        }
        else if (mLightType == 2) {
            ImGui::DragFloat3("direction", glm::value_ptr(mLight.direction), 0.01f);
            ImGui::DragFloat3("position", glm::value_ptr(mLight.position), 0.01f);
            ImGui::DragFloat2("cutoff", glm::value_ptr(mLight.cutoff), 0.5f, 0.0f, 180.0f);
            if (ImGui::Button("Sync the camera"))
            {
                mLight.position = mCamera.pos - (mCamera.front * 0.2f);
                mLight.direction = mCamera.front;
            }
        }
        ImGui::Text("All");
        ImGui::ColorEdit3("ambient", glm::value_ptr(mLight.ambient));
        ImGui::ColorEdit3("diffuse", glm::value_ptr(mLight.diffuse));
        ImGui::ColorEdit3("specular", glm::value_ptr(mLight.specular));
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
        ImGui::Checkbox("Coord", &mIsCoord);
    }
}

bool Context::init()
{
    mProgram = Program::create("shader/lighting.vs", "shader/lighting.fs");
    if (!mProgram)
    {
        return false;
    }

    mSimpleProgram = Program::create("shader/simple.vs", "shader/simple.fs");
    if (!mSimpleProgram)
    {
        return false;
    }

    mCoordProgram = Program::create("shader/coord.vs", "shader/coord.fs");
    if (!mCoordProgram)
    {
        return false;
    }

    glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);

    {
        auto boxMaterial = Material::create();
        boxMaterial->specular = Texture::create(
            Image::createSingleColorImage(4, 4, glm::vec4(0.3f, 0.5f, 0.7f, 1.0f)).get());
        boxMaterial->diffuse = Texture::create(
            Image::createSingleColorImage(4, 4, glm::vec4(0.7f, 0.5f, 0.3f, 1.0f)).get());
        mBox = Mesh::createBox();
        mBox->setMaterial(boxMaterial);
    }

    {
        auto sphereMaterial = Material::create();
        sphereMaterial->specular = Texture::create(
            Image::createSingleColorImage(4, 4, glm::vec4(0.7f, 0.5f, 0.3f, 1.0f)).get());
        sphereMaterial->diffuse = Texture::create(
            Image::createSingleColorImage(4, 4, glm::vec4(0.3f, 0.5f, 0.7f, 1.0f)).get());
        mSphere = Mesh::createSphere(15, 15);
        mSphere->setMaterial(sphereMaterial);
    }

    mModel = Model::load("./model/resources/teapot.obj");
    if (!mModel)
    {
        return false;
    }

    return true;
}

void Context::processKeyboardInput(GLFWwindow* window)
{
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) mCamera.pos += cameraSpeed * mCamera.front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) mCamera.pos -= cameraSpeed * mCamera.front;
    auto cameraRight = glm::normalize(glm::cross(mCamera.up, -mCamera.front));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) mCamera.pos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) mCamera.pos -= cameraSpeed * cameraRight;
    auto cameraUp = glm::cross(-mCamera.front, cameraRight);
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
    // if (yoffset > 0)
    // {
    //     mCamera.pos += mCamera.front * (float)abs(yoffset) * 0.6f;
    // }
    // else if (yoffset < 0)
    // {
    //     mCamera.pos -= mCamera.front * (float)abs(yoffset) * 0.6f;
    // }
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
