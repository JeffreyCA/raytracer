// Winter 2020

#pragma once
#include "Ray.hpp"
#include <glm/glm.hpp>
#include "PhongMaterial.hpp"
#include <string>

class Intersection {
public:
    Intersection(const Ray &ray, const glm::vec3 &normal, float t, bool hit);
    virtual ~Intersection();
    static Intersection NonIntersection(const Ray &ray) {
        return Intersection(ray, glm::vec3(0.0f), 0, false);
    }
    void convert_to_world(const glm::mat4 &trans, const glm::mat4 &invtrans);

    bool is_hit() const;
    float get_t() const;
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

    // Stores kd, ks, shininess
    PhongMaterial *material;
};
