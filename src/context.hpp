#ifndef INCLUDED_CONTEXT_HPP
#define INCLUDED_CONTEXT_HPP

#include "camera.hpp"
#include "common.hpp"
#include "framebuffer.hpp"
#include "light.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "object.hpp"
#include "program.hpp"
#include "shader.hpp"

class Context {
 public:
  static std::unique_ptr<Context> Create();
  ~Context();

  glm::vec3 CreateRay(double x, double y);
  void Update();
  void Render();
  void RenderImGui();
  void ProcessKeyboardInput(GLFWwindow* window);
  void ProcessMouseMove(double x, double y);
  void ProcessMouseButton(int button, int action, double x, double y);
  void ProcessMouseScroll(double xoffset, double yoffset);
  void ReshapeViewport(int width, int height);
  void ImguiModal(const std::string& title, const std::string& text,
                  std::function<void(void)> ok,
                  std::function<void(void)> cancel);

 private:
  Context();
  bool Init();

  uint32_t clear_bit_{0};
  int width_{WINDOW_WIDTH};
  int height_{WINDOW_HEIGHT};
  // programs
  std::unique_ptr<Program> simple_program_{nullptr};
  std::unique_ptr<Program> plane_program_{nullptr};
  std::unique_ptr<Program> env_map_program_{nullptr};
  std::unique_ptr<Program> cube_program_{nullptr};
  std::unique_ptr<Program> lighting_program_{nullptr};

  // textures
  std::unique_ptr<Texture> plane_texture_{nullptr};
  std::unique_ptr<CubeTexture> cube_texture_{nullptr};

  // Meshes
  std::unique_ptr<Mesh> red_box_{nullptr};
  std::unique_ptr<Mesh> green_box_{nullptr};
  std::unique_ptr<Mesh> sphere_{nullptr};
  std::unique_ptr<Mesh> plane_{nullptr};
  std::unique_ptr<Model> model_{nullptr};

  // objects
  std::shared_ptr<ObjectComponent> object_;
  size_t pick_object_id_{(size_t)-1};
  ObjectItem* pick_object_item_{nullptr};

  struct Light light_;

  Camera camera_;
  glm::vec2 prev_mouse_pos_{0.0f};
  bool camera_direction_control_{false};
  bool camera_fast_move_{false};

  std::unique_ptr<Framebuffer> framebuffer_{nullptr};
  std::unique_ptr<Framebuffer> index_framebuffer_{nullptr};
  std::unique_ptr<Program> post_program_{nullptr};
  float gamma_{1.0f};

  glm::vec4 clear_color_{0.3f, 0.3f, 0.3f, 1.0f};

  bool is_wireframe_active_{false};
  bool is_animation_active_{true};
  int light_type_{0};
  int imgui_image_size_{300};
  bool is_setting_open_{true};
  bool is_frambuffer_open_{true};
};

size_t RGBAToId(std::array<uint8_t, 4> rgba);
std::array<uint8_t, 4> IdToRGBA(size_t id);

#endif
