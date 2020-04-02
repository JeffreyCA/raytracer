// Winter 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
    PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, float glossiness, float eta, bool is_mirror, bool is_opaque);
    virtual ~PhongMaterial();

    glm::vec3 m_kd;
    glm::vec3 m_ks;

    double m_shininess;
    float m_glossiness;
    float m_refractive_index;
    bool m_mirror;
    bool m_opaque;
};
