#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
// #define RENDER_BOUNDING_VOLUMES

glm::vec3 intersect_ray_triangle(const Ray &ray, glm::vec3 &P0, glm::vec3 &P1, glm::vec3 &P2);

enum class ObjType {
    Textures,
    Normals,
    Vertices
};

struct Triangle
{
    size_t v1;
    size_t v2;
    size_t v3;

    Triangle( size_t pv1, size_t pv2, size_t pv3 )
        : v1( pv1 )
        , v2( pv2 )
        , v3( pv3 )
    {}
};

struct Face {
    Triangle *vertices;
    Triangle *normals;
    Triangle *textures;
    Face(Triangle *vertices, Triangle *normals, Triangle *textures) : vertices(vertices), normals(normals), textures(textures) {}
    Face(Triangle *vertices, Triangle *normals) : vertices(vertices), normals(normals), textures(nullptr) {}
    Face(Triangle *vertices) : vertices(vertices), normals(nullptr), textures(nullptr) {}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
    Mesh(const std::string& fname, ObjType type);
    virtual ~Mesh();
    virtual Intersection intersect(const Ray &ray) override;

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_textures;
    std::vector<Face> m_faces;

    NonhierIrregularBox *bounding_box;
    bool skip_accel;
    ObjType type;
    int num_cells;

    int grid_dim;
    glm::vec3 cell_dim;
    glm::vec3 min_point;
    glm::vec3 max_point;
    std::vector<std::vector<Face>> grid_vector;

    glm::vec3 intersect_ray_triangle(const Ray &ray, const glm::vec3 &P0, const glm::vec3 &P1, const glm::vec3 &P2);
    Intersection intersect_ray_grid(const Ray &ray, const Intersection &bounding_box_intersection);
    Intersection intersect_ray_regular(const Ray &ray);
};
