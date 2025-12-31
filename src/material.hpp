#ifndef INCLUDED_MATERIAL_HPP
#define INCLUDED_MATERIAL_HPP

#include "common.hpp"
#include "program.hpp"
#include "texture.hpp"

class Material {
  public:
    static std::shared_ptr<Material> Create();
    ~Material();

    void SetToProgram(const Program* program) const;

    std::unique_ptr<Texture2d> diffuse_{nullptr};
    std::unique_ptr<Texture2d> specular_{nullptr};
    float shininess_{30.0f};

  private:
    Material();
};

#endif