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
  static std::shared_ptr<Mesh> Create(const std::vector<Vertex> &vertices,
                                      const std::vector<uint32_t> &indices,
                                      uint32_t primitive_type);
  static std::shared_ptr<Mesh> CreateBox();
  static std::shared_ptr<Mesh> CreateSphere(size_t slice, size_t stack);
  static std::shared_ptr<Mesh> CreatePlane();
  ~Mesh();

  void Draw(const Program *program) const;

  inline const VertexArray *va() const { return va_.get(); }
  inline std::shared_ptr<Buffer> vb() const { return vb_; }
  inline std::shared_ptr<Buffer> ib() const { return ib_; }
  inline std::shared_ptr<Material> material() const { return material_; }
  inline void set_material(std::shared_ptr<Material> material) {
    material_ = material;
  }

  inline glm::vec3 vertex_min() const { return vertex_min_; }
  inline glm::vec3 vertex_max() const { return vertex_max_; }

 private:
  Mesh();
  Mesh(const Mesh &mesh);

  void Init(const std::vector<Vertex> &vertices,
            const std::vector<uint32_t> &indices, uint32_t primitive_type);

  uint32_t primitive_type_{GL_TRIANGLES};
  std::unique_ptr<VertexArray> va_{nullptr};
  std::shared_ptr<Buffer> vb_{nullptr};
  std::shared_ptr<Buffer> ib_{nullptr};
  std::shared_ptr<Material> material_{nullptr};

  glm::vec3 vertex_min_{std::numeric_limits<float>::max()};
  glm::vec3 vertex_max_{std::numeric_limits<float>::min()};
};

#endif