#include "Intersection.hpp"
using namespace glm;

Intersection::Intersection(const Ray &ray, const vec3 &normal, float t, bool hit): ray(ray), N(normalize(normal)), t(t), u(0), v(0), hit(hit) {
    point = ray.get_point(t);
}

Intersection::Intersection(const Ray &ray, const vec3 &normal, float t, float u, float v, bool hit): ray(ray), N(normalize(normal)), t(t), u(u), v(v), hit(hit) {
    point = ray.get_point(t);
}

void Intersection::convert_to_world(const glm::mat4 &trans, const glm::mat4 &invtrans) {
    ray.origin = vec3(trans * vec4(ray.origin, 1.0f));
    ray.direction = vec3(trans * vec4(ray.direction, 0.0f));
    point = vec3(trans * vec4(point, 1.0f));
    N = normalize(transpose(mat3(invtrans)) * N);
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

float Intersection::get_u() const {
    return u;
}

float Intersection::get_v() const {
    return v;
}

void Intersection::set_material(PhongMaterial* material) {
    this->material = material;
}

PhongMaterial* Intersection::get_material() const {
    return material;
}

Intersection::~Intersection() {}
