// Winter 2020

#pragma once
#include "Ray.hpp"
#include <glm/glm.hpp>
#include "PhongMaterial.hpp"
#include <string>

class Intersection {
public:
    Intersection(const Ray &ray, const glm::vec3 &normal, float t, bool hit);
    Intersection(const Ray &ray, const glm::vec3 &normal, float t, float u, float v, bool hit);
    virtual ~Intersection();
    static Intersection NonIntersection(const Ray &ray) {
        return Intersection(ray, glm::vec3(0.0f), 0, false);
    }
    void convert_to_world(const glm::mat4 &trans, const glm::mat4 &invtrans);

    bool is_hit() const;
    float get_t() const;
    float get_u() const;
    float get_v() const;
    const glm::vec3& get_N() const;
    const glm::vec3& get_point() const;
    void set_material(PhongMaterial* material);
    PhongMaterial* get_material() const;

private:
    Ray ray;
    glm::vec3 N;
    float t;
    bool hit;
    glm::vec3 point;
    float u;
    float v;

    // Stores kd, ks, shininess
    PhongMaterial *material;
};
