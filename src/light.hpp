#ifndef INCLUDED_LIGHT_HPP
#define INCLUDED_LIGHT_HPP

#include "common.hpp"
#include "object.hpp"
#include "texture.hpp"

enum LightType {
  kDirectional,
  kPoint,
  kSpot,
};

class Light : public Object {
 public:
  static std::shared_ptr<Light> Create(std::shared_ptr<Mesh> mesh) {
    auto sptr = std::shared_ptr<Light>(new Light(mesh));

    return sptr;
  }
  ~Light(){};

  // TODO: refactor
  LightType& type() { return type_; }

  // Point & Spot
  glm::vec3 position() const { return transform().translate(); }
  const float constant{1.0f};
  const float linear{0.09f};
  const float quadratic{0.032f};

  // Directional & Spot
  glm::vec3 direction() const {
    return transform().RotateMatrix() * glm::vec4(direction_, 1.0f);
  }

  // Spot
  glm::vec2 cutoff{glm::vec2(11.0f, 5.0f)};

  // All
  glm::vec3 ambient{glm::vec3(0.3f, 0.3f, 0.3f)};
  glm::vec3 diffuse{glm::vec3(1.0f, 1.0f, 1.0f)};
  glm::vec3 specular{glm::vec3(1.0f, 1.0f, 1.0f)};

 private:
  Light(std::shared_ptr<Mesh> mesh) : Object(mesh){};

  glm::vec3 direction_{glm::vec3(0.0f, -1.0f, 0.0f)};
  LightType type_{kPoint};
};

#endif
