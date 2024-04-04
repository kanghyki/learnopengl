#ifndef INCLUDED_MATERIAL_HPP
#define INCLUDED_MATERIAL_HPP

#include "common.hpp"
#include "Texture.hpp"
#include "Program.hpp"

class Material
{
    public:
        ~Material();
        static std::shared_ptr<Material> create();

        void setToProgram(const Program* program) const;

        std::unique_ptr<Texture>    diffuse     { nullptr };
        std::unique_ptr<Texture>    specular    { nullptr };
        float                       shininess   { 30.0f };

    private:
        Material();
};

#endif