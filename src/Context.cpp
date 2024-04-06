#include <imgui.h>

#include "Context.hpp"
#include "Image.hpp"

Context::Context() {
  mClearBit = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
  glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);
}

Context::~Context() {}

std::unique_ptr<Context> Context::create() {
  auto context = std::unique_ptr<Context>(new Context());
  if (!context->init()) {
    return nullptr;
  }

  return std::move(context);
}

bool Context::init() {
  mFramebuffer = Framebuffer::create(Texture::create(mWidth, mHeight, GL_RGBA));
  if (!mFramebuffer) {
    return false;
  }

  mLightingProgram =
      Program::create("shader/lighting.vs", "shader/lighting.fs");
  if (!mLightingProgram) {
    return false;
  }

  mSimpleProgram = Program::create("shader/simple.vs", "shader/simple.fs");
  if (!mSimpleProgram) {
    return false;
  }

  mPostProgram = Program::create("shader/texture.vs", "shader/gamma.fs");
  if (!mPostProgram) {
    return false;
  }

  mCubeProgram =
      Program::create("shader/cube_texture.vs", "shader/cube_texture.fs");
  if (!mCubeProgram) {
    return false;
  }

  mPlaneProgram = Program::create("shader/texture.vs", "shader/texture.fs");
  if (!mPlaneProgram) {
    return false;
  }

  mEnvMapProgram = Program::create("shader/env_map.vs", "shader/env_map.fs");
  if (!mEnvMapProgram) {
    return false;
  }
  {  // box mesh
    auto mat = Material::create();
    mat->specular = Texture::create(
        Image::createSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            .get());
    mat->diffuse = Texture::create(
        Image::createSingleColorImage(4, 4, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f))
            .get());
    mBox = Mesh::createBox();
    mBox->setMaterial(std::move(mat));
  }
  {  // sphere mesh
    auto mat = Material::create();
    mat->specular = Texture::create(
        Image::createSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            .get());
    mat->diffuse = Texture::create(
        Image::createSingleColorImage(4, 4, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f))
            .get());
    mSphere = Mesh::createSphere(35, 35);
    mSphere->setMaterial(std::move(mat));
  }
  {  // plane mesh
    mPlaneTexture = Texture::create(
        Image::createSingleColorImage(4, 4, glm::vec4(0.4f, 0.4f, 0.4f, 0.5f))
            .get());
    mPlane = Mesh::createPlane();
  }
  {  // cube texture
    auto cubeRight = Image::load("./image/cube_texture/right.jpg", false);
    auto cubeLeft = Image::load("./image/cube_texture/left.jpg", false);
    auto cubeTop = Image::load("./image/cube_texture/top.jpg", false);
    auto cubeBottom = Image::load("./image/cube_texture/bottom.jpg", false);
    auto cubeFront = Image::load("./image/cube_texture/front.jpg", false);
    auto cubeBack = Image::load("./image/cube_texture/back.jpg", false);
    mCubeTexture = CubeTexture::create({
        cubeRight.get(),
        cubeLeft.get(),
        cubeTop.get(),
        cubeBottom.get(),
        cubeFront.get(),
        cubeBack.get(),
    });
  }
  {  // model
    mModel = Model::load("model/resources/teapot.obj");
    if (!mModel) {
      return false;
    }
  }

  return true;
}

void Context::update() { mCamera.move(); }

void Context::render() {
  renderImGui();
  mFramebuffer->bind();
  glEnable(GL_DEPTH_TEST);
  glClear(mClearBit);
  auto projection = mCamera.getPerspectiveProjectionMatrix();
  auto view = mCamera.getViewMatrix();

  {  // cube program
    glActiveTexture(GL_TEXTURE0);
    mCubeTexture->bind();

    auto model = glm::translate(glm::mat4(1.0), mCamera.mPos) *
                 glm::scale(glm::mat4(1.0), glm::vec3(30.0f));
    mCubeProgram->use();
    mCubeProgram->setUniform("cube", 0);
    mCubeProgram->setUniform("transform", projection * view * model);
    mSphere->draw(mCubeProgram.get());
  }
  {  // simple program
    auto lightModelTransform = glm::translate(glm::mat4(1.0), mLight.position) *
                               glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
    mSimpleProgram->use();
    mSimpleProgram->setUniform(
        "color", glm::vec4(mLight.ambient + mLight.diffuse, 1.0f));
    mSimpleProgram->setUniform("transform",
                               projection * view * lightModelTransform);
    mBox->draw(mSimpleProgram.get());
  }
  {  // lighting program
    mLightingProgram->use();
    mLightingProgram->setUniform("lightType", mLightType);
    mLightingProgram->setUniform("viewPos", mCamera.mPos);
    mLightingProgram->setUniform("light.position", mLight.position);
    mLightingProgram->setUniform("light.direction", mLight.direction);
    mLightingProgram->setUniform(
        "light.cutoff",
        glm::vec2(cosf(glm::radians(mLight.cutoff[0])),
                  cosf(glm::radians(mLight.cutoff[0] + mLight.cutoff[1]))));
    mLightingProgram->setUniform("light.constant", mLight.constant);
    mLightingProgram->setUniform("light.linear", mLight.linear);
    mLightingProgram->setUniform("light.quadratic", mLight.quadratic);
    mLightingProgram->setUniform("light.ambient", mLight.ambient);
    mLightingProgram->setUniform("light.diffuse", mLight.diffuse);
    mLightingProgram->setUniform("light.specular", mLight.specular);
    auto genModel = [this](glm::mat4 mat) -> glm::mat4 {
      return glm::scale(
          glm::rotate(
              mat,
              glm::radians((mIsAnimationActive ? (float)glfwGetTime() : 0) *
                           90.0f),
              glm::vec3(1.0f, 0.5f, 0.0f)),
          glm::vec3(0.3f));
    };

    {
      auto pos = glm::vec3(-1.7f, 3.0f, -7.5f);
      auto model = genModel(glm::translate(glm::mat4(1.0f), pos));
      mLightingProgram->setUniform("transform", projection * view * model);
      mLightingProgram->setUniform("modelTransform", model);
      mBox->draw(mLightingProgram.get());
    }
    {
      auto pos = glm::vec3(1.5f, 2.0f, -2.5f);
      auto model = genModel(glm::translate(glm::mat4(1.0f), pos));
      mLightingProgram->setUniform("transform", projection * view * model);
      mLightingProgram->setUniform("modelTransform", model);
      mSphere->draw(mLightingProgram.get());
    }
    {
      auto model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 1.5f, 0.0f));
      model = glm::scale(
          glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::vec3(0.3f));

      mLightingProgram->use();
      mLightingProgram->setUniform("transform", projection * view * model);
      mLightingProgram->setUniform("modelTransform", model);
      mModel->draw(mLightingProgram.get());
    }
  }
  {  // env map program
    auto transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(-3.5f, 1.5f, -1.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    mEnvMapProgram->use();
    mEnvMapProgram->setUniform("model", transform);
    mEnvMapProgram->setUniform("view", view);
    mEnvMapProgram->setUniform("projection", projection);
    mEnvMapProgram->setUniform("cameraPos", mCamera.mPos);
    mCubeTexture->bind();
    mEnvMapProgram->setUniform("cube", 0);
    mSphere->draw(mEnvMapProgram.get());
  }
  {  // plane program
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    mPlaneTexture->bind();

    auto model = glm::scale(glm::rotate(glm::mat4(1.0), glm::radians(90.0f),
                                        glm::vec3(1.0f, 0.0f, 0.0f)),
                            glm::vec3(10.0f));
    mPlaneProgram->use();
    mPlaneProgram->setUniform("tex", 0);
    mPlaneProgram->setUniform("transform", projection * view * model);
    mPlaneProgram->setUniform("modelTransform", model);
    mPlane->draw(mPlaneProgram.get());
    glDisable(GL_BLEND);
  }
  Framebuffer::bindToDefault();
  glDisable(GL_DEPTH_TEST);
  glClear(mClearBit);
  {  // post program
    auto model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    mPostProgram->use();
    mPostProgram->setUniform("transform", model);
    mPostProgram->setUniform("gamma", mGamma);
    glActiveTexture(GL_TEXTURE0);
    mFramebuffer->getColorAttachment()->bind();
    mPostProgram->setUniform("tex", 0);
    mPlane->draw(mPostProgram.get());
  }
}

void Context::processKeyboardInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    mCamera.setMove(FRONT);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
    mCamera.unsetMove(FRONT);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    mCamera.setMove(BACK);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
    mCamera.unsetMove(BACK);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    mCamera.setMove(RIGHT);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
    mCamera.unsetMove(RIGHT);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    mCamera.setMove(LEFT);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
    mCamera.unsetMove(LEFT);
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    mCamera.setMove(UP);
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
    mCamera.unsetMove(UP);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    mCamera.setMove(DOWN);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
    mCamera.unsetMove(DOWN);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    mCamera.setMoveSpeed(0.15f);
  } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
    mCamera.setMoveSpeed(0.05f);
  }
}

void Context::processMouseMove(double x, double y) {
  if (mCameraDirectionControl) {
    glm::vec2 curPos(x, y);
    mCamera.rotate(curPos - mPrevMousePos);
    mPrevMousePos = curPos;
  }
}

void Context::processMouseButton(int button, int action, double x, double y) {
  mPrevMousePos = glm::vec2((float)x, (float)y);
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      mCameraDirectionControl = true;
    } else if (action == GLFW_RELEASE) {
      mCameraDirectionControl = false;
    }
  }
}

void Context::processMouseScroll(double xoffset, double yoffset) {}

void Context::reshapeViewport(int width, int height) {
  mWidth = width;
  mHeight = height;
  mCamera.changeAspect(mWidth, mHeight);
  glViewport(0, 0, mWidth, mHeight);
  mFramebuffer = Framebuffer::create(Texture::create(mWidth, mHeight, GL_RGBA));
}

void Context::renderImGui() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Open")) {
      if (ImGui::MenuItem("Settings")) {
        mIsSettingOpen = true;
      }
      if (ImGui::MenuItem("Framebuffer")) {
        mIsFrambufferOpen = true;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  if (mIsSettingOpen) {
    if (ImGui::Begin("Settings", &mIsSettingOpen,
                     ImGuiWindowFlags_AlwaysAutoResize)) {
      static float prevTime = 0;
      static int frames = 0;
      static float fps = 0.0f;
      static int prevFrames = 0;

      frames++;
      float currTime = (float)glfwGetTime();
      if (currTime - prevTime >= 1.0) {
        prevFrames = frames;
        fps = 1000.0f / frames;
        prevTime = currTime;
        frames = 0;
      }

      ImGui::Text("%.3f ms/frame (%dfps)", fps, prevFrames);
      ImGui::Spacing();
      ImGui::Spacing();

      if (ImGui::CollapsingHeader("Background",
                                  ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::ColorEdit4("color", glm::value_ptr(mClearColor))) {
          glClearColor(mClearColor[0], mClearColor[1], mClearColor[2],
                       mClearColor[3]);
        }
      }
      ImGui::Spacing();
      ImGui::Spacing();
      if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Camera");
        ImGui::DragFloat3("Position", glm::value_ptr(mCamera.mPos), 0.01f);
        ImGui::Text("Front : x(%.3f), y(%.3f), z(%.3f)", mCamera.mFront.x,
                    mCamera.mFront.y, mCamera.mFront.z);
        ImGui::Text("Up    : x(%.3f), y(%.3f), z(%.3f)", mCamera.mUp.x,
                    mCamera.mUp.y, mCamera.mUp.z);
        if (ImGui::Button("Reset camera")) {
          mCamera.reset();
        }
        ImGui::SameLine();
      }
      ImGui::Spacing();
      ImGui::Spacing();
      if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::RadioButton("Directional", &mLightType, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Point", &mLightType, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Spot", &mLightType, 2);
        if (mLightType == 0) {
          ImGui::DragFloat3("direction", glm::value_ptr(mLight.direction),
                            0.01f);
          if (ImGui::Button("Sync the camera")) {
            mLight.direction = mCamera.mFront;
          }
        } else if (mLightType == 1) {
          ImGui::DragFloat3("position", glm::value_ptr(mLight.position), 0.01f);
          if (ImGui::Button("Sync the camera")) {
            mLight.position = mCamera.mPos - (mCamera.mFront * 0.2f);
          }
        } else if (mLightType == 2) {
          ImGui::DragFloat3("direction", glm::value_ptr(mLight.direction),
                            0.01f);
          ImGui::DragFloat3("position", glm::value_ptr(mLight.position), 0.01f);
          ImGui::DragFloat2("cutoff", glm::value_ptr(mLight.cutoff), 0.5f, 0.0f,
                            180.0f);
          if (ImGui::Button("Sync the camera")) {
            mLight.position = mCamera.mPos - (mCamera.mFront * 0.2f);
            mLight.direction = mCamera.mFront;
          }
        }
        ImGui::Text("All");
        ImGui::ColorEdit3("ambient", glm::value_ptr(mLight.ambient));
        ImGui::ColorEdit3("diffuse", glm::value_ptr(mLight.diffuse));
        ImGui::ColorEdit3("specular", glm::value_ptr(mLight.specular));
      }
      ImGui::Spacing();
      ImGui::Spacing();
      if (ImGui::CollapsingHeader("Extras", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("Active wireFrame", &mIsWireframeActive)) {
          if (mIsWireframeActive) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          }
        }
        ImGui::Checkbox("Active animation", &mIsAnimationActive);
        ImGui::DragFloat("Gamma", &mGamma, 0.01f, 0.0f, 2.0f);
      }
    }
    ImGui::End();
  }
  if (mIsFrambufferOpen) {
    if (ImGui::Begin(
            "Framebuffer", &mIsFrambufferOpen,
            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize)) {
      static const std::string title = "Save as PNG";
      static int isModalOpen = false;

      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          if (ImGui::MenuItem("Save as PNG", "-")) {
            isModalOpen = true;
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      ImGui::Image(reinterpret_cast<ImTextureID>(
                       mFramebuffer->getColorAttachment()->getId()),
                   ImVec2(mImGuiImageSize * ((float)mWidth / (float)mHeight),
                          (float)mImGuiImageSize),
                   ImVec2(0, 1), ImVec2(1, 0));
      ImGui::InputInt("Size", &mImGuiImageSize);

      if (isModalOpen) {
        ImGui::OpenPopup(title.c_str());
      }
      imguiModal(
          title,
          "if the file already exists, it will be overwritten\n\n"
          "Are you sure?",
          [this]() -> void {
            if (!mFramebuffer->getColorAttachment()->saveAsPng(
                    "save/framebuffer.png")) {
              SPDLOG_INFO("failed");
            }
            isModalOpen = false;
          },
          []() -> void { isModalOpen = false; });
    }
    ImGui::End();
  }

  ImGui::Render();
}

void Context::imguiModal(const std::string& title, const std::string& text,
                         std::function<void(void)> ok,
                         std::function<void(void)> cancel) {
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal(title.c_str(), NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("%s", text.c_str());
    ImGui::Separator();
    if (ImGui::Button("OK", ImVec2(50, 0))) {
      ok();
      ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(50, 0))) {
      cancel();
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}
