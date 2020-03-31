// Winter 2020

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
    const glm::vec3& kd, const glm::vec3& ks, double shininess, float glossiness, bool is_mirror)
    : m_kd(kd)
    , m_ks(ks)
    , m_shininess(shininess)
    , m_glossiness(glossiness)
    , m_mirror(is_mirror)
{}

PhongMaterial::~PhongMaterial()
{}
