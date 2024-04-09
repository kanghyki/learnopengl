#include "mesh.hpp"

Mesh::Mesh() {}

Mesh::~Mesh() {}

std::unique_ptr<Mesh> Mesh::Create(const std::vector<Vertex> &vertices,
                                   const std::vector<uint32_t> &indices,
                                   uint32_t primitive_type) {
  auto mesh = std::unique_ptr<Mesh>(new Mesh());
  mesh->Init(vertices, indices, primitive_type);

  return std::move(mesh);
}

void Mesh::Init(const std::vector<Vertex> &vertices,
                const std::vector<uint32_t> &indices, uint32_t primitive_type) {
  /* for test */
  CreateBoundingSphere(vertices, indices);

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

std::unique_ptr<Mesh> Mesh::CreateBox() {
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

std::unique_ptr<Mesh> Mesh::CreateSphere(size_t slice, size_t stack) {
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

std::unique_ptr<Mesh> Mesh::CreatePlane() {
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

void Mesh::CreateBoundingSphere(const std::vector<Vertex> &vertexes,
                                const std::vector<uint32_t> &indices) {
  bounding_.resize(vertexes.size());
  bounding_index_.resize(indices.size());
  for (size_t i = 0; i < vertexes.size(); ++i) {
    bounding_[i] = vertexes[i].position;
  }
  for (size_t i = 0; i < indices.size(); ++i) {
    bounding_index_[i] = indices[i];
  }
}

bool Mesh::Intersect(const glm::vec3 &ray_position,
                     const glm::vec3 &ray_direction, float &distance) {
  glm::vec3 hit_point(0.0f);
  float temp_distance = 0.0f;
  for (size_t i = 0; i < bounding_index_.size(); i += 3) {
    const glm::vec3 &v0 = bounding_[bounding_index_[i]];
    const glm::vec3 &v1 = bounding_[bounding_index_[i + 1]];
    const glm::vec3 &v2 = bounding_[bounding_index_[i + 2]];

    if (IntersectTriangle(v0, v1, v2, ray_position, ray_direction, hit_point,
                          temp_distance)) {
      distance = temp_distance;
      return true;
    }
  }
  return false;
}

bool Mesh::IntersectTriangle(const glm::vec3 &v0, const glm::vec3 &v1,
                             const glm::vec3 &v2, const glm::vec3 &ray_position,
                             const glm::vec3 &ray_direction,
                             glm::vec3 &hit_point, float &distance) {
  if (IsClose(v1 - v0, glm::vec3(0.0f), 1e-3f) ||
      IsClose(v2 - v0, glm::vec3(0.0f), 1e-3f)) {
    return false;
  }
  glm::vec3 nface = glm::normalize(glm::cross(v1 - v0, v2 - v0));

  if (glm::dot(-ray_direction, nface) < 0.0f ||
      glm::abs(glm::dot(ray_direction, nface)) < 1e-3f) {
    return false;
  }

  distance = (glm::dot(v0, nface) - glm::dot(ray_position, nface)) /
             (glm::dot(ray_direction, nface));
  if (distance < 0.0f) return false;
  hit_point = ray_position + distance * ray_direction;

  const glm::vec3 n0 = glm::cross(v1 - v0, hit_point - v0);
  const glm::vec3 n1 = glm::cross(hit_point - v0, v2 - v0);
  const glm::vec3 n2 = glm::cross(hit_point - v2, v1 - v2);
  if (glm::dot(n0, nface) < 0.0f || glm::dot(n1, nface) < 0.0f ||
      glm::dot(n2, nface) < 0.0f) {
    return false;
  }

  return true;
}