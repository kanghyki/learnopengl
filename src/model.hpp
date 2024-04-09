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
  Mesh *GetMeshPtr() { return mesh_.get(); }  // test

 private:
  Model();
  Model(const Model &model);
  Model &operator=(const Model &model);

  bool ParseObjToMesh(const std::string &data);
  bool LoadMaterial();

  std::unique_ptr<Mesh> mesh_;
};

#endif