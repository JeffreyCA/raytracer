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

class StripedTexture : public Texture {
public:
    StripedTexture(glm::vec3 colour_1, glm::vec3 colour_2, float scale_factor, bool horizontal);
    virtual ~StripedTexture();
    glm::vec3 get_colour(float u, float v) override;
private:
    glm::vec3 colour_1;
    glm::vec3 colour_2;
    float scale_factor;
    bool horizontal;
};

class ImageTexture : public Texture {
public:
    ImageTexture(const std::string &filename, float width_factor, float height_factor, bool flip_v);
    virtual ~ImageTexture();
    glm::vec3 get_colour(float u, float v) override;
private:
    Image image;
    float width_factor;
    float height_factor;
    bool flip_v;
};

