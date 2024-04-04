#include "Material.hpp"

Material::Material()
{}

Material::~Material()
{}

std::shared_ptr<Material> Material::create()
{
    return std::shared_ptr<Material>(new Material());
}

void Material::setToProgram(const Program* program) const
{
    int textureCount = 0;

    if (diffuse)
    {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->setUniform("material.diffuse", textureCount);
        diffuse->bind();
        ++textureCount;
    }
    if (specular) {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->setUniform("material.specular", textureCount);
        specular->bind();
        ++textureCount;
    }
    program->setUniform("material.shininess", shininess);
}