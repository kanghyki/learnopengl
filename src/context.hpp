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
#include "ray.hpp"
#include "shader.hpp"

class Context {
 public:
  static std::unique_ptr<Context> Create();
  ~Context();

  void Update();
  void Render();
  void RenderImGui();
  void ProcessKeyboardInput(GLFWwindow* window, int key, int action);
  void ProcessMouseInput(int button, int action, double x, double y);
  void ProcessMouseMove(double x, double y);
  void ProcessMouseScroll(double xoffset, double yoffset);
  void ReshapeViewport(int width, int height);
  void ImguiModal(const std::string& title, const std::string& text,
                  std::function<void(void)> ok,
                  std::function<void(void)> cancel);

  Ray CalcCursorRay(glm::vec2 cursor);

 private:
  Context();
  bool Init();

  uint32_t clear_bit_{0};
  int width_{WINDOW_WIDTH};
  int height_{WINDOW_HEIGHT};
  std::shared_ptr<Buffer> ubo{nullptr};
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
  std::shared_ptr<Mesh> box_{nullptr};
  std::shared_ptr<Mesh> sphere_{nullptr};
  std::shared_ptr<Mesh> plane_{nullptr};
  std::shared_ptr<Model> model_{nullptr};

  // objects
  std::vector<std::shared_ptr<Object>> objects_;
  size_t pick_id_{(size_t)-1};
  std::shared_ptr<Object> pick_object_{nullptr};

  Ray cursor_ray_;
  glm::vec3 world_near_;
  glm::vec3 world_far_;
  glm::vec3 prev_vector_;
  float prev_ratio_{0.0f};
  glm::vec3 prev_position_{0.0f};
  glm::vec3 hit_point_{0.0f};
  bool is_hit_{false};
  bool drag_{false};
  bool ctrl_{false};
  bool shift_{false};

  struct Light light_;

  Camera camera_;
  glm::vec2 prev_cursor_{0.0f};
  bool camera_direction_control_{false};
  bool camera_fast_move_{false};

  std::unique_ptr<Framebuffer> framebuffer_{nullptr};
  std::unique_ptr<Framebuffer> index_framebuffer_{nullptr};
  std::unique_ptr<Program> post_program_{nullptr};
  float gamma_{1.0f};

  glm::vec4 clear_color_{0.3f, 0.3f, 0.3f, 1.0f};

  bool is_wireframe_active_{false};
  int light_type_{0};
  int imgui_image_size_{300};
  bool is_setting_open_{true};
  bool is_frambuffer_open_{true};
};

size_t RGBAToId(std::array<uint8_t, 4> rgba);
std::array<uint8_t, 4> IdToRGBA(size_t id);

#endif
