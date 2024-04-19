#include "mesh.hpp"

Mesh::Mesh(uint32_t primitive_type) : primitive_type_(primitive_type) {}

Mesh::~Mesh() {}

std::shared_ptr<Mesh> Mesh::Create(const std::vector<Vertex>& vertices,
                                   const std::vector<uint32_t>& indices,
                                   uint32_t primitive_type) {
  auto mesh = std::shared_ptr<Mesh>(new Mesh(primitive_type));
  mesh->Init(vertices, indices);

  return std::move(mesh);
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
      v.tex_coord = glm::vec2(static_cast<float>(j) / slice,
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

void Mesh::ComputeTangents(std::vector<Vertex>& vertices,
                           const std::vector<uint32_t>& indices) {
  auto compute = [](const glm::vec3& pos1, const glm::vec3& pos2,
                    const glm::vec3& pos3, const glm::vec2& uv1,
                    const glm::vec2& uv2, const glm::vec2& uv3) -> glm::vec3 {
    auto edge1 = pos2 - pos1;
    auto edge2 = pos3 - pos1;
    auto deltaUV1 = uv2 - uv1;
    auto deltaUV2 = uv3 - uv1;
    float det = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    if (det != 0.0f) {
      auto invDet = 1.0f / det;
      return deltaUV2.y * edge1 - deltaUV1.y * edge2;
    } else {
      return glm::vec3(0.0f, 0.0f, 0.0f);
    }
  };

  std::vector<glm::vec3> tangents(vertices.size());
  memset(tangents.data(), 0, tangents.size() * sizeof(glm::vec3));

  for (size_t i = 0; i < indices.size(); i += 3) {
    auto i0 = indices[i];
    auto i1 = indices[i + 1];
    auto i2 = indices[i + 2];

    tangents[i0] += compute(vertices[i0].position, vertices[i1].position,
                            vertices[i2].position, vertices[i0].tex_coord,
                            vertices[i1].tex_coord, vertices[i2].tex_coord);

    tangents[i1] = compute(vertices[i1].position, vertices[i2].position,
                           vertices[i0].position, vertices[i1].tex_coord,
                           vertices[i2].tex_coord, vertices[i0].tex_coord);

    tangents[i2] = compute(vertices[i2].position, vertices[i0].position,
                           vertices[i1].position, vertices[i2].tex_coord,
                           vertices[i0].tex_coord, vertices[i1].tex_coord);
  }

  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i].tangent = glm::normalize(tangents[i]);
  }
}

void Mesh::Init(const std::vector<Vertex>& vertices,
                const std::vector<uint32_t>& indices) {
  if (primitive_type_ == GL_TRIANGLES) {
    ComputeTangents(const_cast<std::vector<Vertex>&>(vertices), indices);
  }
  vertex_array_ = VertexArray::Create();
  vertex_buffer_ =
      Buffer::Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices.data(),
                     sizeof(Vertex), vertices.size());
  index_buffer_ =
      Buffer::Create(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices.data(),
                     sizeof(uint32_t), indices.size());
  vertex_array_->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
  vertex_array_->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex),
                           offsetof(Vertex, normal));
  vertex_array_->SetAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex),
                           offsetof(Vertex, tex_coord));
  vertex_array_->SetAttrib(3, 3, GL_FLOAT, false, sizeof(Vertex),
                           offsetof(Vertex, tangent));
}

void Mesh::Draw(const Program* program) const {
  vertex_array_->Bind();
  if (material_) {
    material_->SetToProgram(program);
  }
  glDrawElements(primitive_type_, index_buffer_->count(), GL_UNSIGNED_INT, 0);
}
