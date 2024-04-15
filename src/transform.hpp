#ifndef INCLUDED_TRANSFORM_HPP
#define INCLUDED_TRANSFORM_HPP

#include "common.hpp"

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

#endif