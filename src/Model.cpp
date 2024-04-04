#include "Model.hpp"

Model::Model()
{}

Model::~Model()
{}

std::unique_ptr<Model> Model::load(const std::string& filename)
{
    auto                        model = std::unique_ptr<Model>(new Model());
    std::optional<std::string>  data = loadTextFile(filename);

    if (!data || !model->parseObjToMesh(*data))
    {
        return nullptr;
    }

    return std::move(model);
}

void Model::draw() const
{
    mMeshes->draw();
}

bool Model::parseObjToMesh(const std::string& data)
{
    std::vector<Vertex>         vertexes;
    std::vector<uint32_t>       indices;
    std::vector<glm::vec3>      tempVertexes;
    std::vector<glm::vec2>      tempTexCoords;
    std::vector<glm::vec3>      tempNormals;
    std::vector<std::string>    lines = split(data, "\n");

    for (const auto& line : lines)
    {
        std::vector<std::string> word = split(line, " ");
        std::string prefix = word[0];
        word.erase(word.begin());

        if (word.size() == 0 || prefix.rfind("#", 0) == 0)
        {
            continue;
        }
        else if (prefix == "v")
        {
            glm::vec3 temp;
            temp.x = std::stof(word[0]);
            temp.y = std::stof(word[1]);
            temp.z = std::stof(word[2]);
            tempVertexes.push_back(temp);
        }
        else if (prefix == "vt")
        {
            glm::vec2 temp;
            temp.x = std::stof(word[0]);
            temp.y = std::stof(word[1]);
            tempTexCoords.push_back(temp);
        }
        else if (prefix == "vn")
        {
            glm::vec3 temp;
            temp.x = std::stof(word[0]);
            temp.y = std::stof(word[1]);
            temp.z = std::stof(word[2]);
            tempNormals.push_back(temp);
        }
        else if (prefix == "f")
        {
            std::vector<VertexIndex> vi;
            for (size_t i = 0; i < word.size(); ++i)
            {
                std::vector<std::string> vtx = split(word[i], "/");
                VertexIndex index = { -1, -1 , -1 };
                index.v += std::stoi(vtx[0]);
                if (vtx.size() == 3) {
                    index.vt += std::stoi(vtx[1]);
                    index.vn += std::stoi(vtx[2]);
                }
                vi.push_back(index);
            }

            std::vector<Vertex> tempV;
            for (size_t i = 0; i < word.size(); ++i)
            {
                VertexIndex index = vi[i];
                glm::vec3 v = tempVertexes[index.v];
                glm::vec3 vn = index.vn != -1 ? tempNormals[index.vn] : glm::vec3(0.0f);
                glm::vec2 vt = index.vt != -1 ? tempTexCoords[index.vt] : glm::vec2(0.0f);
                tempV.push_back({v, vn, vt});
            }

            glm::vec3 v0 = tempV[1].position - tempV[0].position;
            glm::vec3 v1 = tempV[2].position - tempV[0].position;
            glm::vec3 n = glm::cross(v0, v1);
            for (size_t i = 0; i < vi.size(); ++i)
            {
                if (vi[i].vn == -1)
                {
                    tempV[i].normal = n;
                }
            }

            size_t beforeVertexSize = vertexes.size();
            for (size_t i = 0; i < word.size() - 2; ++i)
            {
                indices.push_back(beforeVertexSize);
                indices.push_back(beforeVertexSize + i + 1);
                indices.push_back(beforeVertexSize + i + 2);
            }

            vertexes.insert(vertexes.end(), tempV.begin(), tempV.end());
        }
    }

    mMeshes = Mesh::create(vertexes, indices, GL_TRIANGLES);

    return true;
}