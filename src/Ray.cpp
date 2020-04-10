#include "Ray.hpp"
#include <glm/gtx/io.hpp>

using namespace glm;

Ray::Ray(glm::vec3 origin, glm::vec3 direction): origin(origin), direction(direction) {}

Ray::Ray(const Ray & other) : origin(other.origin), direction(other.direction) {}

vec3 Ray::get_point(float t) const {
    return origin + t * direction;
}

vec3 Ray::get_normalized_direction() const {
    return normalize(direction);
}

std::ostream & operator << (std::ostream & os, const Ray & ray) {
    os << ":[";

    os << "origin: " << ray.origin << ", ";
    os << "direction:" << ray.direction;

    os << "]\n";
    return os;
}
