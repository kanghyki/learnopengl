#include "model.hpp"

Model::Model() {}

Model::~Model() {}

std::unique_ptr<Model> Model::Load(const std::string &filename) {
  auto model = std::unique_ptr<Model>(new Model());
  std::optional<std::string> data = LoadTextFile(filename);

  if (!data || !model->ParseObjToMesh(*data)) {
    return nullptr;
  }
  if (!model->ParseObjToMesh(*data)) {
    return nullptr;
  }

  return std::move(model);
}

void Model::Draw(const Program *program) const { mesh_->Draw(program); }

bool Model::ParseObjToMesh(const std::string &data) {
  std::vector<Vertex> vertexes;
  std::vector<uint32_t> indices;
  std::vector<glm::vec3> temp_vertexes;
  std::vector<glm::vec2> temp_texcoords;
  std::vector<glm::vec3> temp_normals;
  std::vector<std::string> lines = Split(data, "\n");

  for (const auto &line : lines) {
    std::vector<std::string> word = Split(line, " ");
    std::string prefix = word[0];
    word.erase(word.begin());

    if (word.size() == 0 || prefix.rfind("#", 0) == 0) {
      continue;
    } else if (prefix == "v") {
      glm::vec3 temp;
      temp.x = std::stof(word[0]);
      temp.y = std::stof(word[1]);
      temp.z = std::stof(word[2]);
      temp_vertexes.push_back(temp);
    } else if (prefix == "vt") {
      glm::vec2 temp;
      temp.x = std::stof(word[0]);
      temp.y = std::stof(word[1]);
      temp_texcoords.push_back(temp);
    } else if (prefix == "vn") {
      glm::vec3 temp;
      temp.x = std::stof(word[0]);
      temp.y = std::stof(word[1]);
      temp.z = std::stof(word[2]);
      temp_normals.push_back(temp);
    } else if (prefix == "f") {
      std::vector<VertexIndex> vi;
      for (size_t i = 0; i < word.size(); ++i) {
        std::vector<std::string> vtx = Split(word[i], "/");
        VertexIndex index = {-1, -1, -1};
        index.v += std::stoi(vtx[0]);
        if (vtx.size() == 3) {
          index.vt += std::stoi(vtx[1]);
          index.vn += std::stoi(vtx[2]);
        }
        vi.push_back(index);
      }

      std::vector<Vertex> temp_v;
      for (size_t i = 0; i < word.size(); ++i) {
        VertexIndex index = vi[i];
        glm::vec3 v = temp_vertexes[index.v];
        glm::vec3 vn =
            index.vn != -1 ? temp_normals[index.vn] : glm::vec3(0.0f);
        glm::vec2 vt =
            index.vt != -1 ? temp_texcoords[index.vt] : glm::vec2(0.0f);
        temp_v.push_back({v, vn, vt});
      }

      glm::vec3 v0 = temp_v[1].position - temp_v[0].position;
      glm::vec3 v1 = temp_v[2].position - temp_v[0].position;
      glm::vec3 n = glm::cross(v0, v1);
      for (size_t i = 0; i < vi.size(); ++i) {
        if (vi[i].vn == -1) {
          temp_v[i].normal = n;
        }
      }

      size_t before_vertex_size = vertexes.size();
      for (size_t i = 0; i < word.size() - 2; ++i) {
        indices.push_back(before_vertex_size);
        indices.push_back(before_vertex_size + i + 1);
        indices.push_back(before_vertex_size + i + 2);
      }

      vertexes.insert(vertexes.end(), temp_v.begin(), temp_v.end());
    }
  }

  mesh_ = Mesh::Create(vertexes, indices, GL_TRIANGLES);
  LoadMaterial();

  return true;
}

// TODO: load material
bool Model::LoadMaterial() {
  auto material = Material::Create();

  material->specular_ = Texture::Create(
      Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
          .get());
  material->diffuse_ = Texture::Create(
      Image::CreateSingleColorImage(4, 4, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f))
          .get());
  mesh_->set_material(material);

  return true;
}