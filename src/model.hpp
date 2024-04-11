#ifndef INCLUDED_MODEL_HPP
#define INCLUDED_MODEL_HPP

#include "common.hpp"
#include "mesh.hpp"

struct VertexIndex {
  int v;
  int vt;
  int vn;
};

class Model {
 public:
  static std::unique_ptr<Model> Load(const std::string &filename);
  ~Model();

  void Draw(const Program *program) const;

  std::shared_ptr<Mesh> mesh() { return mesh_; }

 private:
  Model();
  Model(const Model &model);

  bool ParseObjToMesh(const std::string &data);
  bool LoadMaterial();

  std::shared_ptr<Mesh> mesh_;
};

#endif