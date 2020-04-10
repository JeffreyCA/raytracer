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

class Cube : public Primitive {
  NonhierBox *base_shape;
public:
  Cube();
  virtual ~Cube();
  virtual Intersection intersect(const Ray &ray) override;
};

class Cone : public Primitive {
public:
  Cone(double length) : m_length(length) {
    // Tip is at z = 0, base is at z = -m_length
    z_max = 0;
    z_min = -m_length;
  }
  virtual ~Cone();
  virtual Intersection intersect(const Ray &ray) override;

private:
  double m_length;
  double z_max;
  double z_min;
};

class Cylinder : public Primitive {
public:
  Cylinder(double radius, double length) : m_radius(radius), m_length(length) {
    z_min = -m_length / 2.0;
    z_max = m_length / 2.0;
  }
  virtual ~Cylinder();
  virtual Intersection intersect(const Ray &ray) override;

private:
  double m_radius;
  double m_length;
  double z_min;
  double z_max;
};

class Sphere : public Primitive {
  NonhierSphere *base_shape;
public:
  Sphere();
  virtual ~Sphere();
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
};
