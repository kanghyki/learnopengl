#ifndef INCLUDED_OBJECT_HPP
#define INCLUDED_OBJECT_HPP

#include "common.hpp"
#include "mesh.hpp"
#include "program.hpp"

struct Transform {
  glm::vec3 translate{0.0f};
  glm::vec3 scale{1.0f};
  glm::vec3 rotate{0.0f};

  glm::mat4 GetMatrix() {
    auto t = glm::translate(glm::mat4(1.0f), translate);
    auto s = glm::scale(glm::mat4(1.0f), scale);
    auto r = glm::rotate(glm::mat4(1.0f), glm::radians(rotate.z),
                         glm::vec3(0.0f, 0.0f, 1.0f)) *
             glm::rotate(glm::mat4(1.0f), glm::radians(rotate.y),
                         glm::vec3(0.0f, 1.0f, 0.0f)) *
             glm::rotate(glm::mat4(1.0f), glm::radians(rotate.x),
                         glm::vec3(1.0f, 1.0f, 1.0f));

    return t * s * r;
  }
};

class ObjectItem;
class ObjectGroup;
class ObjectComponent {
 public:
  virtual void Add(std::shared_ptr<ObjectComponent> c) {}
  virtual void Remove(std::shared_ptr<ObjectComponent> c) {}
  virtual void Draw(std::function<void(ObjectComponent *)> uniform_fn,
                    glm::mat4 model, Program *p) = 0;

  virtual Transform &transform() = 0;
  virtual glm::mat4 model() const = 0;

  virtual ObjectItem *FindOjbectItem(size_t id) = 0;
  virtual ObjectGroup *FindOjbectGroup(size_t id) = 0;
  virtual size_t id() const = 0;
};

class ObjectGroup : public ObjectComponent {
 public:
  static std::shared_ptr<ObjectGroup> Create();
  ~ObjectGroup();

  void Draw(const Program *program) const;
  void Add(std::shared_ptr<ObjectComponent> c) override;
  void Remove(std::shared_ptr<ObjectComponent> c) override;
  void Draw(std::function<void(ObjectComponent *)> uniform_fn, glm::mat4 model,
            Program *p) override;

  inline Transform &transform() override { return transform_; }
  inline glm::mat4 model() const override { return model_; }

  ObjectItem *FindOjbectItem(size_t id) override;
  ObjectGroup *FindOjbectGroup(size_t id) override;
  inline size_t id() const override { return id_; }

 private:
  static size_t kGroupId;

  ObjectGroup();
  ObjectGroup &operator=(const ObjectGroup &obj);

  Mesh *mesh_;
  Transform transform_;
  glm::mat4 model_;
  size_t id_;
  std::vector<std::shared_ptr<ObjectComponent>> components_;
};

class ObjectItem : public ObjectComponent {
 public:
  static std::shared_ptr<ObjectItem> Create(Mesh *mesh);
  ~ObjectItem();

  void Draw(std::function<void(ObjectComponent *)> uniform_fn, glm::mat4 model,
            Program *p) override;

  inline Transform &transform() override { return transform_; }
  inline glm::mat4 model() const override { return model_; }

  ObjectItem *FindOjbectItem(size_t id) override;
  ObjectGroup *FindOjbectGroup(size_t id) override;
  inline size_t id() const override { return id_; }

  inline Mesh *mesh() { return mesh_; }

 private:
  static size_t kItemId;
  ObjectItem(Mesh *mesh);
  ObjectGroup &operator=(const ObjectGroup &obj);

  Mesh *mesh_;
  Transform transform_;
  glm::mat4 model_;
  size_t id_;
};

#endif