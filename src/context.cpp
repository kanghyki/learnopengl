#include "context.hpp"

#include <imgui.h>

#include "image.hpp"

void Context::CalcCursorRay(glm::vec2 cursor) {
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

  world_near_ = world_near_position;
  world_far_ = world_far_position;

  Ray ray;
  ray.position = glm::vec3(world_near_position);
  ray.direction =
      glm::normalize(glm::vec3(world_far_position - world_near_position));

  cursor_ray_ = ray;
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
      Framebuffer::Create(Texture2d::Create(width_, height_, GL_RGBA));
  if (!framebuffer_) {
    return false;
  }

  index_framebuffer_ =
      Framebuffer::Create(Texture2d::Create(width_, height_, GL_RGBA));
  if (!index_framebuffer_) {
    return false;
  }

  depth_2d_map_ = DepthMap::Create(1024, k2D);
  if (!depth_2d_map_) {
    return false;
  }

  depth_3d_map_ = DepthMap::Create(1024, k3D);
  if (!depth_3d_map_) {
    return false;
  }

  lighting_program_ = Program::Create(
      "shader/lighting.vs", "shader/lighting.fs", "shader/lighting.gs");
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

  depth_3d_program_ =
      Program::Create("shader/omni_depth_map.vs", "shader/omni_depth_map.fs",
                      "shader/omni_depth_map.gs");
  if (!depth_3d_program_) {
    return false;
  }

  normal_program_ = Program::Create("shader/normal.vs", "shader/normal.fs",
                                    "shader/normal.gs");
  if (!normal_program_) {
    return false;
  }

  {  // cube texture
    auto cubeRight = Image::Load("./image/cube_texture/right.jpg", false);
    auto cubeLeft = Image::Load("./image/cube_texture/left.jpg", false);
    auto cubeTop = Image::Load("./image/cube_texture/top.jpg", false);
    auto cubeBottom = Image::Load("./image/cube_texture/bottom.jpg", false);
    auto cubeFront = Image::Load("./image/cube_texture/front.jpg", false);
    auto cubeBack = Image::Load("./image/cube_texture/back.jpg", false);
    cube_texture_ = Texture3d::Create({
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
    mat->specular_ = Texture2d::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.0f, 0.7f, 0.0f, 1.0f))
            .get());
    mat->diffuse_ = Texture2d::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.1f, 0.5f, 0.1f, 1.0f))
            .get());
    box_ = Mesh::CreateBox();
    box_->set_material(std::move(mat));
  }
  {
    auto mat = Material::Create();
    mat->specular_ = Texture2d::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            .get());
    mat->diffuse_ = Texture2d::Create("image/wood.png");
    wood_box_ = Mesh::CreateBox();
    wood_box_->set_material(std::move(mat));
  }
  {  // sphere mesh
    auto mat = Material::Create();

    mat->diffuse_ = Texture2d::Create(Image::Load("image/1.png", true).get());
    mat->specular_ = Texture2d::Create(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f))
            .get());
    sphere_ = Mesh::CreateSphere(35, 35);
    sphere_->set_material(std::move(mat));
  }
  {  // plane mesh
    plain_plane_ = Mesh::CreatePlane();
  }
  {  // model
    model_ = Model::Load("model/resources/teapot.obj");
    if (!model_) {
      return false;
    }
  }

  glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
  light_ = Light::Create(sphere_);
  light_->CreateBoundingSphere(0.5f);
  light_->transform().set_translate(center);
  light_->transform().set_scale(glm::vec3(0.5f));
  objects_.push_back(light_);

  for (int i = -1; i < 2; ++i) {
    for (int j = -1; j < 2; ++j) {
      for (int k = -1; k < 2; ++k) {
        if (i == 0 && j == 0 && k == 0) continue;
        auto box = Object::Create(box_);
        box->transform().set_translate(
            center + glm::vec3(j * 0.5f, k * 0.5, i * 0.5) * 3.0f);
        box->transform().set_scale(glm::vec3(0.5f));
        box->CreateBoundingSphere(0.7f);
        objects_.push_back(box);
      }
    }
  }

  {
    auto top = Object::Create(wood_box_);
    top->transform().set_scale(glm::vec3(10.0f, 0.5f, 10.0f));
    top->transform().set_translate(glm::vec3(0.0f, 5.0f, 0.0f));
    top->transform().set_rotate(glm::vec3(0.0f, 0.0f, 0.0f));
    objects_.push_back(top);
    auto bottom = Object::Create(wood_box_);
    bottom->transform().set_scale(glm::vec3(10.0f, 0.5f, 10.0f));
    bottom->transform().set_translate(glm::vec3(0.0f, -5.0f, 0.0f));
    bottom->transform().set_rotate(glm::vec3(0.0f, 0.0f, 0.0f));
    objects_.push_back(bottom);

    auto front = Object::Create(wood_box_);
    front->transform().set_scale(glm::vec3(10.0f, 0.5f, 10.0f));
    front->transform().set_translate(glm::vec3(0.0f, 0.0f, 5.0f));
    front->transform().set_rotate(glm::vec3(90.0f, 0.0f, 0.0f));
    objects_.push_back(front);

    auto back = Object::Create(wood_box_);
    back->transform().set_scale(glm::vec3(10.0f, 0.5f, 10.0f));
    back->transform().set_translate(glm::vec3(0.0f, 0.0f, -5.0f));
    back->transform().set_rotate(glm::vec3(90.0f, 0.0f, 0.0f));
    objects_.push_back(back);

    auto left = Object::Create(wood_box_);
    left->transform().set_scale(glm::vec3(10.0f, 0.5f, 10.0f));
    left->transform().set_translate(glm::vec3(-5.0f, 0.0f, 0.0f));
    left->transform().set_rotate(glm::vec3(0.0f, 0.0f, 90.0f));
    objects_.push_back(left);

    auto right = Object::Create(wood_box_);
    right->transform().set_scale(glm::vec3(10.0f, 0.5f, 10.0f));
    right->transform().set_translate(glm::vec3(5.0f, 0.0f, 0.0f));
    right->transform().set_rotate(glm::vec3(0.0f, 0.0f, 90.0f));
    objects_.push_back(right);
  }

  // shader에 uniform block 연결, binding point 0번
  glUniformBlockBinding(
      simple_program_->id(),
      glGetUniformBlockIndex(simple_program_->id(), "Transform"), 0);
  glUniformBlockBinding(
      lighting_program_->id(),
      glGetUniformBlockIndex(lighting_program_->id(), "Transform"), 0);
  glUniformBlockBinding(
      cube_program_->id(),
      glGetUniformBlockIndex(cube_program_->id(), "Transform"), 0);

  ubo_transform_ = Buffer::Create(GL_UNIFORM_BUFFER, GL_STATIC_DRAW, NULL,
                                  sizeof(glm::mat4), 2);
  // ubo를 bingding point 0번
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_transform_->id());

  return true;
}

void Context::Update() { camera_.Move(); }

void Context::Render() {
  RenderImGui();

  {  // depth_map 2d
    auto rm = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
                          glm::vec3(-1.0f, 0.0f, 0.0f));
    auto lightView = glm::lookAt(
        light_->position(), light_->position() + light_->direction(),
        glm::vec3(glm::vec4(light_->direction(), 0.0f) * rm));
    glm::mat4 lightProjection;
    if (light_->type() == kDirectional) {
      lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
    } else {
      lightProjection = glm::perspective(
          glm::radians((light_->cutoff[0] + light_->cutoff[1]) * 2.0f), 1.0f,
          1.0f, 20.0f);
    }
    ubo_transform_->Bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                    glm::value_ptr(lightView));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                    glm::value_ptr(lightProjection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    {
      depth_2d_map_->Bind();
      glEnable(GL_DEPTH_TEST);
      glClear(GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, depth_2d_map_->depth_map()->width(),
                 depth_2d_map_->depth_map()->height());
      simple_program_->Use();
      simple_program_->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

      for (const auto& object : objects_) {
        simple_program_->SetUniform("model", object->transform().ModelMatrix());
        object->Draw(simple_program_.get());
      }
    }
    {
      depth_3d_map_->Bind();
      glEnable(GL_DEPTH_TEST);
      glClear(GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, depth_3d_map_->depth_map_3d()->width(),
                 depth_3d_map_->depth_map_3d()->height());

      float aspect = (float)depth_3d_map_->depth_map_3d()->width() /
                     (float)depth_3d_map_->depth_map_3d()->height();
      glm::mat4 shadowProj =
          glm::perspective(glm::radians(90.0f), aspect, 0.5f, 25.0f);
      std::vector<glm::mat4> shadowTransforms;
      shadowTransforms.push_back(
          shadowProj *
          glm::lookAt(light_->position(),
                      light_->position() + glm::vec3(1.0, 0.0, 0.0),
                      glm::vec3(0.0, -1.0, 0.0)));
      shadowTransforms.push_back(
          shadowProj *
          glm::lookAt(light_->position(),
                      light_->position() + glm::vec3(-1.0, 0.0, 0.0),
                      glm::vec3(0.0, -1.0, 0.0)));
      shadowTransforms.push_back(
          shadowProj *
          glm::lookAt(light_->position(),
                      light_->position() + glm::vec3(0.0, 1.0, 0.0),
                      glm::vec3(0.0, 0.0, 1.0)));
      shadowTransforms.push_back(
          shadowProj *
          glm::lookAt(light_->position(),
                      light_->position() + glm::vec3(0.0, -1.0, 0.0),
                      glm::vec3(0.0, 0.0, -1.0)));
      shadowTransforms.push_back(
          shadowProj *
          glm::lookAt(light_->position(),
                      light_->position() + glm::vec3(0.0, 0.0, 1.0),
                      glm::vec3(0.0, -1.0, 0.0)));
      shadowTransforms.push_back(
          shadowProj *
          glm::lookAt(light_->position(),
                      light_->position() + glm::vec3(0.0, 0.0, -1.0),
                      glm::vec3(0.0, -1.0, 0.0)));

      depth_3d_program_->Use();
      depth_3d_program_->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
      depth_3d_program_->SetUniform("shadowMatrices", shadowTransforms);
      depth_3d_program_->SetUniform("far_plane", 25.0f);
      depth_3d_program_->SetUniform("lightPos", light_->position());

      for (const auto& object : objects_) {
        depth_3d_program_->SetUniform("model",
                                      object->transform().ModelMatrix());
        object->Draw(depth_3d_program_.get());
      }
    }

    Framebuffer::BindToDefault();
    glViewport(0, 0, width_, height_);
  }

  framebuffer_->Bind();
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, clear_color_.a);
  glClear(clear_bit_);
  auto projection = camera_.GetPerspectiveProjectionMatrix();
  auto view = camera_.GetViewMatrix();

  // copy
  ubo_transform_->Bind();
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(view));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                  glm::value_ptr(projection));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  {  // cube program
    glActiveTexture(GL_TEXTURE0);
    cube_texture_->Bind();

    auto model = glm::translate(glm::mat4(1.0), camera_.position_) *
                 glm::scale(glm::mat4(1.0), glm::vec3(100.0f));
    cube_program_->Use();
    cube_program_->SetUniform("cube", 0);
    cube_program_->SetUniform("model", model);
    sphere_->Draw(cube_program_.get());
    glActiveTexture(GL_TEXTURE0);
    // depth_map_3d_->depth_map_3d()->Bind();

    // cube_program_->Use();
    // auto model2 =
    //     glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 2.0f, 0.0f)) *
    //     glm::scale(glm::mat4(1.0), glm::vec3(1.0f));
    // cube_program_->SetUniform("cube", 0);
    // cube_program_->SetUniform("model", model2);
    // box_->Draw(cube_program_.get());
  }
  {  // simple program
    simple_program_->Use();
    if (is_hit_) {
      auto model = glm::translate(glm::mat4(1.0), hit_point_) *
                   glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
      simple_program_->SetUniform("color", glm::vec4(0.2f, 0.3f, 0.4f, 1.0f));
      simple_program_->SetUniform("model", model);
      sphere_->Draw(simple_program_.get());
    }
  }
  {  // lighting program
    lighting_program_->Use();
    lighting_program_->SetUniform("lightType", light_->type());
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
    lighting_program_->SetUniform("isBlinn", is_blinn_);
    glActiveTexture(GL_TEXTURE3);
    depth_2d_map_->depth_map()->Bind();
    lighting_program_->SetUniform("depthMap", 3);
    auto rm = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
                          glm::vec3(-1.0f, 0.0f, 0.0f));
    auto lightView = glm::lookAt(
        light_->position(), light_->position() + light_->direction(),
        glm::vec3(glm::vec4(light_->direction(), 0.0f) * rm));
    glm::mat4 lightProjection;
    if (light_->type() == kDirectional) {
      lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
    } else {
      lightProjection = glm::perspective(
          glm::radians((light_->cutoff[0] + light_->cutoff[1]) * 2.0f), 1.0f,
          1.0f, 20.0f);
    }
    lighting_program_->SetUniform("lightTransform",
                                  lightProjection * lightView);
    glActiveTexture(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE4);
    depth_3d_map_->depth_map_3d()->Bind();
    lighting_program_->SetUniform("depthMap3d", 4);
    glActiveTexture(GL_TEXTURE0);
    lighting_program_->SetUniform("far_plane", 25.0f);

    for (const auto& object : objects_) {
      lighting_program_->SetUniform("model", object->transform().ModelMatrix());
      lighting_program_->SetUniform("isPick", pick_id_ == object->id());
      object->Draw(lighting_program_.get());
    }

    normal_program_->Use();
    for (const auto& object : objects_) {
      normal_program_->SetUniform("length", 0.1f);
      normal_program_->SetUniform(
          "transform", projection * view * object->transform().ModelMatrix());
      object->Draw(normal_program_.get());
    }
  }

  index_framebuffer_->Bind();
  glDisable(GL_BLEND);
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

    simple_program_->SetUniform("model", object->transform().ModelMatrix());
    object->Draw(simple_program_.get());
  }

  Framebuffer::BindToDefault();
  glDisable(GL_BLEND);
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
    plain_plane_->Draw(post_program_.get());
  }

  if (is_wireframe_active_) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
        break;
    }
  }
  if (key == GLFW_KEY_LEFT_SHIFT) {
    switch (action) {
      case GLFW_PRESS:
        camera_.SetMove(kDown);
        break;
      case GLFW_RELEASE:
        camera_.UnsetMove(kDown);
        break;
    }
  }
  if (key == GLFW_KEY_SPACE) {
    switch (action) {
      case GLFW_PRESS:
        camera_.SetMove(kUp);
        break;
      case GLFW_RELEASE:
        camera_.UnsetMove(kUp);
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
}

ObjectType GetObjectType(std::shared_ptr<Object> p) {
  std::shared_ptr<Light> light = std::dynamic_pointer_cast<Light>(p);

  if (light) return kLight;
  return kNormal;
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
        left_mouse_ = true;
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
            object_type_ = GetObjectType(object);
            break;
          }
        }
        break;
      }
      case GLFW_RELEASE:
        left_mouse_ = false;
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
    CalcCursorRay(cur_cursor);
    auto dist = pick_object_->Intersect(cursor_ray_, pick_object_->transform());
    if (dist) {
      is_hit_ = true;
      hit_point_ = cursor_ray_.position + cursor_ray_.direction * dist.value();
    } else {
      is_hit_ = false;
    }

    if (left_mouse_ && ctrl_) {
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
    } else if (left_mouse_ && is_hit_) {
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
      Framebuffer::Create(Texture2d::Create(width_, height_, GL_RGBA));
  index_framebuffer_ =
      Framebuffer::Create(Texture2d::Create(width_, height_, GL_RGBA));
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
        ImGui::RadioButton("Directional", (int*)&light_->type(), kDirectional);
        ImGui::SameLine();
        ImGui::RadioButton("Point", (int*)&light_->type(), kPoint);
        ImGui::SameLine();
        ImGui::RadioButton("Spot", (int*)&light_->type(), kSpot);
        ImGui::Checkbox("Blinn", &is_blinn_);
        if (light_->type() == kDirectional) {
          ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Direction",
                      light_->direction().x, light_->direction().y,
                      light_->direction().z);
        } else if (light_->type() == kPoint) {
          ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Position",
                      light_->position().x, light_->position().y,
                      light_->position().z);
        } else if (light_->type() == kSpot) {
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

  if (ImGui::Begin("Index framebuffer", NULL)) {
    auto window_size = ImGui::GetWindowSize();

    ImGui::Image(
        reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(
            index_framebuffer_->color_attachment()->id())),
        ImVec2(window_size.x, window_size.x * ((float)height_ / (float)width_)),
        ImVec2(0, 1), ImVec2(1, 0));

    static char buf[512] = "index_framebuffer";
    ImGui::Text("Save as png");
    ImGui::SameLine();
    ImGui::InputText("", buf, 512 - 1);
    ImGui::SameLine();
    if (ImGui::Button("OK", ImVec2(50, 0))) {
      index_framebuffer_->color_attachment()->SaveAsPng(
          std::string("save/") + std::string(buf) + std::string(".png"));
    }
  }
  ImGui::End();

  if (ImGui::Begin("Framebuffer", NULL)) {
    auto window_size = ImGui::GetWindowSize();
    ImGui::Image(
        reinterpret_cast<ImTextureID>(
            static_cast<uintptr_t>(framebuffer_->color_attachment()->id())),
        ImVec2(window_size.x, window_size.x * ((float)height_ / (float)width_)),
        ImVec2(0, 1), ImVec2(1, 0));

    static char buf[512] = "framebuffer";
    ImGui::Text("Save as png");
    ImGui::SameLine();
    ImGui::InputText("", buf, 512 - 1);
    ImGui::SameLine();
    if (ImGui::Button("OK", ImVec2(50, 0))) {
      framebuffer_->color_attachment()->SaveAsPng(
          std::string("save/") + std::string(buf) + std::string(".png"));
    }
  }
  ImGui::End();

  if (ImGui::Begin("Depth map", NULL)) {
    auto window_size = ImGui::GetWindowSize();
    ImGui::Image(
        reinterpret_cast<ImTextureID>(
            static_cast<uintptr_t>(depth_2d_map_->depth_map()->id())),
        ImVec2(window_size.x, window_size.x * ((float)height_ / (float)width_)),
        ImVec2(0, 1), ImVec2(1, 0));

    static char buf[512] = "depth_map";
    ImGui::Text("Save as png");
    ImGui::SameLine();
    ImGui::InputText("", buf, 512 - 1);
    ImGui::SameLine();
    if (ImGui::Button("OK", ImVec2(50, 0))) {
      framebuffer_->color_attachment()->SaveAsPng(
          std::string("save/") + std::string(buf) + std::string(".png"));
    }
  }
  ImGui::End();

  {
    if (ImGui::Begin("Object")) {
      if (pick_object_) {
        switch (object_type_) {
          case kNormal:
            ImGui::Text("Object type: Normal");
            break;
          case kLight:
            ImGui::Text("Object type: Light");
            break;
        }
        ImGui::Text("Object id : %d", pick_id_);
        std::shared_ptr<Mesh> mesh = pick_object_->mesh();
        Transform& transform = pick_object_->transform();

        if (ImGui::CollapsingHeader("Transform",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("Translate: x(%.3f), y(%.3f), z(%.3f)",
                      transform.translate().x, transform.translate().y,
                      transform.translate().z);
          ImGui::Text("Scale: x(%.3f), y(%.3f), z(%.3f)", transform.scale().x,
                      transform.scale().y, transform.scale().z);
          ImGui::Text("Rotate: x(%.3f), y(%.3f), z(%.3f)",
                      transform.rotate_euler().x, transform.rotate_euler().y,
                      transform.rotate_euler().z);
        }

        if (ImGui::CollapsingHeader("Material",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          if (mesh->material()) {
            if (mesh->material()->diffuse_) {
              ImGui::Text("Diffuse texture");
              ImGui::Image(
                  reinterpret_cast<ImTextureID>(
                      static_cast<uintptr_t>(mesh->material()->diffuse_->id())),
                  ImVec2((float)150, (float)150), ImVec2(0, 1), ImVec2(1, 0));
            }
            if (mesh->material()->specular_) {
              ImGui::Text("Specular texture");
              ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(
                               mesh->material()->specular_->id())),
                           ImVec2((float)150, (float)150), ImVec2(0, 1),
                           ImVec2(1, 0));
            }
            ImGui::DragFloat("shininess", &mesh->material()->shininess_, 0.01f);
          }
        }

        if (object_type_ == kLight) {
          if (ImGui::CollapsingHeader("Light",
                                      ImGuiTreeNodeFlags_DefaultOpen)) {
            std::shared_ptr<Light> light =
                std::dynamic_pointer_cast<Light>(pick_object_);
            switch (light->type()) {
              case kDirectional:
                ImGui::Text("Light type : Directional");
                ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Direction",
                            light_->direction().x, light_->direction().y,
                            light_->direction().z);
                break;
              case kPoint:
                ImGui::Text("Light type : Point");
                ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Position",
                            light_->position().x, light_->position().y,
                            light_->position().z);
                break;
              case kSpot:
                ImGui::Text("Light type : Spot");
                ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Direction",
                            light_->direction().x, light_->direction().y,
                            light_->direction().z);
                ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Position",
                            light_->position().x, light_->position().y,
                            light_->position().z);
                ImGui::Text("%10s : x(%.3f), y(%.3f)", "Cut off",
                            light_->cutoff.x, light_->cutoff.y);
                break;
            }
            ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "Ambient",
                        light->ambient.x, light->ambient.y, light->ambient.z);
            ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "diffuse",
                        light->diffuse.x, light->diffuse.y, light->diffuse.z);
            ImGui::Text("%10s : x(%.3f), y(%.3f), z(%.3f)", "specular",
                        light->specular.x, light->specular.y,
                        light->specular.z);
          }
        }
      }
    }
    ImGui::End();
  }

  ImGui::Render();
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