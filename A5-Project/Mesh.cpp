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


Mesh::Mesh( const std::string& fname )
    : m_vertices()
    , m_faces()
{
    std::string code;
    double vx, vy, vz;
    size_t s1, s2, s3;

    std::ifstream ifs;

    is_plane = fname.find("plane.obj") != string::npos;
    ifs = std::ifstream(fname.c_str());
    if (!ifs.good()) {
        ifs = std::ifstream(("Assets/" + fname).c_str());
    }

    while( ifs >> code ) {
        if( code == "v" ) {
            ifs >> vx >> vy >> vz;
            m_vertices.push_back( glm::vec3( vx, vy, vz ) );
        } else if( code == "f" ) {
            ifs >> s1 >> s2 >> s3;
            m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
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
    
    // grid_dim = 28;

    float cubeRoot = pow(3 * m_faces.size() / (dim.x * dim.y * dim.z), 1 / 3.f); 
    resolution.x = std::floor(dim.x * cubeRoot); 
    resolution.x = std::max(1, std::min((int) resolution.x, 128));
    resolution.y = std::floor(dim.y * cubeRoot); 
    resolution.y = std::max(1, std::min((int) resolution.y, 128));  
    resolution.z = std::floor(dim.z * cubeRoot); 
    resolution.z = std::max(1, std::min((int) resolution.z, 128));  

    resolution = vec3(20, 20, 20);
    cellDimension = dim / vec3(resolution);
    num_cells = resolution.x * resolution.y * resolution.z;
    // cellDimension = dim / (float) grid_dim; 
    // num_cells = std::pow(grid_dim, 3);

    for (int i = 0; i < num_cells; i++) {
        myvector.push_back(vector<Triangle>());
    }

    for (Triangle &triangle: m_faces) {
        vec3 &v0 = m_vertices[triangle.v1];
        vec3 &v1 = m_vertices[triangle.v2];
        vec3 &v2 = m_vertices[triangle.v3];

        vec3 min;
        vec3 max;
        min.x = std::min(v0.x, std::min(v1.x, v2.x));
        min.y = std::min(v0.y, std::min(v1.y, v2.y));
        min.z = std::min(v0.z, std::min(v1.z, v2.z));
        max.x = std::max(v0.x, std::max(v1.x, v2.x));
        max.y = std::max(v0.y, std::max(v1.y, v2.y));
        max.z = std::max(v0.z, std::max(v1.z, v2.z));

        int min_cell_x = glm::clamp((int) std::floor((min.x - min_point.x) / cellDimension.x), 0, resolution.x - 1);
        int min_cell_y = glm::clamp((int) std::floor((min.y - min_point.y) / cellDimension.y), 0, resolution.y - 1);
        int min_cell_z = glm::clamp((int) std::floor((min.z - min_point.z) / cellDimension.z), 0, resolution.z - 1);
        
        int max_cell_x = glm::clamp((int) std::floor((max.x - min_point.x) / cellDimension.x), 0, resolution.x - 1);
        int max_cell_y = glm::clamp((int) std::floor((max.y - min_point.y) / cellDimension.y), 0, resolution.y - 1);
        int max_cell_z = glm::clamp((int) std::floor((max.z - min_point.z) / cellDimension.z), 0, resolution.z - 1);

        for (int x = min_cell_x; x <= max_cell_x; ++x) {
            for (int y = min_cell_y; y <= max_cell_y; ++y) {
                for (int z = min_cell_z; z <= max_cell_z; ++z) {
                    myvector[x + resolution.x * (y + resolution.z * z)].push_back(triangle);
                }
            }
        }
    }

    int sum = 0;
    for (int i = 0; i < num_cells; i++) {
        sum += myvector[i].size();
    }

    cout <<"sum: " << sum << endl;
    cout <<"triangles: " << m_faces.size() << endl;
}

Mesh::~Mesh() {
    delete bounding_box;
}

Intersection Mesh::intersect(const Ray &ray) {
    const Intersection bounding_box_intersection = bounding_box->intersect(ray);

#ifdef RENDER_BOUNDING_VOLUMES
    // Make exception for 'plane' mesh
    if (!is_plane) {
        return bounding_box_intersection;
    }
#endif

    // Ray does not intersect bounding volume, return early
    if (!bounding_box_intersection.is_hit()) {
        return bounding_box_intersection;
    }
    
    vec3 bb_intersection = bounding_box_intersection.get_point();

    int x = glm::clamp((int) std::floor((bb_intersection.x - min_point.x) / cellDimension.x), 0, resolution.x - 1);
    int y = glm::clamp((int) std::floor((bb_intersection.y - min_point.y) / cellDimension.y), 0, resolution.y - 1);
    int z = glm::clamp((int) std::floor((bb_intersection.z - min_point.z) / cellDimension.z), 0, resolution.z - 1);
    
    vec3 delta_t;
    vec3 next_crossing_t;
    delta_t.x = (ray.direction.x < 0 ? -1 : 1) * cellDimension.x / ray.direction.x;
    delta_t.y = (ray.direction.y < 0 ? -1 : 1) * cellDimension.y / ray.direction.y;
    delta_t.z = (ray.direction.z < 0 ? -1 : 1) * cellDimension.z / ray.direction.z;

    int offset;
    offset = ray.direction.x < 0 ? 0 : 1;
    next_crossing_t.x = bounding_box_intersection.get_t() + ((x + offset) * cellDimension.x - (bb_intersection.x - min_point.x)) / ray.direction.x;
    offset = ray.direction.y < 0 ? 0 : 1;
    next_crossing_t.y = bounding_box_intersection.get_t() + ((y + offset) * cellDimension.y - (bb_intersection.y - min_point.y)) / ray.direction.y;
    offset = ray.direction.z < 0 ? 0 : 1;
    next_crossing_t.z = bounding_box_intersection.get_t() + ((z + offset) * cellDimension.z - (bb_intersection.z - min_point.z)) / ray.direction.z;
    
    int exit_x = ray.direction.x < 0 ? -1 : resolution.x;
    int exit_y = ray.direction.y < 0 ? -1 : resolution.y;
    int exit_z = ray.direction.z < 0 ? -1 : resolution.z;

    int step_x = ray.direction.x < 0 ? -1 : 1;
    int step_y = ray.direction.y < 0 ? -1 : 1;
    int step_z = ray.direction.z < 0 ? -1 : 1;

    Intersection intersection = Intersection::NonIntersection(ray);
    vector<Triangle> cur_cell;
    
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
        cur_cell = myvector[x + resolution.x * (y + resolution.z * z)];
        float cross_t;
        if (cur == 0) {
            cross_t = next_crossing_t.x;
        } else if (cur == 1) {
            cross_t = next_crossing_t.y;
        } else {
            cross_t = next_crossing_t.z;
        }

        for (Triangle &triangle: cur_cell) {
            vec3 &v0 = m_vertices[triangle.v1];
            vec3 &v1 = m_vertices[triangle.v2];
            vec3 &v2 = m_vertices[triangle.v3];
            const vec3 bary = intersect_ray_triangle(ray, v0, v1, v2);
            const float beta = bary.x;
            const float gamma = bary.y;
            const float t = bary.z;

            // Intersection point should not be behind ray origin
            if (t > 0 && beta >= 0 && gamma >= 0 && beta + gamma <= 1 && t < cross_t) {			
                if (!intersection.is_hit() || t < intersection.get_t()) {
                    const vec3 normal = cross(v1 - v0, v2 - v0);
                    intersection = Intersection(ray, normal, t, true);
                }
            }
        }

        if (intersection.is_hit()) {
            break;
        }
        
        // cout << "next cross: " << next_crossing_t << endl;
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
    /*
    Intersection intersection = Intersection::NonIntersection(ray);
    // Compute nearest intersection point with mesh
    for (const Triangle &triangle: m_faces) {
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
    */
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
