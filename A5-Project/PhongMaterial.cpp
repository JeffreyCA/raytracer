// Winter 2020

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks,
                             double shininess, float glossiness, float eta, bool is_mirror,
                             bool is_opaque)
    : m_kd(kd),
      m_ks(ks),
      m_shininess(shininess),
      m_glossiness(glossiness),
      m_refractive_index(eta),
      m_mirror(is_mirror),
      m_opaque(is_opaque) {}

PhongMaterial::~PhongMaterial() {}
