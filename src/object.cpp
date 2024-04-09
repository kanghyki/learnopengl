#include "object.hpp"

size_t ObjectGroup::kGroupId = 0;

std::shared_ptr<ObjectGroup> ObjectGroup::Create() {
  auto object = std::shared_ptr<ObjectGroup>(new ObjectGroup());

  return std::move(object);
}

ObjectGroup::ObjectGroup() : id_(ObjectGroup::kGroupId++), mesh_(nullptr) {}

ObjectGroup::~ObjectGroup() {}

void ObjectGroup::Draw(const Program *program) const { mesh_->Draw(program); }

void ObjectGroup::Add(std::shared_ptr<ObjectComponent> c) {
  components_.push_back(c);
}

void ObjectGroup::Remove(std::shared_ptr<ObjectComponent> c) {
  for (size_t i = 0; i < components_.size(); ++i) {
    if (c.get() == components_[i].get()) {
      this->components_.erase(this->components_.begin() + i);
      break;
    }
  }
}

ObjectItem *ObjectGroup::FindOjbectItem(size_t id) {
  for (auto &component : components_) {
    ObjectItem *ret = component->FindOjbectItem(id);
    if (ret != nullptr) {
      return ret;
    }
  }
  return nullptr;
}

ObjectGroup *ObjectGroup::FindOjbectGroup(size_t id) {
  if (id == this->id()) return this;
  for (auto &component : components_) {
    ObjectGroup *ret = component->FindOjbectGroup(id);
    if (ret != nullptr) {
      return ret;
    }
  }
  return nullptr;
}

void ObjectGroup::Draw(std::function<void(ObjectComponent *)> uniform_fn,
                       glm::mat4 model, Program *p) {
  model_ = model * transform().GetMatrix();
  for (const auto &component : components_) {
    component->Draw(uniform_fn, model_, p);
  }
}

//

size_t ObjectItem::kItemId = 0;

std::shared_ptr<ObjectItem> ObjectItem::Create(Mesh *mesh) {
  auto object = std::shared_ptr<ObjectItem>(new ObjectItem(mesh));

  return std::move(object);
}

ObjectItem::ObjectItem(Mesh *mesh) : id_(ObjectItem::kItemId++), mesh_(mesh) {}

ObjectItem::~ObjectItem() {}

ObjectItem *ObjectItem::FindOjbectItem(size_t id) {
  if (id == this->id()) return this;

  return nullptr;
}

ObjectGroup *ObjectItem::FindOjbectGroup(size_t id) { return nullptr; }

void ObjectItem::Draw(std::function<void(ObjectComponent *)> uniform_fn,
                      glm::mat4 model, Program *p) {
  model_ = model * transform().GetMatrix();
  uniform_fn(this);
  mesh_->Draw(p);
}