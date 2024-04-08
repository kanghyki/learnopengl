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

class ObjectComponent abstract {
 public:
  virtual void Add(std::shared_ptr<ObjectComponent> c) = 0;
  virtual void Remove(std::shared_ptr<ObjectComponent> c) = 0;
  virtual void Draw(std::function<void(ObjectComponent *)> uniform_fn,
                    glm::mat4 model, Program *p) = 0;
  virtual ObjectComponent *FindChild(size_t id) = 0;

  virtual Mesh *mesh() = 0;
  virtual Transform &transform() = 0;
  virtual glm::mat4 model() const = 0;
  virtual size_t id() const = 0;
};

class Object : public ObjectComponent {
 public:
  static size_t kId;
  static std::shared_ptr<Object> Create(Mesh *mesh) {
    auto object = std::shared_ptr<Object>(new Object(mesh));

    return std::move(object);
  }
  ~Object() {}

  void Draw(const Program *program) const { mesh_->Draw(program); }
  inline void set_transform(Transform transform) { transform_ = transform; }

  void Add(std::shared_ptr<ObjectComponent> c) override {
    components_.push_back(c);
  }

  void Remove(std::shared_ptr<ObjectComponent> c) override {
    for (size_t i = 0; i < components_.size(); ++i) {
      if (c.get() == components_[i].get()) {
        this->components_.erase(this->components_.begin() + i);
        break;
      }
    }
  }

  virtual ObjectComponent *FindChild(size_t id) override {
    if (id == this->id()) return this;

    for (auto &component : components_) {
      ObjectComponent *ret = component->FindChild(id);
      if (ret != nullptr) {
        return ret;
      }
    }
    return nullptr;
  }

  size_t id() const override { return id_; }

  void Draw(std::function<void(ObjectComponent *)> uniform_fn, glm::mat4 model,
            Program *p) override {
    model_ = model * transform().GetMatrix();
    uniform_fn(this);
    mesh()->Draw(p);
    for (const auto &component : components_) {
      component->Draw(uniform_fn, model_, p);
    }
  }

  Mesh *mesh() override { return mesh_; }

  Transform &transform() override { return transform_; }
  glm::mat4 model() const override { return model_; }

 private:
  Object(Mesh *mesh) : id_(Object::kId++), mesh_(mesh) {}
  Object &operator=(const Object &obj);

  Mesh *mesh_;
  Transform transform_;
  glm::mat4 model_;
  size_t id_;
  std::vector<std::shared_ptr<ObjectComponent>> components_;
};

// class ObjectItem : public ObjectComponent {
//  public:
//   static std::shared_ptr<ObjectItem> Create(Mesh *mesh) {
//     return std::shared_ptr<ObjectItem>(new ObjectItem(mesh));
//   }
//   ~ObjectItem() {}

//   void Add(std::shared_ptr<ObjectComponent> c) override {}

//   void Remove(std::shared_ptr<ObjectComponent> c) override {}

//   std::shared_ptr<ObjectComponent> GetChild(int i) override { return nullptr;
//   }

//   void Draw(const std::string &name, glm::mat4 model, Program *p) override {
//     p->SetUniform(name, model * transform().GetMatrix());
//     mesh()->Draw(p);
//   }

//   Mesh *mesh() override { return mesh_; }

//   Transform &transform() override { return transform_; }

//  private:
//   ObjectItem() {}
//   ObjectItem(Mesh *mesh) { mesh_ = mesh; }
//   Object &operator=(const Object &obj);

//   Mesh *mesh_;
//   Transform transform_;
// };

#endif