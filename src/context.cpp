#include "context.hpp"

#include <imgui.h>

#include "image.hpp"

Ray Context::CalcCursorRay(glm::vec2 cursor) {
  float ndc_x = (float)cursor.x / (width_ * 0.5f) - 1.0f;
  float ndc_y = (float)cursor.y / (height_ * 0.5f) - 1.0f;

  glm::vec4 near_pos = glm::vec4(ndc_x, -ndc_y, 0.0f, 1.0f);
  glm::vec4 far_pos = glm::vec4(ndc_x, -ndc_y, 1.0f, 1.0f);

  glm::mat4 i_proj = glm::inverse(camera_.GetPerspectiveProjectionMatrix());
  glm::mat4 i_view = glm::inverse(camera_.GetViewMatrix());

  glm::vec4 view_near_temp = i_proj * near_pos;
  glm::vec4 view_near_position = view_near_temp / view_near_temp.w;
  glm::vec4 world_near_position = i_view * view_near_position;

  glm::vec4 view_far_temp = i_proj * far_pos;
  glm::vec4 view_far_position = view_far_temp / view_far_temp.w;
  glm::vec4 world_far_position = i_view * view_far_position;

  // FIXME:
  world_near_ = world_near_position;
  world_far_ = world_far_position;

  Ray ray;
  ray.position = glm::vec3(world_near_position);
  ray.direction =
      glm::normalize(glm::vec3(world_far_position - world_near_position));

  return ray;
}

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

  index_framebuffer_ =
      Framebuffer::Create(Texture::CreateEmpty(width_, height_, GL_RGBA));
  if (!index_framebuffer_) {
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

  {
    auto mat = Material::Create();
    mat->specular_ = Texture::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.0f, 0.7f, 0.0f, 1.0f))
            .get());
    mat->diffuse_ = Texture::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.1f, 0.5f, 0.1f, 1.0f))
            .get());
    box_ = Mesh::CreateBox();
    box_->set_material(std::move(mat));
  }
  {  // sphere mesh
    auto mat = Material::Create();

    mat->diffuse_ = Texture::Create(Image::Load("image/wall.jpg", true).get());
    mat->specular_ = Texture::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f))
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
  {  // model
    model_ = Model::Load("model/resources/teapot.obj");
    if (!model_) {
      return false;
    }
  }

  light_ = Light::Create(sphere_);
  light_->CreateBoundingSphere(0.5f);
  light_->transform().set_translate(glm::vec3(0.0f, 3.0f, 0.0f));
  light_->transform().set_scale(glm::vec3(0.5f));
  objects_.push_back(light_);

  for (int i = -2; i < 3; ++i) {
    for (int j = -2; j < 3; ++j) {
      for (int k = -2; k < 3; ++k) {
        auto box = Object::Create(box_);
        box->transform().set_translate(glm::vec3(0.5f) * glm::vec3(j, k, i));
        box->transform().set_scale(glm::vec3(0.3f));
        box->CreateBoundingSphere(0.7f);
        objects_.push_back(box);
      }
    }
  }

  ubo = Buffer::Create(GL_UNIFORM_BUFFER, GL_STATIC_DRAW, NULL, 153, 1);

  return true;
}

void Context::Update() { camera_.Move(); }

void Context::Render() {
  RenderImGui();
  framebuffer_->Bind();
  glEnable(GL_DEPTH_TEST);
  glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, clear_color_.a);
  glClear(clear_bit_);
  auto projection = camera_.GetPerspectiveProjectionMatrix();
  auto view = camera_.GetViewMatrix();

  {  // cube program
    glActiveTexture(GL_TEXTURE0);
    cube_texture_->Bind();

    auto model = glm::translate(glm::mat4(1.0), camera_.position_) *
                 glm::scale(glm::mat4(1.0), glm::vec3(100.0f));
    cube_program_->Use();
    cube_program_->SetUniform("cube", 0);
    cube_program_->SetUniform("transform", projection * view * model);
    sphere_->Draw(cube_program_.get());
  }
  {  // simple program
    simple_program_->Use();
    if (is_hit_) {
      auto model = glm::translate(glm::mat4(1.0), hit_point_) *
                   glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
      simple_program_->SetUniform("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
      simple_program_->SetUniform("transform", projection * view * model);
      sphere_->Draw(simple_program_.get());
    }
  }
  {  // lighting program
    lighting_program_->Use();
    lighting_program_->SetUniform("lightType", light_type_);
    lighting_program_->SetUniform("viewPos", camera_.position_);
    lighting_program_->SetUniform("light.position", light_->position());
    lighting_program_->SetUniform("light.direction", light_->direction());
    lighting_program_->SetUniform(
        "light.cutoff",
        glm::vec2(cosf(glm::radians(light_->cutoff[0])),
                  cosf(glm::radians(light_->cutoff[0] + light_->cutoff[1]))));
    lighting_program_->SetUniform("light.constant", light_->constant);
    lighting_program_->SetUniform("light.linear", light_->linear);
    lighting_program_->SetUniform("light.quadratic", light_->quadratic);
    lighting_program_->SetUniform("light.ambient", light_->ambient);
    lighting_program_->SetUniform("light.diffuse", light_->diffuse);
    lighting_program_->SetUniform("light.specular", light_->specular);
    lighting_program_->SetUniform("projection", projection);
    lighting_program_->SetUniform("view", view);

    for (const auto& object : objects_) {
      lighting_program_->SetUniform("model", object->transform().ModelMatrix());
      lighting_program_->SetUniform("isPick", pick_id_ == object->id());
      object->Draw(lighting_program_.get());
    }
  }

  index_framebuffer_->Bind();
  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(clear_bit_);
  simple_program_->Use();
  size_t index = 0;

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  for (const auto& object : objects_) {
    auto rgba = IdToRGBA(object->id());
    uint8_t r = rgba[0];
    uint8_t g = rgba[1];
    uint8_t b = rgba[2];
    uint8_t a = rgba[3];
    simple_program_->SetUniform(
        "color", glm::vec4((float)r / 255, (float)g / 255, (float)b / 255,
                           (float)a / 255));
    simple_program_->SetUniform(
        "transform", projection * view * object->transform().ModelMatrix());
    object->Draw(simple_program_.get());
  }

  if (is_wireframe_active_) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  Framebuffer::BindToDefault();
  glDisable(GL_DEPTH_TEST);
  glClear(clear_bit_);
  {
    auto model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    post_program_->Use();
    post_program_->SetUniform("transform", model);
    post_program_->SetUniform("gamma", gamma_);
    glActiveTexture(GL_TEXTURE0);
    framebuffer_->color_attachment()->Bind();
    post_program_->SetUniform("tex", 0);
    plane_->Draw(post_program_.get());
  }
}

void Context::ProcessKeyboardInput(GLFWwindow* window, int key, int action) {
  if (key == GLFW_KEY_LEFT_CONTROL) {
    switch (action) {
      case GLFW_PRESS:
        ctrl_ = true;
        break;
      case GLFW_RELEASE:
        ctrl_ = false;
        drag_ = false;
        break;
    }
  }
  if (key == GLFW_KEY_LEFT_SHIFT) {
    switch (action) {
      case GLFW_PRESS:
        camera_.SetMoveSpeed(0.15f);
        shift_ = true;
        break;
      case GLFW_RELEASE:
        camera_.SetMoveSpeed(0.05f);
        shift_ = false;
        drag_ = false;
        break;
    }
  }
  if (key == GLFW_KEY_W) {
    switch (action) {
      case GLFW_PRESS:
        camera_.SetMove(kFront);
        break;
      case GLFW_RELEASE:
        camera_.UnsetMove(kFront);
        break;
    }
  }
  if (key == GLFW_KEY_S) {
    switch (action) {
      case GLFW_PRESS:
        camera_.SetMove(kBack);
        break;
      case GLFW_RELEASE:
        camera_.UnsetMove(kBack);
        break;
    }
  }
  if (key == GLFW_KEY_D) {
    switch (action) {
      case GLFW_PRESS:
        camera_.SetMove(kRight);
        break;
      case GLFW_RELEASE:
        camera_.UnsetMove(kRight);
        break;
    }
  }
  if (key == GLFW_KEY_A) {
    switch (action) {
      case GLFW_PRESS:
        camera_.SetMove(kLeft);
        break;
      case GLFW_RELEASE:
        camera_.UnsetMove(kLeft);
        break;
    }
  }
  if (key == GLFW_KEY_E) {
    switch (action) {
      case GLFW_PRESS:
        camera_.SetMove(kUp);
        break;
      case GLFW_RELEASE:
        camera_.UnsetMove(kUp);
        break;
    }
  }
  if (key == GLFW_KEY_Q) {
    switch (action) {
      case GLFW_PRESS:
        camera_.SetMove(kDown);
        break;
      case GLFW_RELEASE:
        camera_.UnsetMove(kDown);
        break;
    }
  }
}

void Context::ProcessMouseInput(int button, int action, double x, double y) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    switch (action) {
      case GLFW_PRESS:
        camera_direction_control_ = true;
        break;
      case GLFW_RELEASE:
        camera_direction_control_ = false;
        break;
    }
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    switch (action) {
      case GLFW_PRESS: {
        index_framebuffer_->Bind();
        int height = index_framebuffer_->color_attachment()->height();
        auto pixel = index_framebuffer_->color_attachment()->GetTexPixel(
            (int)x, height - (int)y);
        size_t id = RGBAToId(pixel);
        pick_id_ = -1;
        pick_object_ = nullptr;
        for (const auto& object : objects_) {
          if (id == object->id()) {
            pick_object_ = object;
            pick_id_ = id;
            break;
          }
        }
        break;
      }
      case GLFW_RELEASE:
        drag_ = false;
        break;
    }
  }
}

void Context::ProcessMouseMove(double x, double y) {
  glm::vec2 cur_cursor{x, y};
  if (camera_direction_control_) {
    glm::vec2 delta = cur_cursor - prev_cursor_;
    camera_.Rotate(delta);
  }
  prev_cursor_ = cur_cursor;

  if (pick_object_) {
    cursor_ray_ = CalcCursorRay(cur_cursor);
    auto dist = pick_object_->Intersect(cursor_ray_, pick_object_->transform());
    if (dist) {
      is_hit_ = true;
      hit_point_ = cursor_ray_.position + cursor_ray_.direction * dist.value();
    } else {
      is_hit_ = false;
    }

    if (ctrl_) {
      if (!drag_ && is_hit_) {
        prev_ratio_ = dist.value() / glm::length(world_far_ - world_near_);
        prev_position_ =
            cursor_ray_.position + cursor_ray_.direction * dist.value();
        drag_ = true;
      }
      if (drag_) {
        glm::vec3 new_pos =
            world_near_ + prev_ratio_ * (world_far_ - world_near_);
        glm::vec3 translate(new_pos - prev_position_);
        pick_object_->transform().set_translate(
            pick_object_->transform().translate() + translate);
        prev_position_ = new_pos;
      }
    }
    if (shift_ && is_hit_) {
      glm::vec3 cur_vector =
          hit_point_ - glm::vec3(pick_object_->transform().TranslateMatrix() *
                                 glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
      if (!drag_) {
        drag_ = true;
      } else {
        glm::quat q_rotate = glm::rotation(glm::normalize(prev_vector_),
                                           glm::normalize(cur_vector));
        pick_object_->transform().set_rotate(
            q_rotate * pick_object_->transform().rotate_quat());
      }
      prev_vector_ = cur_vector;
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
  index_framebuffer_ =
      Framebuffer::Create(Texture::CreateEmpty(width_, height_, GL_RGBA));
}

void Context::RenderImGui() {
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
          ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Direction",
                      light_->direction().x, light_->direction().y,
                      light_->direction().z);
        } else if (light_type_ == 1) {
          ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Position",
                      light_->position().x, light_->position().y,
                      light_->position().z);
        } else if (light_type_ == 2) {
          ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Direction",
                      light_->direction().x, light_->direction().y,
                      light_->direction().z);
          ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Position",
                      light_->position().x, light_->position().y,
                      light_->position().z);
          ImGui::DragFloat2("cutoff", glm::value_ptr(light_->cutoff), 0.5f,
                            0.0f, 180.0f);
        }
        ImGui::Separator();
        ImGui::ColorEdit3("ambient", glm::value_ptr(light_->ambient));
        ImGui::ColorEdit3("diffuse", glm::value_ptr(light_->diffuse));
        ImGui::ColorEdit3("specular", glm::value_ptr(light_->specular));
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

      ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(
                       index_framebuffer_->color_attachment()->id())),
                   ImVec2(imgui_image_size_ * ((float)width_ / (float)height_),
                          (float)imgui_image_size_),
                   ImVec2(0, 1), ImVec2(1, 0));
      ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(
                       framebuffer_->color_attachment()->id())),
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
            if (!framebuffer_->color_attachment()->SaveAsPng(
                    "save/framebuffer.png")) {
              SPDLOG_INFO("failed");
            }
            isModalOpen = false;
          },
          []() -> void { isModalOpen = false; });
    }
    ImGui::End();
  }

  {
    if (ImGui::Begin("Object")) {
      if (pick_object_) {
        ImGui::Text("Object id : %d", pick_id_);
        std::shared_ptr<Mesh> mesh = pick_object_->mesh();
        Transform& transform = pick_object_->transform();

        ImGui::Text("Translate: x(%.3f), y(%.3f), z(%.3f)",
                    transform.translate().x, transform.translate().y,
                    transform.translate().z);
        ImGui::Text("Scale: x(%.3f), y(%.3f), z(%.3f)", transform.scale().x,
                    transform.scale().y, transform.scale().z);
        ImGui::Text("Rotate: x(%.3f), y(%.3f), z(%.3f)",
                    transform.rotate_euler().x, transform.rotate_euler().y,
                    transform.rotate_euler().z);

        if (mesh->material()->diffuse_) {
          ImGui::Text("Diffuse texture");
          ImGui::Image(
              reinterpret_cast<ImTextureID>(
                  static_cast<uintptr_t>(mesh->material()->diffuse_->id())),
              ImVec2((float)150, (float)150), ImVec2(0, 1), ImVec2(1, 0));
        }
        if (mesh->material()->specular_) {
          ImGui::Text("Specular texture");
          ImGui::Image(
              reinterpret_cast<ImTextureID>(
                  static_cast<uintptr_t>(mesh->material()->specular_->id())),
              ImVec2((float)150, (float)150), ImVec2(0, 1), ImVec2(1, 0));
        }
      }
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

size_t RGBAToId(std::array<uint8_t, 4> rgba) {
  size_t id = 0;

  id |= (rgba[0] & 0xFF);
  id |= (rgba[1] & 0xFF) << 8;
  id |= (rgba[2] & 0xFF) << 16;
  id |= (rgba[3] & 0xFF) << 24;

  return id;
}
std::array<uint8_t, 4> IdToRGBA(size_t id) {
  uint8_t rgba[4];

  rgba[0] = id & 0xFF;
  rgba[1] = (id >> 8) & 0xFF;
  rgba[2] = (id >> 16) & 0xFF;
  rgba[3] = (id >> 24) & 0xFF;

  return {rgba[0], rgba[1], rgba[2], rgba[3]};
}