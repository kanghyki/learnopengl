#ifndef INCLUDED_MODEL_HPP
#define INCLUDED_MODEL_HPP

// clang-format off
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// clang-format on

#include "common.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "texture.hpp"

class Model {
  public:
    static std::unique_ptr<Model> Load(const std::string& filename);
    ~Model();

    void Draw(const Program* program) const;

    std::shared_ptr<Mesh> mesh(int i) {
        if (i >= 0 && i < meshes_.size()) {
            return meshes_[i];
        }
        return nullptr;
    }
    size_t meshes_count() const { return meshes_.size(); }

  private:
    Model();
    Model(const Model& model);

    bool LoadByAssimp(const std::string& filename);
    void ProcessMesh(aiMesh* ai_mesh, const aiScene* ai_scene);
    void ProcessNode(aiNode* ai_node, const aiScene* ai_scene);

    std::vector<std::shared_ptr<Mesh>> meshes_;
    std::vector<std::shared_ptr<Material>> materials_;
};

#endif