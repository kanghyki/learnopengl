#include "model.hpp"

Model::Model() {}

Model::~Model() {}

std::unique_ptr<Model> Model::Load(const std::string& filename) {
    auto model = std::unique_ptr<Model>(new Model());

    if (!model->LoadByAssimp(filename)) {
        return nullptr;
    }
    return std::move(model);
}

void Model::Draw(const Program* program) const {
    for (auto& mesh : meshes_) {
        mesh->Draw(program);
    }
}

bool Model::LoadByAssimp(const std::string& filename) {
    Assimp::Importer importer;

    auto LoadTexture = [](const std::string& dirname, aiMaterial* ai_material,
                          aiTextureType ai_texture_type) -> std::unique_ptr<Texture2d> {
        if (ai_material->GetTextureCount(ai_texture_type) <= 0) {
            return nullptr;
        }
        aiString filepath;
        ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);
        auto image = Image::Load(fmt::format(dirname + "/" + filepath.C_Str()));
        if (!image) {
            return nullptr;
        }
        return Texture2d::Create(image.get());
    };
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        SPDLOG_ERROR("failed to load model: {}", filename);
        return false;
    }
    auto dirname = filename.substr(0, filename.find_last_of("/"));
    for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* scene_material = scene->mMaterials[i];
        std::shared_ptr<Material> material = Material::Create();
        material->diffuse_ = LoadTexture(dirname, scene_material, aiTextureType_DIFFUSE);
        material->specular_ = LoadTexture(dirname, scene_material, aiTextureType_SPECULAR);

        materials_.push_back(std::move(material));
    }
    ProcessNode(scene->mRootNode, scene);
    return true;
}

void Model::ProcessNode(aiNode* ai_node, const aiScene* ai_scene) {
    for (uint32_t i = 0; i < ai_node->mNumMeshes; i++) {
        auto mesh_index = ai_node->mMeshes[i];
        auto mesh = ai_scene->mMeshes[mesh_index];
        ProcessMesh(mesh, ai_scene);
    }
    for (uint32_t i = 0; i < ai_node->mNumChildren; i++) {
        ProcessNode(ai_node->mChildren[i], ai_scene);
    }
}

void Model::ProcessMesh(aiMesh* ai_mesh, const aiScene* ai_scene) {
    std::vector<Vertex> vertices(ai_mesh->mNumVertices);
    std::vector<uint32_t> indices(ai_mesh->mNumFaces * 3);

    for (uint32_t i = 0; i < ai_mesh->mNumVertices; i++) {
        Vertex& v = vertices[i];
        v.position =
            glm::vec3(ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z);
        v.normal =
            glm::vec3(ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z);
        v.tex_coord = glm::vec2(ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y);
        v.tangent = glm::vec3(0.0f);
    }
    for (uint32_t i = 0; i < ai_mesh->mNumFaces; i++) {
        indices[i * 3] = ai_mesh->mFaces[i].mIndices[0];
        indices[i * 3 + 1] = ai_mesh->mFaces[i].mIndices[1];
        indices[i * 3 + 2] = ai_mesh->mFaces[i].mIndices[2];
    }
    std::shared_ptr<Mesh> mesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
    if (ai_mesh->mMaterialIndex >= 0) {
        mesh->set_material(materials_[ai_mesh->mMaterialIndex]);
    }
    meshes_.push_back(std::move(mesh));
}