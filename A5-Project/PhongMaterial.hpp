// Winter 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
    PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, bool is_mirror);
    virtual ~PhongMaterial();

    glm::vec3 m_kd;
    glm::vec3 m_ks;

    double m_shininess;
    bool m_mirror;
};
