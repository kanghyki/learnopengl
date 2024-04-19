#ifndef INCLUDED_BOUNDING_SPHERE_HPP
#define INCLUDED_BOUNDING_SPHERE_HPP

#include "common.hpp"
#include "ray.hpp"
#include "transform.hpp"

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
    return glm::max(glm::max(t.scale_.x, t.scale_.y), t.scale_.z) * radius_;
  }

  const float radius_;
};

#endif