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
  /* for test */
  bool Intersect(const glm::vec3 &ray_position, const glm::vec3 &ray_direction,
                 float &distance);

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
  /* for test */
  void CreateBoundingSphere(const std::vector<Vertex> &vertexes,
                            const std::vector<uint32_t> &indices);
  bool Mesh::IntersectTriangle(const glm::vec3 &v0, const glm::vec3 &v1,
                               const glm::vec3 &v2,
                               const glm::vec3 &ray_position,
                               const glm::vec3 &ray_direction,
                               glm::vec3 &hit_point, float &distance);
  inline bool IsClose(const glm::vec3 &v0, const glm::vec3 &v1,
                      float epsilon = glm::epsilon<float>()) {
    return glm::all(glm::lessThanEqual(glm::abs(v0 - v1), glm::vec3(epsilon)));
  }

  uint32_t primitive_type_{GL_TRIANGLES};
  std::unique_ptr<VertexArray> va_{nullptr};
  std::shared_ptr<Buffer> vb_{nullptr};
  std::shared_ptr<Buffer> ib_{nullptr};
  std::shared_ptr<Material> material_{nullptr};

  /* for test */
  std::vector<glm::vec3> bounding_;
  std::vector<uint32_t> bounding_index_;
};

#endif