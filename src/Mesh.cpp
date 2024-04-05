#include "Mesh.hpp"

Mesh::Mesh()
{}

Mesh::~Mesh()
{}

std::unique_ptr<Mesh> Mesh::create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t primitiveType)
{
    auto mesh = std::unique_ptr<Mesh>(new Mesh());
    mesh->init(vertices, indices, primitiveType);

    return std::move(mesh);
}

void Mesh::init(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t primitiveType)
{
    mVertexArray = VertexArray::create();
    mVertexBuffer = Buffer::create(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(Vertex),
        vertices.size());
    mIndexBuffer = Buffer::create(
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32_t),
        indices.size());
    mVertexArray->setAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
    mVertexArray->setAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, normal));
    mVertexArray->setAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, texCoord));
}

void Mesh::draw(const Program* program) const
{
    mVertexArray->bind();
    if (mMaterial)
    {
        mMaterial->setToProgram(program);
    }
    glDrawElements(mPrimitiveType, mIndexBuffer->getCount(), GL_UNSIGNED_INT, 0);
}

const VertexArray* Mesh::getVertexLayout() const
{
    return mVertexArray.get();
}

std::shared_ptr<Buffer> Mesh::getVertexBuffer() const
{
    return mVertexBuffer;
}

std::shared_ptr<Buffer> Mesh::getIndexBuffer() const
{
    return mIndexBuffer;
}

std::shared_ptr<Material> Mesh::getMaterial() const {
    return mMaterial;
}

void Mesh::setMaterial(std::shared_ptr<Material> material) {
    mMaterial = material;
}

std::unique_ptr<Mesh> Mesh::createBox()
{
    std::vector<Vertex> vertices = {
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },

        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },

        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },

        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },

        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
    };

    std::vector<uint32_t> indices = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    return create(vertices, indices, GL_TRIANGLES);
}

std::unique_ptr<Mesh> Mesh::createSphere(size_t slice, size_t stack)
{
    std::vector<Vertex>     vertices;
    std::vector<uint32_t>   indices;
    const float             radius = 1.0f;
    const float             dTheta = -(glm::pi<float>() * 2) / static_cast<float>(slice);
    const float             dPhi = -glm::pi<float>() / static_cast<float>(stack);

    for (size_t i = 0; i < stack + 1; ++i)
    {
        glm::vec3 yPoint = glm::vec4(0.0f, -radius, 0.0f, 1.0f) *
            glm::rotate(
                glm::mat4(1.0f),
                (dPhi * i),
                glm::vec3(0.0f, 0.0f, -1.0f)
            );
        for (size_t j = 0; j < slice + 1; ++j)
        {
            Vertex v;
            v.position = glm::vec4(yPoint, 1.0f) *
                glm::rotate(
                    glm::mat4(1.0f),
                    (dTheta * j),
                    glm::vec3(0.0f, -1.0f, 0.0f));
            v.normal = glm::normalize(v.position);
            v.texCoord = glm::vec2(static_cast<float>(j) / slice, 1.0f - static_cast<float>(i) / stack);
            vertices.push_back(v);
        }
    }

    for (size_t i = 0; i < stack; ++i)
    {
        const size_t offset = (slice + 1) * i;
        for (size_t j = 0; j < slice; ++j)
        {
            indices.push_back(offset + j);
            indices.push_back(offset + j + slice + 1);
            indices.push_back(offset + j + 1 + slice + 1);
            indices.push_back(offset + j);
            indices.push_back(offset + j + 1 + slice + 1);
            indices.push_back(offset + j + 1);
        }
    }

    return create(vertices, indices, GL_TRIANGLES);
}

std::unique_ptr<Mesh> Mesh::createPlane()
{
    std::vector<Vertex> vertices = {
        Vertex { glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
    };

    std::vector<uint32_t> indices = {
        0, 1, 3,
        1, 2, 3
    };

    return create(vertices, indices, GL_TRIANGLES);
}