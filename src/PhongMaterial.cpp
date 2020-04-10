#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks,
                             double shininess, float glossiness, float eta,
                             bool is_mirror, bool is_opaque)
    : m_kd(kd),
      m_ks(ks),
      m_shininess(shininess),
      m_glossiness(glossiness),
      m_refractive_index(eta),
      m_mirror(is_mirror),
      m_opaque(is_opaque),
      m_textured(false),
      m_ignore_lighting(false) {}

PhongMaterial::PhongMaterial(Texture *texture, const glm::vec3& ks, double shininess, bool ignore_lighting)
    : m_texture(texture),
      m_kd(glm::vec3(0)),
      m_ks(ks),
      m_shininess(shininess),
      m_glossiness(0.0f),
      m_refractive_index(0.0f),
      m_mirror(false),
      m_opaque(true),
      m_textured(true),
      m_ignore_lighting(ignore_lighting) {}

PhongMaterial::~PhongMaterial() {}
