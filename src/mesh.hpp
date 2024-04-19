#ifndef INCLUDED_MESH_HPP
#define INCLUDED_MESH_HPP

#include "buffer.hpp"
#include "common.hpp"
#include "material.hpp"
#include "vertex_array.hpp"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coord;
  glm::vec3 tangent;
};

class Mesh {
 public:
  static std::shared_ptr<Mesh> Create(const std::vector<Vertex> &vertices,
                                      const std::vector<uint32_t> &indices,
                                      uint32_t primitive_type);
  static std::shared_ptr<Mesh> CreateBox();
  static std::shared_ptr<Mesh> CreateSphere(size_t slice, size_t stack);
  static std::shared_ptr<Mesh> CreatePlane();
  static void ComputeTangents(std::vector<Vertex> &vertices,
                              const std::vector<uint32_t> &indices);
  ~Mesh();

  void Draw(const Program *program) const;

  inline const VertexArray *vertex_array() const { return vertex_array_.get(); }
  inline std::shared_ptr<Buffer> vertex_buffer() const {
    return vertex_buffer_;
  }
  inline std::shared_ptr<Buffer> index_buffer() const { return index_buffer_; }
  inline std::shared_ptr<Material> material() const { return material_; }
  inline void set_material(std::shared_ptr<Material> material) {
    material_ = material;
  }

 private:
  Mesh(uint32_t primitive_type);
  Mesh(const Mesh &mesh);

  void Init(const std::vector<Vertex> &vertices,
            const std::vector<uint32_t> &indices);

  uint32_t primitive_type_{GL_TRIANGLES};
  std::unique_ptr<VertexArray> vertex_array_{nullptr};
  std::shared_ptr<Buffer> vertex_buffer_{nullptr};
  std::shared_ptr<Buffer> index_buffer_{nullptr};
  std::shared_ptr<Material> material_{nullptr};
};

#endif