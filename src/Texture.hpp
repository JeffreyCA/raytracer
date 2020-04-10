#pragma once

#include <glm/glm.hpp>
#include "Image.hpp"

class Texture {
public:
    virtual ~Texture();
    virtual glm::vec3 get_colour(float u, float v) = 0;
};

class CheckeredTexture : public Texture {
public:
    CheckeredTexture(glm::vec3 colour_1, glm::vec3 colour_2, float scale_factor);
    virtual ~CheckeredTexture();
    glm::vec3 get_colour(float u, float v) override;
private:
    glm::vec3 colour_1;
    glm::vec3 colour_2;
    float scale_factor;
};

class ImageTexture : public Texture {
public:
    ImageTexture(const std::string &filename);
    virtual ~ImageTexture();
    glm::vec3 get_colour(float u, float v) override;
private:
    Image image;
};

