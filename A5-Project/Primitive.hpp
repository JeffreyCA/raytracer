// Winter 2020

#pragma once

#include <glm/glm.hpp>
#include "Ray.hpp"
#include "Intersection.hpp"

class NonhierSphere;
class NonhierBox;

class Primitive {
public:
  virtual ~Primitive();
  virtual Intersection intersect(const Ray &ray) = 0;
};

class Sphere : public Primitive {
  NonhierSphere *base_shape;
public:
  Sphere();
  virtual ~Sphere();
  virtual Intersection intersect(const Ray &ray) override;
};

class Cube : public Primitive {
  NonhierBox *base_shape;
public:
  Cube();
  virtual ~Cube();
  virtual Intersection intersect(const Ray &ray) override;
};

class NonhierPlane : public Primitive {
public:
  NonhierPlane() {}
  virtual ~NonhierPlane();
  virtual Intersection intersect(const Ray &ray) override;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  virtual Intersection intersect(const Ray &ray) override;

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierIrregularBox : public Primitive {
public:
  NonhierIrregularBox(const glm::vec3& pos, const glm::vec3 &size);
  virtual ~NonhierIrregularBox();
  virtual Intersection intersect(const Ray &ray) override;

protected:
  glm::vec3 m_pos;
  glm::vec3 min_corner;
  glm::vec3 max_corner;
  glm::vec3 dim;

  glm::vec3 get_normal(const glm::vec3 &point);
};

class NonhierBox : public NonhierIrregularBox {
public:
  NonhierBox(const glm::vec3& pos, double size);
  virtual ~NonhierBox();
  virtual Intersection intersect(const Ray &ray);
private:
  double m_size;
};
