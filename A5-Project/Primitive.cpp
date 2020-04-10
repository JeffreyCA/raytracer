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

Cone::~Cone() {}

Intersection Cone::intersect(const Ray& ray) {
    const double A = pow(ray.direction.x, 2) + pow(ray.direction.y, 2) - pow(ray.direction.z, 2);
    const double B = 2 * (ray.origin.x * ray.direction.x + ray.origin.y * ray.direction.y - ray.origin.z * ray.direction.z);
    const double C = pow(ray.origin.x, 2) + pow(ray.origin.y, 2) - pow(ray.origin.z, 2);

    double roots[2];
    const size_t num_roots = quadraticRoots(A, B, C, roots);

    if (num_roots == 0) {
        return Intersection::NonIntersection(ray);
    } else if (num_roots == 1) {
        const double min_root = roots[0];
        const vec3 point = ray.get_point(min_root);
        if (point.z <= z_max && point.z >= z_min) {
            const vec3 normal = vec3(2 * point.x, 2 * point.y, -2 * point.z);
            const float u = 0.5f + atan2(point.x, point.y) / (2 * M_PI);
            const float v = -point.z / m_length;
            return Intersection(ray, normal, min_root, u, v, min_root > 0);
        }
    } else {
        const double min_t = std::min(roots[0], roots[1]);
        const double max_t = std::max(roots[0], roots[1]);
        const vec3 p1 = ray.get_point(min_t);
        const vec3 p2 = ray.get_point(max_t);
        const bool valid_t1 = p1.z >= z_min && p1.z <= z_max;
        const bool valid_t2 = p2.z >= z_min && p2.z <= z_max;

        if (!valid_t1 && !valid_t2) {
            return Intersection::NonIntersection(ray);
        } else if (valid_t1 && valid_t2) {
            double min_root;
            if (min_t < 0 && max_t > 0) {
                min_root = max_t;
            } else {
                min_root = min_t;
            }
            const vec3 point = ray.get_point(min_root);
            const vec3 normal = normalize(vec3(2 * point.x, 2 * point.y, -2 * point.z));
            const float u = 0.5f + atan2(point.x, point.y) / (2 * M_PI);
            const float v = -point.z / m_length;
            return Intersection(ray, normal, min_root, u, v, min_root > 0);
        } else {
            const double z1 = ray.get_point(min_t).z;
            const double z2 = ray.get_point(max_t).z;
            const double larger_z = std::max(z1, z2);
            const double smaller_z = std::min(z1, z2);
            const bool smaller_in_larger_out = smaller_z <= z_max && smaller_z >= z_min && larger_z > z_max;
            const bool larger_in_smaller_out = larger_z >= z_min && larger_z <= z_max && smaller_z < z_min;

            if (smaller_in_larger_out || larger_in_smaller_out) {
                // Intersection with cap
                const double cap_t = (z_min - ray.origin.z) / ray.direction.z;
                vec3 normal;

                double t1;
                double t2;
                double min_root = 0;

                if (valid_t1) {
                    t1 = std::min(min_t, cap_t);
                    t2 = std::max(min_t, cap_t);
                } else {
                    t1 = std::min(max_t, cap_t);
                    t2 = std::max(max_t, cap_t);
                }

                if (t1 > 0) {
                    // Smaller t value is positive
                    min_root = t1;
                } else if (t1 < 0 && t2 > 0) {
                    // Smaller t value is negative, larger t value is positive (ray origin inside cone)
                    min_root = t2;
                }
                const vec3 point = ray.get_point(min_root);
                if (min_root == cap_t) {
                    // Hit cap
                    normal = vec3(0.0, 0.0, -1);
                } else {
                    // Hit lateral sides
                    normal = normalize(vec3(2 * point.x, 2 * point.y, -2 * point.z));
                }
                const float u = 0.5f + atan2(point.x, point.y) / (2 * M_PI);
                const float v = -point.z / m_length;
                return Intersection(ray, normal, min_root, u, v, min_root > 0);
            }
        }
    }

    return Intersection::NonIntersection(ray);
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

Cylinder::~Cylinder() {}

Intersection Cylinder::intersect(const Ray &ray) {
    const double A = pow(ray.direction.x, 2) + pow(ray.direction.y, 2);
    const double B = 2 * (ray.origin.x * ray.direction.x + ray.origin.y * ray.direction.y);
    const double C = pow(ray.origin.x, 2) + pow(ray.origin.y, 2) - pow(m_radius, 2);

    double roots[2];
    const size_t root_count = quadraticRoots(A, B, C, roots);

    if (root_count == 0) {
        return Intersection::NonIntersection(ray);
    } else if (root_count == 1) {
        const double min_root = roots[0];
        const vec3 point = ray.get_point(min_root);
        if (point.z <= z_max && point.z >= z_min) {
            const vec3 normal = normalize(vec3(point.x, point.y, 0));
            return Intersection(ray, normal, min_root, min_root > 0);
        }
    } else if (root_count == 2) {
        const double min_t = std::min(roots[0], roots[1]);
        const double max_t = std::max(roots[0], roots[1]);
        const double t1 = min_t;
        const double t2 = max_t;
        const vec3 p1 = ray.get_point(t1);
        const vec3 p2 = ray.get_point(t2);

        if (p1.z < z_min && p2.z >= z_min) {
            // Hit back cap
            const double min_root = t1 + (t2 - t1) * (p1.z - z_min) / (p1.z - p2.z);
            const vec3 point = ray.get_point(min_root);
            const vec3 normal = vec3(0, 0, -1);
            const float u = 0.5f + atan2(point.x, point.y) / (2 * M_PI);
            const float v = point.z / m_length + 0.5f;
            return Intersection(ray, normal, min_root, u, v, min_root > 0);
        } else if (p1.z >= z_min && p1.z <= z_max) {
            // Hit lateral sides
            const vec3 point = ray.get_point(t1);
            const vec3 normal = normalize(vec3(point.x, point.y, 0));
            const float u = 0.5f + atan2(point.x, point.y) / (2 * M_PI);
            const float v = point.z / m_length + 0.5f;
            return Intersection(ray, normal, t1, u, v, t1 > 0);
        } else if (p1.z > z_max && p2.z <= z_max) {
            // Hit front cap
            const double min_root = t1 + (t2 - t1) * (p1.z - z_max) / (p1.z - p2.z);
            const vec3 point = ray.get_point(min_root);
            const vec3 normal = vec3(0, 0, 1);
            const float u = 0.5f + atan2(point.x, point.y) / (2 * M_PI);
            const float v = point.z / m_length + 0.5f;
            return Intersection(ray, normal, min_root, u, v, min_root > 0);
        }
    }

    return Intersection::NonIntersection(ray);
}

NonhierPlane::~NonhierPlane() {}

Intersection NonhierPlane::intersect(const Ray &ray) {
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
            float u = p_on_plane.x + 0.5f;
            float v = p_on_plane.y + 0.5f;
            return Intersection(ray, normal, t, u, v, t > 0);
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

        const vec3 normal = normalize(ray.get_point(min_root) - m_pos);
        const float u = 0.5f + atan2(-normal.z, normal.x) / (2 * M_PI);
        const float v = 0.5f - asin(normal.y) / M_PI;
        return Intersection(ray, normal, min_root, u, v, min_root > 0);
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

NonhierBox::NonhierBox(const vec3& pos, double size): NonhierIrregularBox(pos, vec3(size, size, size)) {}

NonhierBox::~NonhierBox() {}

Intersection NonhierBox::intersect(const Ray &ray) {
    return NonhierIrregularBox::intersect(ray);
}
