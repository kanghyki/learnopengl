#ifndef INCLUDED_OBJECT_HPP
#define INCLUDED_OBJECT_HPP

#include "bounding_sphere.hpp"
#include "common.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include "ray.hpp"
#include "transform.hpp"

class DrawableObject {
  public:
    DrawableObject(std::shared_ptr<Mesh> mesh) : mesh_(mesh) {};
    virtual ~DrawableObject() {};

    inline virtual void Draw(const Program* p) const final { mesh_->Draw(p); }
    inline virtual std::shared_ptr<Mesh> mesh() const final { return mesh_; }

  private:
    std::shared_ptr<Mesh> mesh_;
};

class TransformableObject {
  public:
    TransformableObject() {};
    virtual ~TransformableObject() {};

    inline virtual Transform& transform() final { return transform_; }
    inline virtual const Transform& transform() const final { return transform_; }

  private:
    Transform transform_;
};

class TouchableObject {
  public:
    TouchableObject() {}
    virtual ~TouchableObject() {}

    virtual void CreateBoundingSphere(float radius) final {
        bounding_sphere_ = BoundingSphere::Create(radius);
    }

    virtual std::optional<float> Intersect(const Ray& ray, const Transform& t) final {
        if (!bounding_sphere_) {
            return {};
        }
        return bounding_sphere_->Intersect(ray, t);
    }

  private:
    std::unique_ptr<BoundingSphere> bounding_sphere_{nullptr};
};

class Object : public DrawableObject, public TransformableObject, public TouchableObject {
  public:
    static std::shared_ptr<Object> Create(std::shared_ptr<Mesh> mesh) {
        auto object = std::shared_ptr<Object>(new Object(mesh));

        return std::move(object);
    }
    ~Object() {};

    inline size_t id() const { return id_; }

  protected:
    Object(std::shared_ptr<Mesh> mesh)
        : DrawableObject(mesh), TransformableObject(), TouchableObject(), id_(Object::kId++) {}

  private:
    static size_t kId;
    const size_t id_;
};

enum ObjectType {
    kNormal,
    kLight,
};

#endif