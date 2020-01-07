// Winter 2020

#include "Intersection.hpp"
using namespace glm;

Intersection::Intersection(const Ray &ray, const vec3 &normal, float t, bool hit): ray(ray), t(t), N(normalize(normal)), hit(hit) {
    if (dot(ray.direction, N) > 0) {
        N = -N;
    }
    point = ray.get_point(t);
}

void Intersection::convert_to_world(const glm::mat4 &trans, const glm::mat4 &invtrans) {
    ray.origin = vec3(trans * vec4(ray.origin, 1.0f));
    ray.direction = vec3(trans * vec4(ray.direction, 0.0f));
    point = vec3(trans * vec4(point, 1.0f));
    N = normalize(transpose(mat3(invtrans)) * N);
    if (dot(ray.direction, N) > 0) {
        N = -N;
    }
}

bool Intersection::is_hit() const {
    return hit;
}

const vec3& Intersection::get_N() const {
    return N;
}

const vec3& Intersection::get_point() const {
    return point;
}

float Intersection::get_t() const {
    return t;
}

void Intersection::set_material(PhongMaterial* material) {
    this->material = material;
}

PhongMaterial* Intersection::get_material() const {
    return material;
}

Intersection::~Intersection() {}
