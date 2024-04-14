#ifndef INCLUDED_OBJECT_HPP
#define INCLUDED_OBJECT_HPP

#include "common.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include "ray.hpp"

class Transform {
 public:
  Transform(){};
  ~Transform(){};

  glm::mat4 Matrix() const {
    auto t = glm::translate(glm::mat4(1.0f), translate_);
    auto s = glm::scale(glm::mat4(1.0f), scale_);
    auto r = glm::toMat4(quat_);

    return t * s * r;
  }

  void set_translate(const glm::vec3 &t) { translate_ = t; }
  void set_scale(const glm::vec3 &s) { scale_ = s; }
  void set_rotate(const glm::quat &quat) { quat_ = quat; }
  void set_rotate(const glm::vec3 &euler) {
    quat_ = glm::quat(glm::radians(euler));
  }

  glm::vec3 translate() const { return translate_; }
  glm::vec3 scale() const { return scale_; }
  glm::quat rotate_quat() const { return quat_; }
  glm::vec3 rotate_euler() const {
    return glm::degrees(glm::eulerAngles(quat_));
  }

 private:
  glm::vec3 translate_{0.0f};
  glm::vec3 scale_{1.0f};
  glm::quat quat_{1.0f, 0.0f, 0.0f, 0.0f};
};

class BoundingSphere {
 public:
  static std::unique_ptr<BoundingSphere> Create(const glm::vec3 &min,
                                                const glm::vec3 &max,
                                                float radius) {
    auto ptr =
        std::unique_ptr<BoundingSphere>(new BoundingSphere(min, max, radius));

    return std::move(ptr);
  }
  ~BoundingSphere() {}

  std::optional<float> Intersect(const Ray &ray, const Transform &t) {
    const glm::vec3 center = CalcCenter(t);
    float radius = radius_;

    const float b = 2.0f * glm::dot(ray.direction, ray.position - center);
    const float c = glm::dot(ray.position - center, ray.position - center) -
                    radius * radius;
    const float det = b * b - 4.0f * c;

    if (det >= 0.0f) {
      const float d1 = (-b - sqrt(det)) / 2.0f;
      const float d2 = (-b + sqrt(det)) / 2.0f;
      float dist = glm::min(d1, d2);
      return dist;
    }

    return {};
  }

  glm::vec3 CalcCenter(const Transform &t) {
    glm::vec3 center = (max_ + min_) / 2.0f;

    return t.Matrix() * glm::vec4(center, 1.0f);
  }

 private:
  // TODO: smallest
  BoundingSphere(const glm::vec3 &min, const glm::vec3 &max, float radius)
      : min_(min), max_(max), radius_(radius) {}

  glm::vec3 min_;
  glm::vec3 max_;
  float radius_;
};

class Object {
 public:
  static std::shared_ptr<Object> Create(std::shared_ptr<Mesh> mesh) {
    auto object = std::shared_ptr<Object>(new Object(mesh));

    return std::move(object);
  }
  ~Object(){};

  inline void Draw(const Program *p) const { mesh_->Draw(p); }
  inline Transform &transform() { return transform_; }
  inline size_t id() const { return id_; }
  inline std::shared_ptr<Mesh> mesh() const { return mesh_; }
  inline void set_material(std::shared_ptr<Material> m) {
    mesh_->set_material(std::move(m));
  }

  void CreateBoundingSphere(float radius) {
    bounding_sphere_ = BoundingSphere::Create(mesh_->vertex_min(),
                                              mesh_->vertex_max(), radius);
  }
  std::optional<float> Intersect(const Ray &ray) {
    if (!bounding_sphere_) {
      return {};
    }
    return bounding_sphere_->Intersect(ray, transform_);
  }

  glm::vec3 bounding_sphere_center() const {
    return bounding_sphere_->CalcCenter(transform_);
  }

 private:
  static size_t kId;
  Object(std::shared_ptr<Mesh> mesh) : id_(Object::kId++), mesh_(mesh) {}

  size_t id_;
  Transform transform_;
  std::shared_ptr<Mesh> mesh_;
  std::unique_ptr<BoundingSphere> bounding_sphere_{nullptr};
};

#endif