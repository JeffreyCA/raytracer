#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

void render(
    // What to render
    SceneNode * root,

    // Image to write to, set to a given width and height
    Image & image,

    // Viewing parameters
    const glm::vec3 & look_from,
    const glm::vec3 & look_at,
    const glm::vec3 & up,
    double fovy,

    // Lighting parameters
    const glm::vec3 & ambient,
    const std::list<Light *> & lights
);
