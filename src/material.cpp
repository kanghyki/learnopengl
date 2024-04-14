#include "material.hpp"

Material::Material() {}

Material::~Material() {}

std::shared_ptr<Material> Material::Create() {
  return std::shared_ptr<Material>(new Material());
}

void Material::SetToProgram(const Program *program) const {
  int textureCount = 0;

  if (diffuse_) {
    glActiveTexture(GL_TEXTURE0 + textureCount);
    program->SetUniform("material.diffuse", textureCount);
    diffuse_->Bind();
    ++textureCount;
  }
  if (specular_) {
    glActiveTexture(GL_TEXTURE0 + textureCount);
    program->SetUniform("material.specular", textureCount);
    specular_->Bind();
    ++textureCount;
  }
  program->SetUniform("material.shininess", shininess_);
}