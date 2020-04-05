// Winter 2020

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/io.hpp>

using namespace glm;
using namespace std;

#define GRID_DIM 28

static const float EPSILON = 0.0001f;

Mesh::Mesh(const std::string& fname, ObjType type)
    : m_vertices()
    , m_faces()
{
    std::string code;
    char slash;
    double vx, vy, vz;
    size_t s1, s2, s3, skip;
    size_t n1, n2, n3;

    std::ifstream ifs;
    ifs = std::ifstream(fname.c_str());

    if (!ifs.good()) {
        ifs = std::ifstream(("Assets/" + fname).c_str());
    }

    has_normals = (type == ObjType::WithNormals);

    while( ifs >> code ) {
        if (code == "v") {
            ifs >> vx >> vy >> vz;
            m_vertices.push_back(glm::vec3(vx, vy, vz));
        } else if (code == "vn") {
            ifs >> vx >> vy >> vz;
            m_normals.push_back(glm::vec3(vx, vy, vz));
        } else if ( code == "f" ) {
            if (type == ObjType::WithNormals) {
                ifs >> s1 >> slash >> n1 >> s2 >> slash >> n2 >> s3 >> slash >> n3;
                Triangle vertices(s1 - 1, s2 - 1, s3 - 1);
                Triangle normals(n1 - 1, n2 - 1, n3 - 1);
                m_faces.push_back(Face(vertices, normals));
                // m_normal_faces.push_back(Triangle(n1 - 1, n2 - 1, n3 - 1));
            } else if (type == ObjType::VerticesOnly) {
                ifs >> s1 >> s2 >> s3;
                Triangle vertices(s1 - 1, s2 - 1, s3 - 1);
                m_faces.push_back(Face(vertices, vertices));
                // m_faces.push_back(Triangle(s1 - 1, s2 - 1, s3 - 1));
            }
        }
    }
    
    // Construct bounding volume (cube)
    bool is_set = false;

    for (const glm::vec3 &vec: m_vertices) {
        if (!is_set) {
            is_set = true;
            min_point = vec;
            max_point = vec;
        }
        // Keep track of min and max (x, y, z) coordinates
        min_point.x = std::min(min_point.x, vec.x);
        min_point.y = std::min(min_point.y, vec.y);
        min_point.z = std::min(min_point.z, vec.z);

        max_point.x = std::max(max_point.x, vec.x);
        max_point.y = std::max(max_point.y, vec.y);
        max_point.z = std::max(max_point.z, vec.z);
    }

    const vec3 dim = max_point - min_point;
    bounding_box = new NonhierIrregularBox(min_point, dim);

    if (dim.x < EPSILON || dim.y < EPSILON || dim.z < EPSILON) {
        cout << "Skipped grid acceleration for " << fname << endl;
        skip_accel = true;
        return;
    }

    // Setup grid acceleration structure params
    grid_dim = GRID_DIM;
    cell_dim = dim / (float) grid_dim; 
    num_cells = std::pow(grid_dim, 3);

    for (int i = 0; i < num_cells; i++) {
        grid_vector.push_back(vector<Face>());
    }

    for (int i = 0; i < m_faces.size(); ++i) {
        Triangle &face_triangle = m_faces[i].vertices;
        Triangle &norm_triangle = type == ObjType::WithNormals ? m_faces[i].normals : face_triangle;

        vec3 &v0 = m_vertices[face_triangle.v1];
        vec3 &v1 = m_vertices[face_triangle.v2];
        vec3 &v2 = m_vertices[face_triangle.v3];
        vec3 min;
        vec3 max;
        min.x = std::min(v0.x, std::min(v1.x, v2.x));
        min.y = std::min(v0.y, std::min(v1.y, v2.y));
        min.z = std::min(v0.z, std::min(v1.z, v2.z));
        max.x = std::max(v0.x, std::max(v1.x, v2.x));
        max.y = std::max(v0.y, std::max(v1.y, v2.y));
        max.z = std::max(v0.z, std::max(v1.z, v2.z));

        int min_cell_x = glm::clamp((int) std::floor((min.x - min_point.x) / cell_dim.x), 0, grid_dim - 1);
        int min_cell_y = glm::clamp((int) std::floor((min.y - min_point.y) / cell_dim.y), 0, grid_dim - 1);
        int min_cell_z = glm::clamp((int) std::floor((min.z - min_point.z) / cell_dim.z), 0, grid_dim - 1);
        
        int max_cell_x = glm::clamp((int) std::floor((max.x - min_point.x) / cell_dim.x), 0, grid_dim - 1);
        int max_cell_y = glm::clamp((int) std::floor((max.y - min_point.y) / cell_dim.y), 0, grid_dim - 1);
        int max_cell_z = glm::clamp((int) std::floor((max.z - min_point.z) / cell_dim.z), 0, grid_dim - 1);

        for (int x = min_cell_x; x <= max_cell_x; ++x) {
            for (int y = min_cell_y; y <= max_cell_y; ++y) {
                for (int z = min_cell_z; z <= max_cell_z; ++z) {
                    grid_vector[x + grid_dim * (y + grid_dim * z)].push_back(Face(face_triangle, norm_triangle));
                }
            }
        }
    }
    int sum = 0;
    for (int i = 0; i < num_cells; i++) {
        sum += grid_vector[i].size();
    }

    cout << "sum: " << sum << endl;
    cout << "triangles: " << m_faces.size() << endl;
}

Mesh::~Mesh() {
    delete bounding_box;
}

Intersection Mesh::intersect(const Ray &ray) {
    const Intersection bounding_box_intersection = bounding_box->intersect(ray);

#ifdef RENDER_BOUNDING_VOLUMES
    // Make exception for 'plane' mesh
    if (!skip_accel) {
        return bounding_box_intersection;
    }
#endif

    // Ray does not intersect bounding volume, return early
    if (!bounding_box_intersection.is_hit()) {
        return bounding_box_intersection;
    }

    if (skip_accel) {
        Intersection intersection = Intersection::NonIntersection(ray);
        // Compute nearest intersection point with mesh
        for (const Face &face: m_faces) {
            const Triangle &triangle = face.vertices;
            vec3 &v0 = m_vertices[triangle.v1];
            vec3 &v1 = m_vertices[triangle.v2];
            vec3 &v2 = m_vertices[triangle.v3];

            const vec3 bary = intersect_ray_triangle(ray, v0, v1, v2);
            const float beta = bary.x;
            const float gamma = bary.y;
            const float t = bary.z;

            // Intersection point should not be behind ray origin
            if (t > 0 && beta >= 0 && gamma >= 0 && beta + gamma <= 1) {			
                if (!intersection.is_hit() || t < intersection.get_t()) {
                    const vec3 normal = cross(v1 - v0, v2 - v0);
                    intersection = Intersection(ray, normal, t, true);
                }
            }
        }
        return intersection;
    }

    vec3 bb_intersection = bounding_box_intersection.get_point();

    int x = glm::clamp((int) std::floor((bb_intersection.x - min_point.x) / cell_dim.x), 0, grid_dim - 1);
    int y = glm::clamp((int) std::floor((bb_intersection.y - min_point.y) / cell_dim.y), 0, grid_dim - 1);
    int z = glm::clamp((int) std::floor((bb_intersection.z - min_point.z) / cell_dim.z), 0, grid_dim - 1);
    
    vec3 delta_t;
    vec3 next_crossing_t;
    delta_t.x = (ray.direction.x < 0 ? -1 : 1) * cell_dim.x / ray.direction.x;
    delta_t.y = (ray.direction.y < 0 ? -1 : 1) * cell_dim.y / ray.direction.y;
    delta_t.z = (ray.direction.z < 0 ? -1 : 1) * cell_dim.z / ray.direction.z;

    int offset;
    offset = ray.direction.x < 0 ? 0 : 1;
    next_crossing_t.x = bounding_box_intersection.get_t() + ((x + offset) * cell_dim.x - (bb_intersection.x - min_point.x)) / ray.direction.x;
    offset = ray.direction.y < 0 ? 0 : 1;
    next_crossing_t.y = bounding_box_intersection.get_t() + ((y + offset) * cell_dim.y - (bb_intersection.y - min_point.y)) / ray.direction.y;
    offset = ray.direction.z < 0 ? 0 : 1;
    next_crossing_t.z = bounding_box_intersection.get_t() + ((z + offset) * cell_dim.z - (bb_intersection.z - min_point.z)) / ray.direction.z;
    
    int exit_x = ray.direction.x < 0 ? -1 : grid_dim;
    int exit_y = ray.direction.y < 0 ? -1 : grid_dim;
    int exit_z = ray.direction.z < 0 ? -1 : grid_dim;

    int step_x = ray.direction.x < 0 ? -1 : 1;
    int step_y = ray.direction.y < 0 ? -1 : 1;
    int step_z = ray.direction.z < 0 ? -1 : 1;

    Intersection intersection = Intersection::NonIntersection(ray);
    vector<Face> cur_cell;
    
    int cur;
    if (next_crossing_t.x < next_crossing_t.y) {
        if (next_crossing_t.x < next_crossing_t.z) {
            cur = 0;
        } else {
            cur = 2;
        }
    } else {
        if (next_crossing_t.y < next_crossing_t.z) {
            cur = 1;
        } else {
            cur = 2;
        }
    }

    while (true) {
        cur_cell = grid_vector[x + grid_dim * (y + grid_dim * z)];
        float cross_t;
        if (cur == 0) {
            cross_t = next_crossing_t.x;
        } else if (cur == 1) {
            cross_t = next_crossing_t.y;
        } else {
            cross_t = next_crossing_t.z;
        }

        for (Face &face: cur_cell) {
            vec3 &v0 = m_vertices[face.vertices.v1];
            vec3 &v1 = m_vertices[face.vertices.v2];
            vec3 &v2 = m_vertices[face.vertices.v3];

            const vec3 bary = intersect_ray_triangle(ray, v0, v1, v2);
            const float beta = bary.x;
            const float gamma = bary.y;
            const float alpha = 1.0f - beta - gamma;
            const float t = bary.z;

            // Intersection point should not be behind ray origin
            if (t > 0 && beta >= 0 && gamma >= 0 && beta + gamma <= 1 && t < cross_t) {			
                if (!intersection.is_hit() || t < intersection.get_t()) {
                    const vec3 normal = cross(v1 - v0, v2 - v0);
                    if (has_normals) {
                        vec3 &n0 = m_normals[face.normals.v1];
                        vec3 &n1 = m_normals[face.normals.v2];
                        vec3 &n2 = m_normals[face.normals.v3];

                        vec3 interp_normal = alpha * n0 + beta * n1 + gamma * n2;
                        if (dot(ray.direction, normal) * dot(ray.direction, interp_normal) < 0) {
                            interp_normal = -interp_normal;
                        }
                        intersection = Intersection(ray, interp_normal, t, true);
                    } else {
                        intersection = Intersection(ray, normal, t, true);
                    }
                }
            }
        }

        if (intersection.is_hit()) {
            break;
        }
        
        if (next_crossing_t.x < next_crossing_t.y) {
            if (next_crossing_t.x < next_crossing_t.z) {
                x += step_x;
                if (x == exit_x) {
                    break;
                }
                next_crossing_t.x = next_crossing_t.x + delta_t.x;
                cur = 0;
            } else {
                z += step_z;
                if (z == exit_z) {
                    break;
                }
                next_crossing_t.z = next_crossing_t.z + delta_t.z;
                cur = 2;
            }
        } else {
            if (next_crossing_t.y < next_crossing_t.z) {
                y += step_y;
                if (y == exit_y) {
                    break;
                }
                next_crossing_t.y = next_crossing_t.y + delta_t.y;
                cur = 1;
            } else {
                z += step_z;
                if (z == exit_z) {
                    break;
                }
                next_crossing_t.z = next_crossing_t.z + delta_t.z;
                cur = 2;
            }
        }
    }

    return intersection;
}

vec3 Mesh::intersect_ray_triangle(const Ray &ray, vec3 &P0, vec3 &P1, vec3 &P2) {
    const float R1 = ray.origin.x - P0.x;
    const float R2 = ray.origin.y - P0.y;
    const float R3 = ray.origin.z - P0.z;
    const float X1 = P1.x - P0.x;
    const float Y1 = P1.y - P0.y;
    const float Z1 = P1.z - P0.z;
    const float X2 = P2.x - P0.x;
    const float Y2 = P2.y - P0.y;
    const float Z2 = P2.z - P0.z;
    const float X3 = -ray.direction.x;
    const float Y3 = -ray.direction.y;
    const float Z3 = -ray.direction.z;

    float D = determinant(mat3(
        vec3(X1, Y1, Z1),
        vec3(X2, Y2, Z2),
        vec3(X3, Y3, Z3)
    ));
    float D1 = determinant(mat3(
        vec3(R1, R2, R3),
        vec3(X2, Y2, Z2),
        vec3(X3, Y3, Z3)
    ));
    float D2 = determinant(mat3(
        vec3(X1, Y1, Z1),
        vec3(R1, R2, R3),
        vec3(X3, Y3, Z3)
    ));
    float D3 = determinant(mat3(
        vec3(X1, Y1, Z1),
        vec3(X2, Y2, Z2),
        vec3(R1, R2, R3)
    ));

    return vec3(D1 / D, D2 / D, D3 / D);
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
      const MeshVertex& v = mesh.m_verts[idx];
      out << glm::to_string( v.m_position );
    if( mesh.m_have_norm ) {
        out << " / " << glm::to_string( v.m_normal );
    }
    if( mesh.m_have_uv ) {
        out << " / " << glm::to_string( v.m_uv );
    }
  }

*/
  out << "}";
  return out;
}
