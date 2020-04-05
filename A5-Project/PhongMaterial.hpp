// Winter 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "Texture.hpp"

class PhongMaterial : public Material {
public:
    PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, float glossiness, float eta, bool is_mirror, bool is_opaque);
    PhongMaterial(Texture *texture, const glm::vec3& ks, double shininess, bool ignore_lighting);
    virtual ~PhongMaterial();

    Texture *m_texture;
    glm::vec3 m_kd;
    glm::vec3 m_ks;

    double m_shininess;
    float m_glossiness;
    float m_refractive_index;
    bool m_mirror;
    bool m_opaque;
    bool m_textured;
    bool m_ignore_lighting;
};
