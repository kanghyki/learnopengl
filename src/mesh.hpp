#ifndef INCLUDED_MESH_HPP
#define INCLUDED_MESH_HPP

#include "buffer.hpp"
#include "common.hpp"
#include "material.hpp"
#include "vertex_array.hpp"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;
};

class Mesh {
 public:
  static std::unique_ptr<Mesh> Create(const std::vector<Vertex> &vertices,
                                      const std::vector<uint32_t> &indices,
                                      uint32_t primitive_type);
  static std::unique_ptr<Mesh> CreateBox();
  static std::unique_ptr<Mesh> CreateSphere(size_t slice, size_t stack);
  static std::unique_ptr<Mesh> CreatePlane();
  ~Mesh();

  void Draw(const Program *program) const;

  inline const VertexArray *va() const { return va_.get(); }
  inline std::shared_ptr<Buffer> vb() const { return vb_; }
  inline std::shared_ptr<Buffer> ib() const { return ib_; }
  inline std::shared_ptr<Material> material() const { return material_; }
  inline void set_material(std::shared_ptr<Material> material) {
    material_ = material;
  }

 private:
  Mesh();
  Mesh(const Mesh &mesh);
  Mesh &operator=(const Mesh &mesh);

  void Init(const std::vector<Vertex> &vertices,
            const std::vector<uint32_t> &indices, uint32_t primitive_type);

  uint32_t primitive_type_{GL_TRIANGLES};
  std::unique_ptr<VertexArray> va_{nullptr};
  std::shared_ptr<Buffer> vb_{nullptr};
  std::shared_ptr<Buffer> ib_{nullptr};

  std::shared_ptr<Material> material_{nullptr};
};

struct Transform {
  glm::vec3 translate{0.0f};
  glm::vec3 scale{0.0f};
  glm::vec3 rotate{0.0f};

  glm::mat4 GetMatrix() {
    auto t = glm::translate(glm::mat4(1.0f), translate);
    auto s = glm::scale(glm::mat4(1.0f), scale);
    auto r = glm::rotate(glm::mat4(1.0f), glm::radians(rotate.z),
                         glm::vec3(0.0f, 0.0f, 1.0f)) *
             glm::rotate(glm::mat4(1.0f), glm::radians(rotate.y),
                         glm::vec3(0.0f, 1.0f, 0.0f)) *
             glm::rotate(glm::mat4(1.0f), glm::radians(rotate.x),
                         glm::vec3(1.0f, 1.0f, 1.0f));

    return t * s * r;
  }
};

class Object {
 public:
  Object() {}
  Object(const Object &obj) {
    mesh_ = obj.mesh_;
    transform_ = obj.transform_;
  }
  Object(Mesh *mesh) { mesh_ = mesh; }
  ~Object() {}

  void Draw(const Program *program) const { mesh_->Draw(program); }
  inline Transform transform() const { return transform_; }
  inline void set_transform(Transform transform) { transform_ = transform; }

 private:
  Object &operator=(const Object &obj);

  Mesh *mesh_;
  Transform transform_;
};

#endif