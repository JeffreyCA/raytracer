#pragma once

#include <glm/glm.hpp>
#include <ostream>

struct Ray {
    Ray(glm::vec3 origin, glm::vec3 direction);
    Ray(const Ray &other);
    glm::vec3 get_point(float t) const;
    glm::vec3 get_normalized_direction();
    friend std::ostream & operator << (std::ostream & os, const Ray & ray);

    glm::vec3 origin;
    glm::vec3 direction;
};
