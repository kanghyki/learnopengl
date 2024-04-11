#include "mesh.hpp"

Mesh::Mesh() {}

Mesh::~Mesh() {}

std::shared_ptr<Mesh> Mesh::Create(const std::vector<Vertex> &vertices,
                                   const std::vector<uint32_t> &indices,
                                   uint32_t primitive_type) {
  auto mesh = std::shared_ptr<Mesh>(new Mesh());
  mesh->Init(vertices, indices, primitive_type);

  return std::move(mesh);
}

void Mesh::Init(const std::vector<Vertex> &vertices,
                const std::vector<uint32_t> &indices, uint32_t primitive_type) {
  for (const auto &vertex : vertices) {
    vertex_min_ = glm::min(vertex_min_, vertex.position);
    vertex_max_ = glm::max(vertex_max_, vertex.position);
  }

  va_ = VertexArray::Create();
  vb_ = Buffer::Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices.data(),
                       sizeof(Vertex), vertices.size());
  ib_ = Buffer::Create(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices.data(),
                       sizeof(uint32_t), indices.size());
  va_->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
  va_->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex),
                 offsetof(Vertex, normal));
  va_->SetAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex),
                 offsetof(Vertex, texCoord));
}

void Mesh::Draw(const Program *program) const {
  va_->Bind();
  if (material_) {
    material_->setToProgram(program);
  }
  glDrawElements(primitive_type_, ib_->count(), GL_UNSIGNED_INT, 0);
}

std::shared_ptr<Mesh> Mesh::CreateBox() {
  std::vector<Vertex> vertices = {
      Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),
             glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),
             glm::vec2(1.0f, 1.0f)},

      Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),
             glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(1.0f, 0.0f)},

      Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),
             glm::vec2(1.0f, 0.0f)},

      Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),
             glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),
             glm::vec2(0.0f, 0.0f)},

      Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f),
             glm::vec2(0.0f, 1.0f)},

      Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f),
             glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f),
             glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f),
             glm::vec2(1.0f, 1.0f)},

      Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f),
             glm::vec2(0.0f, 0.0f)},
      Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(0.0f, 1.0f)},

      Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(0.0f, 0.0f)},
  };

  std::vector<uint32_t> indices = {
      0,  2,  1,  2,  0,  3,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
      12, 14, 13, 14, 12, 15, 16, 17, 18, 18, 19, 16, 20, 22, 21, 22, 20, 23,
  };

  return Create(vertices, indices, GL_TRIANGLES);
}

std::shared_ptr<Mesh> Mesh::CreateSphere(size_t slice, size_t stack) {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  const float radius = 0.5f;
  const float d_theta = -(glm::pi<float>() * 2) / static_cast<float>(slice);
  const float d_phi = -glm::pi<float>() / static_cast<float>(stack);

  for (size_t i = 0; i < stack + 1; ++i) {
    glm::vec3 yPoint =
        glm::vec4(0.0f, -radius, 0.0f, 1.0f) *
        glm::rotate(glm::mat4(1.0f), (d_phi * i), glm::vec3(0.0f, 0.0f, -1.0f));
    for (size_t j = 0; j < slice + 1; ++j) {
      Vertex v;
      v.position =
          glm::vec4(yPoint, 1.0f) * glm::rotate(glm::mat4(1.0f), (d_theta * j),
                                                glm::vec3(0.0f, -1.0f, 0.0f));
      v.normal = glm::normalize(v.position);
      v.texCoord = glm::vec2(static_cast<float>(j) / slice,
                             1.0f - static_cast<float>(i) / stack);
      vertices.push_back(v);
    }
  }

  for (size_t i = 0; i < stack; ++i) {
    const size_t offset = (slice + 1) * i;
    for (size_t j = 0; j < slice; ++j) {
      indices.push_back(static_cast<uint32_t>(offset + j));
      indices.push_back(static_cast<uint32_t>(offset + j + slice + 1));
      indices.push_back(static_cast<uint32_t>(offset + j + slice + 2));
      indices.push_back(static_cast<uint32_t>(offset + j));
      indices.push_back(static_cast<uint32_t>(offset + j + slice + 2));
      indices.push_back(static_cast<uint32_t>(offset + j + 1));
    }
  }

  return Create(vertices, indices, GL_TRIANGLES);
}

std::shared_ptr<Mesh> Mesh::CreatePlane() {
  std::vector<Vertex> vertices = {
      Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(0.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(1.0f, 1.0f)},
      Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(1.0f, 0.0f)},
      Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
             glm::vec2(0.0f, 0.0f)},
  };

  std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};

  return Create(vertices, indices, GL_TRIANGLES);
}