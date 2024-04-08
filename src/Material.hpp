#ifndef INCLUDED_MATERIAL_HPP
#define INCLUDED_MATERIAL_HPP

#include "Program.hpp"
#include "Texture.hpp"
#include "common.hpp"

class Material {
 public:
  static std::shared_ptr<Material> Create();
  ~Material();

  void setToProgram(const Program *program) const;

  std::unique_ptr<Texture> diffuse_{nullptr};
  std::unique_ptr<Texture> specular_{nullptr};
  float shininess_{30.0f};

 private:
  Material();
};

#endif