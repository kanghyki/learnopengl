#include "context.hpp"

#include <imgui.h>

#include "image.hpp"

Context::Context() {
  clear_bit_ =
      GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
  glClearColor(clear_color_[0], clear_color_[1], clear_color_[2],
               clear_color_[3]);
}

Context::~Context() {}

std::unique_ptr<Context> Context::Create() {
  auto context = std::unique_ptr<Context>(new Context());
  if (!context->Init()) {
    return nullptr;
  }

  return std::move(context);
}

bool Context::Init() {
  framebuffer_ =
      Framebuffer::Create(Texture::CreateEmpty(width_, height_, GL_RGBA));
  if (!framebuffer_) {
    return false;
  }

  lighting_program_ =
      Program::Create("shader/lighting.vs", "shader/lighting.fs");
  if (!lighting_program_) {
    return false;
  }

  simple_program_ = Program::Create("shader/simple.vs", "shader/simple.fs");
  if (!simple_program_) {
    return false;
  }

  post_program_ = Program::Create("shader/texture.vs", "shader/gamma.fs");
  if (!post_program_) {
    return false;
  }

  cube_program_ =
      Program::Create("shader/cube_texture.vs", "shader/cube_texture.fs");
  if (!cube_program_) {
    return false;
  }

  plane_program_ = Program::Create("shader/texture.vs", "shader/texture.fs");
  if (!plane_program_) {
    return false;
  }

  env_map_program_ = Program::Create("shader/env_map.vs", "shader/env_map.fs");
  if (!env_map_program_) {
    return false;
  }
  {  // box mesh
    auto mat = Material::Create();
    mat->specular_ = Texture::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            .get());
    mat->diffuse_ = Texture::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f))
            .get());
    box_ = Mesh::CreateBox();
    box_->set_material(std::move(mat));
  }
  {  // sphere mesh
    auto mat = Material::Create();
    mat->specular_ = Texture::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            .get());
    mat->diffuse_ = Texture::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f))
            .get());
    sphere_ = Mesh::CreateSphere(35, 35);
    sphere_->set_material(std::move(mat));
  }
  {  // plane mesh
    plane_texture_ = Texture::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.4f, 0.4f, 0.4f, 0.5f))
            .get());
    plane_ = Mesh::CreatePlane();
  }
  {  // cube texture
    auto cubeRight = Image::Load("./image/cube_texture/right.jpg", false);
    auto cubeLeft = Image::Load("./image/cube_texture/left.jpg", false);
    auto cubeTop = Image::Load("./image/cube_texture/top.jpg", false);
    auto cubeBottom = Image::Load("./image/cube_texture/bottom.jpg", false);
    auto cubeFront = Image::Load("./image/cube_texture/front.jpg", false);
    auto cubeBack = Image::Load("./image/cube_texture/back.jpg", false);
    cube_texture_ = CubeTexture::Create({
        cubeRight.get(),
        cubeLeft.get(),
        cubeTop.get(),
        cubeBottom.get(),
        cubeFront.get(),
        cubeBack.get(),
    });
  }
  {  // model
    model_ = Model::Load("model/resources/teapot.obj");
    if (!model_) {
      return false;
    }
  }

  return true;
}

void Context::Update() { camera_.Move(); }

void Context::Render() {
  RenderImGui();
  framebuffer_->Bind();
  glEnable(GL_DEPTH_TEST);
  glClear(clear_bit_);
  auto projection = camera_.GetPerspectiveProjectionMatrix();
  auto view = camera_.GetViewMatrix();

  {  // cube program
    glActiveTexture(GL_TEXTURE0);
    cube_texture_->Bind();

    auto model = glm::translate(glm::mat4(1.0), camera_.position_) *
                 glm::scale(glm::mat4(1.0), glm::vec3(30.0f));
    cube_program_->Use();
    cube_program_->SetUniform("cube", 0);
    cube_program_->SetUniform("transform", projection * view * model);
    sphere_->Draw(cube_program_.get());
  }
  {  // simple program
    auto lightModelTransform = glm::translate(glm::mat4(1.0), light_.position) *
                               glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
    simple_program_->Use();
    simple_program_->SetUniform(
        "color", glm::vec4(light_.ambient + light_.diffuse, 1.0f));
    simple_program_->SetUniform("transform",
                                projection * view * lightModelTransform);
    box_->Draw(simple_program_.get());
  }
  {  // lighting program
    lighting_program_->Use();
    lighting_program_->SetUniform("lightType", light_type_);
    lighting_program_->SetUniform("viewPos", camera_.position_);
    lighting_program_->SetUniform("light.position", light_.position);
    lighting_program_->SetUniform("light.direction", light_.direction);
    lighting_program_->SetUniform(
        "light.cutoff",
        glm::vec2(cosf(glm::radians(light_.cutoff[0])),
                  cosf(glm::radians(light_.cutoff[0] + light_.cutoff[1]))));
    lighting_program_->SetUniform("light.constant", light_.constant);
    lighting_program_->SetUniform("light.linear", light_.linear);
    lighting_program_->SetUniform("light.quadratic", light_.quadratic);
    lighting_program_->SetUniform("light.ambient", light_.ambient);
    lighting_program_->SetUniform("light.diffuse", light_.diffuse);
    lighting_program_->SetUniform("light.specular", light_.specular);
    auto genModel = [this](glm::mat4 mat) -> glm::mat4 {
      return glm::scale(
          glm::rotate(
              mat,
              glm::radians((is_animation_active_ ? (float)glfwGetTime() : 0) *
                           90.0f),
              glm::vec3(1.0f, 0.5f, 0.0f)),
          glm::vec3(0.3f));
    };

    {
      auto pos = glm::vec3(-1.7f, 3.0f, -7.5f);
      auto model = genModel(glm::translate(glm::mat4(1.0f), pos));
      lighting_program_->SetUniform("transform", projection * view * model);
      lighting_program_->SetUniform("modelTransform", model);
      box_->Draw(lighting_program_.get());
    }
    {
      auto pos = glm::vec3(1.5f, 2.0f, -2.5f);
      auto model = genModel(glm::translate(glm::mat4(1.0f), pos));
      lighting_program_->SetUniform("transform", projection * view * model);
      lighting_program_->SetUniform("modelTransform", model);
      sphere_->Draw(lighting_program_.get());
    }
    {
      auto model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 1.5f, 0.0f));
      model = glm::scale(
          glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::vec3(0.3f));

      lighting_program_->Use();
      lighting_program_->SetUniform("transform", projection * view * model);
      lighting_program_->SetUniform("modelTransform", model);
      model_->Draw(lighting_program_.get());
    }
  }
  {  // env map program
    auto transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(-3.5f, 1.5f, -1.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    env_map_program_->Use();
    env_map_program_->SetUniform("model", transform);
    env_map_program_->SetUniform("view", view);
    env_map_program_->SetUniform("projection", projection);
    env_map_program_->SetUniform("cameraPos", camera_.position_);
    cube_texture_->Bind();
    env_map_program_->SetUniform("cube", 0);
    sphere_->Draw(env_map_program_.get());
  }
  {  // plane program
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    plane_texture_->Bind();

    auto model = glm::scale(glm::rotate(glm::mat4(1.0), glm::radians(90.0f),
                                        glm::vec3(1.0f, 0.0f, 0.0f)),
                            glm::vec3(10.0f));
    plane_program_->Use();
    plane_program_->SetUniform("tex", 0);
    plane_program_->SetUniform("transform", projection * view * model);
    plane_program_->SetUniform("modelTransform", model);
    plane_->Draw(plane_program_.get());
    glDisable(GL_BLEND);
  }
  Framebuffer::BindToDefault();
  glDisable(GL_DEPTH_TEST);
  glClear(clear_bit_);
  {  // post program
    auto model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    post_program_->Use();
    post_program_->SetUniform("transform", model);
    post_program_->SetUniform("gamma", gamma_);
    glActiveTexture(GL_TEXTURE0);
    framebuffer_->colorAttachment()->Bind();
    post_program_->SetUniform("tex", 0);
    plane_->Draw(post_program_.get());
  }
}

void Context::ProcessKeyboardInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera_.SetMove(kFront);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
    camera_.UnsetMove(kFront);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_.SetMove(kBack);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
    camera_.UnsetMove(kBack);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera_.SetMove(kRight);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
    camera_.UnsetMove(kRight);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera_.SetMove(kLeft);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
    camera_.UnsetMove(kLeft);
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    camera_.SetMove(kUp);
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
    camera_.UnsetMove(kUp);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    camera_.SetMove(kDown);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
    camera_.UnsetMove(kDown);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    camera_.SetMoveSpeed(0.15f);
  } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
    camera_.SetMoveSpeed(0.05f);
  }
}

void Context::ProcessMouseMove(double x, double y) {
  if (camera_direction_control_) {
    glm::vec2 curPos(x, y);
    camera_.Rotate(curPos - prev_mouse_pos_);
    prev_mouse_pos_ = curPos;
  }
}

void Context::ProcessMouseButton(int button, int action, double x, double y) {
  prev_mouse_pos_ = glm::vec2((float)x, (float)y);
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      camera_direction_control_ = true;
    } else if (action == GLFW_RELEASE) {
      camera_direction_control_ = false;
    }
  }
}

void Context::ProcessMouseScroll(double xoffset, double yoffset) {}

void Context::ReshapeViewport(int width, int height) {
  width_ = width;
  height_ = height;
  camera_.ChangeAspect(width_, height_);
  glViewport(0, 0, width_, height_);
  framebuffer_ =
      Framebuffer::Create(Texture::CreateEmpty(width_, height_, GL_RGBA));
}

void Context::RenderImGui() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Open")) {
      if (ImGui::MenuItem("Settings")) {
        is_setting_open_ = true;
      }
      if (ImGui::MenuItem("Framebuffer")) {
        is_frambuffer_open_ = true;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  if (is_setting_open_) {
    if (ImGui::Begin("Settings", &is_setting_open_,
                     ImGuiWindowFlags_AlwaysAutoResize)) {
      static float prev_time = 0;
      static int frames = 0;
      static float fps = 0.0f;
      static int prev_frames = 0;

      frames++;
      float curr_time = (float)glfwGetTime();
      if (curr_time - prev_time >= 1.0) {
        prev_frames = frames;
        fps = 1000.0f / frames;
        prev_time = curr_time;
        frames = 0;
      }

      ImGui::Text("%.3f ms/frame (%dfps)", fps, prev_frames);
      ImGui::Spacing();
      ImGui::Spacing();

      if (ImGui::CollapsingHeader("Background",
                                  ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::ColorEdit4("color", glm::value_ptr(clear_color_))) {
          glClearColor(clear_color_[0], clear_color_[1], clear_color_[2],
                       clear_color_[3]);
        }
      }
      ImGui::Spacing();
      ImGui::Spacing();
      if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Camera");
        ImGui::DragFloat3("Position", glm::value_ptr(camera_.position_), 0.01f);
        ImGui::Text("Front : x(%.3f), y(%.3f), z(%.3f)", camera_.front_.x,
                    camera_.front_.y, camera_.front_.z);
        ImGui::Text("Up    : x(%.3f), y(%.3f), z(%.3f)", camera_.up_.x,
                    camera_.up_.y, camera_.up_.z);
        if (ImGui::Button("Reset camera")) {
          camera_.Reset();
        }
        ImGui::SameLine();
      }
      ImGui::Spacing();
      ImGui::Spacing();
      if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::RadioButton("Directional", &light_type_, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Point", &light_type_, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Spot", &light_type_, 2);
        if (light_type_ == 0) {
          ImGui::DragFloat3("direction", glm::value_ptr(light_.direction),
                            0.01f);
          if (ImGui::Button("Sync the camera")) {
            light_.direction = camera_.front_;
          }
        } else if (light_type_ == 1) {
          ImGui::DragFloat3("position", glm::value_ptr(light_.position), 0.01f);
          if (ImGui::Button("Sync the camera")) {
            light_.position = camera_.position_ - (camera_.front_ * 0.2f);
          }
        } else if (light_type_ == 2) {
          ImGui::DragFloat3("direction", glm::value_ptr(light_.direction),
                            0.01f);
          ImGui::DragFloat3("position", glm::value_ptr(light_.position), 0.01f);
          ImGui::DragFloat2("cutoff", glm::value_ptr(light_.cutoff), 0.5f, 0.0f,
                            180.0f);
          if (ImGui::Button("Sync the camera")) {
            light_.position = camera_.position_ - (camera_.front_ * 0.2f);
            light_.direction = camera_.front_;
          }
        }
        ImGui::Text("All");
        ImGui::ColorEdit3("ambient", glm::value_ptr(light_.ambient));
        ImGui::ColorEdit3("diffuse", glm::value_ptr(light_.diffuse));
        ImGui::ColorEdit3("specular", glm::value_ptr(light_.specular));
      }
      ImGui::Spacing();
      ImGui::Spacing();
      if (ImGui::CollapsingHeader("Extras", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("Active wireFrame", &is_wireframe_active_)) {
          if (is_wireframe_active_) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          }
        }
        ImGui::Checkbox("Active animation", &is_animation_active_);
        ImGui::DragFloat("Gamma", &gamma_, 0.01f, 0.0f, 2.0f);
      }
    }
    ImGui::End();
  }
  if (is_frambuffer_open_) {
    if (ImGui::Begin(
            "Framebuffer", &is_frambuffer_open_,
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

      ImGui::Image(
          reinterpret_cast<ImTextureID>(framebuffer_->colorAttachment()->id()),
          ImVec2(imgui_image_size_ * ((float)width_ / (float)height_),
                 (float)imgui_image_size_),
          ImVec2(0, 1), ImVec2(1, 0));
      ImGui::InputInt("Size", &imgui_image_size_);

      if (isModalOpen) {
        ImGui::OpenPopup(title.c_str());
      }
      ImguiModal(
          title,
          "if the file already exists, it will be overwritten\n\n"
          "Are you sure?",
          [this]() -> void {
            if (!framebuffer_->colorAttachment()->SaveAsPng(
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

void Context::ImguiModal(const std::string& title, const std::string& text,
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
