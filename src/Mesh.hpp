#ifndef INCLUDED_MESH_HPP
#define INCLUDED_MESH_HPP

#include "common.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Material.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh
{
    public:
        ~Mesh();
        static std::unique_ptr<Mesh> create(
            const std::vector<Vertex>& vertices,
            const std::vector<uint32_t>& indices,
            uint32_t primitiveType);
        static std::unique_ptr<Mesh> createBox();
        static std::unique_ptr<Mesh> createSphere(size_t slice, size_t stack);

        void draw(const Program* program) const;

        const VertexArray*          getVertexLayout() const;
        std::shared_ptr<Buffer>     getVertexBuffer() const;
        std::shared_ptr<Buffer>     getIndexBuffer() const;
        std::shared_ptr<Material>   getMaterial() const;
        void                        setMaterial(std::shared_ptr<Material> material);


    private:
        Mesh();
        Mesh(const Mesh& mesh);
        Mesh& operator=(const Mesh& mesh);

        void init(
            const std::vector<Vertex>& vertices,
            const std::vector<uint32_t>& indices,
            uint32_t primitiveType);

        uint32_t                        mPrimitiveType { GL_TRIANGLES };
        std::unique_ptr<VertexArray>    mVertexArray;
        std::shared_ptr<Buffer>         mVertexBuffer;
        std::shared_ptr<Buffer>         mIndexBuffer;
        std::shared_ptr<Material>       mMaterial;
};

#endif