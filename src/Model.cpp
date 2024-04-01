#include "Model.hpp"

Model::Model()
{}

Model::~Model()
{}

std::unique_ptr<Model> Model::load(const std::string& filename)
{
    auto model = std::unique_ptr<Model>(new Model());
    std::optional<std::string> data = loadTextFile(filename);
    if (!data)
    {
        return nullptr;
    }
    model->parseObjFormat(*data);

    return std::move(model);
}

void Model::draw() const
{
    mMeshes->draw();
}

bool Model::parseObjFormat(const std::string& data)
{
    std::vector<glm::vec3>   tempVertexes;
    std::vector<glm::vec2>   tempTexCoords;
    std::vector<glm::vec3>   tempNormals;

    std::vector<std::string> lines = split(data, "\n");

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
            glm::vec3 tempvv;
            tempvv.x = std::stof(word[0]);
            tempvv.y = std::stof(word[1]);
            tempvv.z = std::stof(word[2]);
            tempVertexes.push_back(tempvv);
        }
        else if (prefix == "vt")
        {
            glm::vec2 tempvv;
            tempvv.x = std::stof(word[0]);
            tempvv.y = std::stof(word[1]);
            tempTexCoords.push_back(tempvv);
        }
        else if (prefix == "vn")
        {
            glm::vec3 tempvv;
            tempvv.x = std::stof(word[0]);
            tempvv.y = std::stof(word[1]);
            tempvv.z = std::stof(word[2]);
            tempNormals.push_back(tempvv);
        }
        else if (prefix == "f")
        {
            std::vector<VertexIndex> vi;

            for (size_t i = 0; i < word.size(); ++i)
            {
                std::vector<std::string> vtx = split(word[i], "/");
                int vIndex = std::stoi(vtx[0]) - 1;
                int vtIndex = -1;
                int vnIndex = -1;
                if (vtx.size() == 3) {
                    vtIndex += std::stoi(vtx[1]);
                    vnIndex += std::stoi(vtx[2]);
                }
                vi.push_back({vIndex, vtIndex, vnIndex});
            }

            size_t beforeVertexSize = mVertexes.size();
            for (size_t i = 0; i < word.size(); ++i)
            {
                VertexIndex index = vi[i];
                glm::vec3 v = tempVertexes[index.v];
                glm::vec3 vn = index.vn != -1 ? tempNormals[index.vn] : glm::vec3(0.0f);
                glm::vec2 vt = index.vt != -1 ? tempTexCoords[index.vt] : glm::vec2(0.0f);
                mVertexes.push_back({v, vn, vt});
            }

            for (size_t i = 0; i < word.size() - 2; ++i)
            {
                mIndices.push_back(beforeVertexSize);
                mIndices.push_back(beforeVertexSize + i + 1);
                mIndices.push_back(beforeVertexSize + i + 2);
            }
        }
    }

    mMeshes = Mesh::create(mVertexes, mIndices, GL_TRIANGLES);

    return true;
}