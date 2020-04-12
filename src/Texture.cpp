

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

#include "Texture.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/io.hpp>

using namespace glm;
using namespace std;

Texture::~Texture() {}

CheckeredTexture::CheckeredTexture(vec3 colour_1, vec3 colour_2, float scale_factor): colour_1(colour_1), colour_2(colour_2), scale_factor(scale_factor) {}

CheckeredTexture::~CheckeredTexture() {}

vec3 CheckeredTexture::get_colour(float u, float v) {
    const int u2 = (int) std::round(u * scale_factor);
    const int v2 = (int) std::round(v * scale_factor);

    if (u2 % 2 == 0) {
        if (v2 % 2 == 0) {
            return colour_1;
        } else {
            return colour_2;
        }
    } else {
        if (v2 % 2 == 0) {
            return colour_2;
        } else {
            return colour_1;
        }
    }
}

StripedTexture::StripedTexture(vec3 colour_1, vec3 colour_2, float scale_factor, bool horizontal) : colour_1(colour_1), colour_2(colour_2), scale_factor(scale_factor), horizontal(horizontal) {}

StripedTexture::~StripedTexture() {}

vec3 StripedTexture::get_colour(float u, float v) {
    const int u2 = (int) std::round(u * scale_factor);
    const int v2 = (int) std::round(v * scale_factor);

    if (horizontal) {
        if (u2 % 2 == 0) {
            return colour_1;
        } else {
            return colour_2;
        }
    } else {
        if (v2 % 2 == 0) {
            return colour_1;
        } else {
            return colour_2;
        }
    }
}

ImageTexture::ImageTexture(const std::string &filename, float width_factor, float height_factor): width_factor(width_factor), height_factor(height_factor) {
    std::cout << "Loading texture " << filename << std::endl;
    image = Image::loadPng(filename);
}

ImageTexture::~ImageTexture() {}

vec3 ImageTexture::get_colour(float u, float v) {
    const uint image_width = image.width();
    const uint image_height = image.height();

    const uint x = (uint) std::round(u * width_factor * image_width) % image_width;
    const uint y = (uint) std::round(v * height_factor * image_height) % image_height;

    const double r = image(x, y, 0);
    const double g = image(x, y, 1);
    const double b = image(x, y, 2);

    return vec3(r, g, b);
}
