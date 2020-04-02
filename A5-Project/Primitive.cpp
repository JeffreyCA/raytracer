// Winter 2020

#include <iostream>
#include "Primitive.hpp"
#include "polyroots.hpp"
#include <glm/gtx/io.hpp>

using namespace std;
using namespace glm;

static const float EPSILON = 0.0001f;

Primitive::~Primitive()
{
}

Sphere::Sphere() {
    base_shape = new NonhierSphere(vec3(0, 0, 0), 1.0);
}

Sphere::~Sphere() {
    delete base_shape;
}

Intersection Sphere::intersect(const Ray &ray) {
    return base_shape->intersect(ray);
}

Cube::Cube() {
    base_shape = new NonhierBox(vec3(0, 0, 0), 1.0);
}

Cube::~Cube() {
    delete base_shape;
}

Intersection Cube::intersect(const Ray &ray) {
    return base_shape->intersect(ray);
}

NonhierPlane::~NonhierPlane() {}

Intersection NonhierPlane::intersect(const Ray &ray) {
    const vec3 point = vec3(0, 0, 0);
    vec3 normal = vec3(0, 0, 1.0f);
    
    float numerator = -dot(normal, ray.origin);
    float denominator = dot(normal, ray.direction);

    if (fabs(denominator) < EPSILON) {
        return Intersection::NonIntersection(ray);
    }

    float t = numerator / denominator;
    if (t > 0) {
        vec3 p_on_plane = ray.get_point(t);
        
        if (p_on_plane.x >= -0.5 && p_on_plane.x <= 0.5 && p_on_plane.y >= -0.5 && p_on_plane.y <= 0.5) {
            if (dot(ray.direction, normal) > 0) {
                normal = -normal;
            }
            return Intersection(ray, normal, t, t > 0);
        }
    }

    return Intersection::NonIntersection(ray);
}

NonhierSphere::~NonhierSphere() {}

Intersection NonhierSphere::intersect(const Ray &ray) {
    const double A = dot(ray.direction, ray.direction);
    const double B = 2.0 * dot(ray.direction, ray.origin - m_pos);
    const double C = dot(ray.origin - m_pos, ray.origin - m_pos) - pow(m_radius, 2);

    double roots[2];
    const size_t root_count = quadraticRoots(A, B, C, roots);

    if (root_count == 0) {
        return Intersection::NonIntersection(ray);
    } else {
        double min_root;

        if (root_count == 1) {
            min_root = roots[0];
        } else {
            const double min = std::min(roots[0], roots[1]);
            const double max = std::max(roots[0], roots[1]);
            
            // Ray origin inside sphere, so min root is actually the larger of the two
            if (min < 0 && max >= 0) {
                min_root = max;
            } else {
                min_root = min;
            }
        }

        const vec3 normal = ray.get_point(min_root) - m_pos;
        return Intersection(ray, normal, min_root, min_root > 0);
    }
}

NonhierIrregularBox::NonhierIrregularBox(const vec3 &pos, const vec3 &dim) : m_pos(pos), dim(dim) {
    min_corner = vec3(m_pos.x, m_pos.y, m_pos.z);
    max_corner = vec3(m_pos.x + dim.x, m_pos.y + dim.y, m_pos.z + dim.z);
}

NonhierIrregularBox::~NonhierIrregularBox() {}

// https://gamedev.stackexchange.com/a/18459
Intersection NonhierIrregularBox::intersect(const Ray &ray) {
    const vec3 reciprocal = 1.0f / ray.direction;
    const float t1 = (min_corner.x - ray.origin.x) * reciprocal.x;
    const float t2 = (max_corner.x - ray.origin.x) * reciprocal.x;
    const float t3 = (min_corner.y - ray.origin.y) * reciprocal.y;
    const float t4 = (max_corner.y - ray.origin.y) * reciprocal.y;
    const float t5 = (min_corner.z - ray.origin.z) * reciprocal.z;
    const float t6 = (max_corner.z - ray.origin.z) * reciprocal.z;

    const float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    const float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    // Box is behind ray
    if (tmax < 0) {
        return Intersection::NonIntersection(ray);
    }

    // No intersection
    if (tmin > tmax) {
        return Intersection::NonIntersection(ray);
    }

    // Intersects
    const vec3 inter_point = ray.get_point(tmin);
    vec3 normal = get_normal(inter_point);
    return Intersection(ray, normal, tmin, true);
}

// http://ray-tracing-conept.blogspot.com/2015/01/ray-box-intersection-and-normal.html
vec3 NonhierIrregularBox::get_normal(const vec3 &intersection) {
    if (fabs(intersection.x - min_corner.x) < EPSILON) {
        return vec3(-1.0f, 0, 0);
    } else if (fabs(intersection.x - max_corner.x) < EPSILON) {
        return vec3(1.0f, 0, 0);
    } else if (fabs(intersection.y - min_corner.y) < EPSILON) {
        return vec3(0, -1.0f, 0);
    } else if (fabs(intersection.y - max_corner.y) < EPSILON) {
        return vec3(0, 1.0f, 0);
    } else if (fabs(intersection.z - min_corner.z) < EPSILON) {
        return vec3(0, 0, -1.0f);
    } else {
        return vec3(0, 0, 1.0f);
    }
}

NonhierBox::NonhierBox(const vec3& pos, double size): m_size(size), NonhierIrregularBox(pos, vec3(size, size, size)) {}

NonhierBox::~NonhierBox() {}

Intersection NonhierBox::intersect(const Ray &ray) {
    return NonhierIrregularBox::intersect(ray);
}
