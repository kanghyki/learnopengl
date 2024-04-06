#ifndef INCLUDED_LIGHT_HPP
#define INCLUDED_LIGHT_HPP

#include "Texture.hpp"
#include "common.hpp"

struct Light {
  // Point & Spot
  glm::vec3 position{glm::vec3(1.0f, 1.0f, 1.0f)};
  const float constant{1.0f};
  const float linear{0.09f};
  const float quadratic{0.032f};

  // Directional & Spot
  glm::vec3 direction{glm::vec3(-1.0f, -1.0f, -1.0f)};

  // Spot
  glm::vec2 cutoff{glm::vec2(11.0f, 5.0f)};

  // All
  glm::vec3 ambient{glm::vec3(0.3f, 0.3f, 0.3f)};
  glm::vec3 diffuse{glm::vec3(1.0f, 1.0f, 1.0f)};
  glm::vec3 specular{glm::vec3(1.0f, 1.0f, 1.0f)};
};

#endif
