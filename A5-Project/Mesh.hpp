// Winter 2020

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

// A polygonal mesh.
class Mesh : public Primitive {
public:
    Mesh(const std::string& fname);
    virtual ~Mesh();
    virtual Intersection intersect(const Ray &ray) override;

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<Triangle> m_faces;
    NonhierIrregularBox *bounding_box;
    bool is_plane;
    int num_cells;

    int grid_dim;
    glm::vec3 cellDimension;
    glm::vec3 min_point;
    glm::vec3 max_point;
    glm::ivec3 resolution;
    std::vector<std::vector<Triangle>> myvector;

    glm::vec3 intersect_ray_triangle(const Ray &ray, glm::vec3 &P0, glm::vec3 &P1, glm::vec3 &P2);
    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
