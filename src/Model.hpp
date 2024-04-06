#ifndef INCLUDED_MODEL_HPP
#define INCLUDED_MODEL_HPP

#include "Mesh.hpp"
#include "common.hpp"

struct VertexIndex {
  int v;
  int vt;
  int vn;
};

class Model {
public:
  ~Model();
  static std::unique_ptr<Model> load(const std::string &filename);

  void draw(const Program *program) const;

private:
  Model();
  Model(const Model &model);
  Model &operator=(const Model &model);

  bool parseObjToMesh(const std::string &data);
  bool loadMaterial();

  std::unique_ptr<Mesh> mMesh;
};

#endif