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

  glm::mat4 ModelMatrix() const {
    return TranslateMatrix() * ScaleMatrix() * RotateMatrix();
  }
  glm::mat4 ScaleMatrix() const { return glm::scale(glm::mat4(1.0f), scale_); }
  glm::mat4 RotateMatrix() const { return glm::toMat4(quat_); }
  glm::mat4 TranslateMatrix() const {
    return glm::translate(glm::mat4(1.0f), translate_);
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
  static std::unique_ptr<BoundingSphere> Create(const float radius) {
    auto ptr = std::unique_ptr<BoundingSphere>(new BoundingSphere(radius));

    return std::move(ptr);
  }
  ~BoundingSphere() {}

  std::optional<float> Intersect(const Ray &ray, const Transform &t) {
    const glm::vec3 center = translated_center(t);
    const float radius = scaled_radius(t);

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

 private:
  BoundingSphere(const float radius) : radius_(radius) {}

  glm::vec3 translated_center(const Transform &t) const {
    return t.TranslateMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  }

  float scaled_radius(const Transform &t) const {
    return glm::max(glm::max(t.scale().x, t.scale().y), t.scale().z) * radius_;
  }

  const float radius_;
};

class DrawableObject {
 public:
  DrawableObject(std::shared_ptr<Mesh> mesh) : mesh_(mesh){};
  virtual ~DrawableObject(){};

  inline void Draw(const Program *p) const { mesh_->Draw(p); }
  inline std::shared_ptr<Mesh> mesh() const { return mesh_; }

 private:
  std::shared_ptr<Mesh> mesh_;
};

class TransformableObject {
 public:
  TransformableObject(){};
  virtual ~TransformableObject(){};

  inline Transform &transform() { return transform_; }
  inline const Transform &transform() const { return transform_; }

 private:
  Transform transform_;
};

class TouchableObject {
 public:
  TouchableObject() {}
  virtual ~TouchableObject() {}

  void CreateBoundingSphere(float radius) {
    bounding_sphere_ = BoundingSphere::Create(radius);
  }

  std::optional<float> Intersect(const Ray &ray, const Transform &t) {
    if (!bounding_sphere_) {
      return {};
    }
    return bounding_sphere_->Intersect(ray, t);
  }

 private:
  std::unique_ptr<BoundingSphere> bounding_sphere_{nullptr};
};

class Object : public DrawableObject,
               public TransformableObject,
               public TouchableObject {
 public:
  static std::shared_ptr<Object> Create(std::shared_ptr<Mesh> mesh) {
    auto object = std::shared_ptr<Object>(new Object(mesh));

    return std::move(object);
  }
  ~Object(){};

  inline size_t id() const { return id_; }

 protected:
  Object(std::shared_ptr<Mesh> mesh)
      : DrawableObject(mesh),
        TransformableObject(),
        TouchableObject(),
        id_(Object::kId++) {}

 private:
  static size_t kId;

  const size_t id_;
  std::unique_ptr<BoundingSphere> bounding_sphere_{nullptr};
};

#endif