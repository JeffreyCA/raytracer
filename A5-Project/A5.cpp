// Winter 2020

#include "A5.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/io.hpp>

#include "Ray.hpp"
#include "Mesh.hpp"
#include "PhongMaterial.hpp"

#include <future>
#include <thread>
#include <mutex>
#include <random>

using namespace std;
using namespace glm;

static const float EPSILON = 0.0001f;
static const float SHADOW_BIAS = 0.0001f;
static const int MAX_HIT_THRESHOLD = 2;
static const float d = 10.0f;
static const int BG_HUE = 213;

static random_device rd;
static default_random_engine mt(rd());
static uniform_real_distribution<float> dist(0.0f, 1.0f);

vec3 hsv_to_rgb(int H, float S, float V);
struct Context;
void A5_Render(SceneNode * root, Image & image, const vec3 &look_from, const vec3 &look_at, const vec3 &up, double fovy, const vec3 & ambient, const list<Light *> &lights);
vec3 trace(Context &context, uint x, uint y, float x_offset, float y_offset);
vec3 ray_colour(Context &context, const Ray &ray, uint x, uint y, int max_hits);
vec3 background_colour(Context &context, uint x, uint y);
Intersection hit(const Ray &ray, SceneNode *root);
float direct_light(SceneNode *root, const vec3 &inter_point, const vec3 &light_point);
Ray get_reflected_ray(const vec3 &inter_point, const vec3 &direction, const vec3 &normal);
vec3 compute_diffuse_specular(Context &context, const Intersection &intersection, PhongMaterial *mat);
vec3 compute_reflection(Context &context, const Ray &ray, const Intersection &intersection, PhongMaterial *mat, uint x, uint y, int max_hits);

struct Context {
    SceneNode *root;
    const mat4 &TSRT;
    const vec3 &look_from;
    const vec3 &ambient;
    const list<Light *> &lights;
    float nx;
    float ny;

    Context(SceneNode *root, const mat4 &TSRT, const vec3 &look_from, const vec3 &ambient, const list<Light *> &lights, float nx, float ny): root(root), TSRT(TSRT), look_from(look_from), ambient(ambient), lights(lights), nx(nx), ny(ny) {}
};

void A5_Render(
    // What to render  
    SceneNode *root,

    // Image to write to, set to a given width and height  
    Image &image,

    // Viewing parameters  
    const vec3 &look_from,
    const vec3 &look_at,
    const vec3 &up,
    double fovy,

    // Lighting parameters  
    const vec3 &ambient,
    const list<Light *> &lights
) {
  cout << "Calling A5_Render(\n" <<
          "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye (look from):  " << to_string(look_from) << endl <<
          "\t" << "view (look at): " << to_string(look_at) << endl <<
          "\t" << "up:   " << to_string(up) << endl <<
          "\t" << "fovy: " << fovy << endl <<
          "\t" << "ambient: " << to_string(ambient) << endl <<
          "\t" << "lights{" << endl;

    for(const Light * light : lights) {
        cout << "\t\t" <<  *light << endl;
    }
    cout << "\t}" << endl;
     cout <<")" << endl;

    const size_t nx = image.width();
    const size_t ny = image.height();
    const size_t pixels = nx * ny;

    const float w = nx / ny * 2 * d * tan(radians(fovy / 2.0f));
    const float h = 2 * d * tan(radians(fovy / 2.0f));

    const vec3 z_axis = normalize(look_at - look_from);
    const vec3 x_axis = normalize(cross(up, z_axis));
    const vec3 y_axis = cross(x_axis, z_axis);

    // Construct camera-to-world transformation matrix
    const mat4 T1 = translate(mat4(), vec3(-(float)nx / 2.0f, -(float)ny / 2.0f, d));
    const mat4 S2 = scale(mat4(), vec3(-h / ny, w / nx, 1.0f));
    const mat4 R3 = mat4(vec4(x_axis.x, x_axis.y, x_axis.z, 0),
                         vec4(y_axis.x, y_axis.y, y_axis.z, 0),
                         vec4(z_axis.x, z_axis.y, z_axis.z, 0),
                         vec4(0, 0, 0, 1));
    const mat4 T4 = mat4(vec4(1, 0, 0, 0),
                            vec4(0, 1, 0, 0),
                         vec4(0, 0, 1, 0),
                         vec4(look_from.x, look_from.y, look_from.z, 1));
    const mat4 TSRT = T4 * R3 * S2 * T1;
    // Use Context struct to hold all constant rendering information
    Context c(root, TSRT, look_from, ambient, lights, nx, ny);

#ifdef PARALLEL
    uint cores = thread::hardware_concurrency();
    vector<future<void>> futures;
    cout << endl;

    // Divide up work evenly for each core
    for (uint thread_id = 0; thread_id < cores; ++thread_id) {
        futures.push_back(async([](Context &c, Image &image, size_t nx, size_t ny, uint thread_id, uint cores) {
            const size_t pixels = nx * ny;
            uint prev_pct = 0;
            uint progress = 0;

            for (int j = thread_id; j < pixels; j += cores) {
                const uint x = j % ny;
                const uint y = j / ny;
                progress = (uint) ((float)y / ny * 100);
                
                // For simplicity, display progress for thread 0, and show multiples of 5 only
                if (thread_id == 0 && progress != prev_pct && progress % 5 == 0) {
                    prev_pct = progress;
                    cout << "\rProgress: " << progress << "%" << flush;
                }
                
                vec3 colour = trace(c, x, y, 0, 0);
                // colour += trace(c, x, y, -0.5f, -0.5f);
                // colour += trace(c, x, y, -0.5f, 0);
                // colour += trace(c, x, y, -0.5f, 0.5f);
                // colour += trace(c, x, y, 0, -0.5f);
                // colour += trace(c, x, y, 0, 0.5f);
                // colour += trace(c, x, y, 0.5f, -0.5f);
                // colour += trace(c, x, y, 0.5f, 0);
                // colour += trace(c, x, y, 0.5f, 0.5f);
                // colour /= 9.0f;
                image(x, y, 0) = (double)colour.x;
                image(x, y, 1) = (double)colour.y;
                image(x, y, 2) = (double)colour.z;
            }
        }, ref(c), ref(image), nx, ny, thread_id, cores));
    }

    // Wait until all jobs are finished
    for (auto &future : futures) {
        future.get();
    }
    cout << "\rProgress: 100%" << flush;
    cout << endl;
#else
    uint prev_pct = 0;
    uint progress = 0;
    cout << endl;

    // Process pixels in serial
    for (uint y = 0; y < ny; ++y) {
        progress = (uint) ((float)y / ny * 100);
        if (progress != prev_pct && progress % 5 == 0) {
            prev_pct = progress;
            cout << "\rProgress: " << progress << "%" << flush;
        }
        for (uint x = 0; x < nx; ++x) {
            vec3 colour = trace(c, x, y, 0, 0);
            colour += trace(c, x, y, -0.5f, -0.5f);
            colour += trace(c, x, y, -0.5f, 0);
            colour += trace(c, x, y, -0.5f, 0.5f);
            colour += trace(c, x, y, 0, -0.5f);
            colour += trace(c, x, y, 0, 0.5f);
            colour += trace(c, x, y, 0.5f, -0.5f);
            colour += trace(c, x, y, 0.5f, 0);
            colour += trace(c, x, y, 0.5f, 0.5f);
            colour /= 9.0f;
            image(x, y, 0) = (double)colour.x;
            image(x, y, 1) = (double)colour.y;
            image(x, y, 2) = (double)colour.z;
        }
    }
    cout << "\rProgress: 100%" << flush;
    cout << endl;
#endif
}

/*
 * Compute the colour at (x + offset, y + offset).
 */
vec3 trace(Context &context, uint x, uint y, float x_offset, float y_offset) {
    SceneNode *root = context.root;
    const mat4 &TSRT = context.TSRT;
    const vec3 &look_from = context.look_from;
    const vec3 &ambient = context.ambient;
    const list<Light *> &lights = context.lights;
    const vec4 world = TSRT * vec4(x + x_offset, y + y_offset, 0, 1);
    const Ray ray = Ray(look_from, vec3(world) - look_from);

    return ray_colour(context, ray, x, y, 0);
}

/*
 * Compute the colour at (x, y) by casting ray from eye to the point.
 */
vec3 ray_colour(Context &context, const Ray &ray, uint x, uint y, int max_hits) {
    const Intersection intersection = hit(ray, context.root);

    if (intersection.is_hit()) {
        PhongMaterial *material = intersection.get_material();
        vec3 colour = material->m_kd * context.ambient;
        // Add diffuse and specular components
        colour += compute_diffuse_specular(context, intersection, material); 
        // Add mirror reflection component
        colour += compute_reflection(context, ray, intersection, material, x, y, max_hits);
        return colour;
    } else if (max_hits > 0) {
        // Currently processing reflection ray, do not return bg colour or else we get some of the background colour throughout entire scene.
        return vec3(0, 0, 0);
    } else {
        return background_colour(context, x, y);
    }
}

/*
 * HSV to RGB conversion.
 * https://gist.github.com/kuathadianto/200148f53616cbd226d993b400214a7f
 */
vec3 hsv_to_rgb(int H, float S, float V) {
    const float C = S * V;
    const float X = C * (1 - abs(fmod(H / 60.0f, 2) - 1));
    const float m = V - C;
    float Rs, Gs, Bs;

    if (H >= 0 && H < 60) {
        Rs = C;
        Gs = X;
        Bs = 0;	
    } else if (H >= 60 && H < 120) {	
        Rs = X;
        Gs = C;
        Bs = 0;	
    } else if (H >= 120 && H < 180) {
        Rs = 0;
        Gs = C;
        Bs = X;	
    } else if (H >= 180 && H < 240) {
        Rs = 0;
        Gs = X;
        Bs = C;	
    } else if(H >= 240 && H < 300) {
        Rs = X;
        Gs = 0;
        Bs = C;	
    } else {
        Rs = C;
        Gs = 0;
        Bs = X;	
    }

    return vec3(Rs + m, Gs + m, Bs + m);
}

/*
 * Compute background colour at (x, y) by interpolating the saturation component of a blueish hue.
 */
vec3 background_colour(Context &context, uint x, uint y) {
    const float s = 1.0f - (float)y / context.ny;
    return hsv_to_rgb(BG_HUE, s, 1.0f);
}

/*
 * Compute intersection (if any) of the ray with the objects in the scene.
 */
Intersection hit(const Ray &ray, SceneNode *root) {
    const Intersection intersection = root->intersect(ray);
    return intersection;
}

/*
 * Takes in a shadow ray and location of a light source and returns 0 if an object is in the way, otherwise 1.
 */
float direct_light(SceneNode *root, const vec3 &inter_point, const vec3 &light_point) {
    const Ray ray = Ray(inter_point, light_point - inter_point);
    const Intersection intersection = root->intersect(ray);
    
    if (intersection.is_hit()) {
        return 0;
    } else {
        return 1;
    }
}

/*
 * Get reflected ray using 'ggReflection' formula.
 */
Ray get_reflected_ray(const vec3 &inter_point, const vec3 &direction, const vec3 &normal) {
    return Ray(inter_point, direction - 2 * dot(direction, normal) * normal);
}

Ray get_perturbed_ray(const Ray &ray, float shininess) {
    vec3 direction = ray.direction;
    vec3 U, V;

    // Get the basis vectors for the square
    // Get the basis vectors for the square of size <glossiness>x<glossiness>
    if(std::abs(direction[2]) > std::abs(direction[0]) && std::abs(direction[2]) > std::abs(direction[1])) {
        U = glm::vec3(-direction[1], direction[0], 0.0);
    }
    else if(std::abs(direction[1]) > std::abs(direction[0])) {
        U = glm::vec3(-direction[2], 0.0, direction[0]);
    }
    else{
        U = glm::vec3(0.0, -direction[2], direction[1]);
    }
    U = glm::normalize(U);
    V = glm::normalize(glm::cross(direction, U));
    
    float a = 1.0f / (1.0f + (float) shininess);
    float i = -a/2.0f + dist(mt) * a;
    float j = -a/2.0f + dist(mt) * a;

    vec3 glossy_direction = ray.direction + i * U + j * V;
    return Ray(ray.origin, glossy_direction);
}

/*
 * Return the diffuse and specular colour components at the intersection point with the object material.
 */
vec3 compute_diffuse_specular(Context &context, const Intersection &intersection, PhongMaterial *mat) {
    const vec3 normal = intersection.get_N();
    const vec3 point = intersection.get_point();
    vec3 colour = vec3(0, 0, 0);

    for (const Light *light: context.lights) {
        const vec3 light_pos = light->position;
        Ray ray = Ray(point, light_pos - point);
        vec3 point_light_direction = ray.get_normalized_direction();
        float n_light_angle = dot(normal, point_light_direction);
        
        if (n_light_angle > 0) {
            // Move intersection point up along normal to avoid shadow acne
            const vec3 biased_p = point + SHADOW_BIAS * normal;
            const float dist = fabs(distance(biased_p, light_pos));
            const float shadow_contribution = direct_light(context.root, biased_p, light_pos);
            // Compute light attenuation using light source's quadratic attenuation parameters
            const float light_attenuation = std::min(1.0, 
                1.0 / (light->falloff[0] + light->falloff[1] * dist + light->falloff[2] * pow(dist, 2)));

            const vec3 V = normalize(context.look_from - point);
            const vec3 L = normalize(light_pos - point);
            // Blinn-Phong
            const vec3 H = normalize(L + V);
            const float n_h_angle = dot(normal, H);
            const float phong = std::max(0.0f, (float) std::pow(n_h_angle, mat->m_shininess));

            // Only add colour components if no object is in the way between light source and intersection point
            if (shadow_contribution > 0.0f) {
                // Diffuse
                colour += light_attenuation * n_light_angle * light->colour * mat->m_kd;
                // Specular
                colour += phong * light->colour * mat->m_ks;
            }
        }
    }
    return colour;
}

/*
 * Compute mirror reflection component by recursively casting reflection rays into the scene.
 */
vec3 compute_reflection(Context &context, const Ray &ray, const Intersection &intersection, PhongMaterial *mat, uint x, uint y, int max_hits) {
    vec3 colour = vec3(0, 0, 0);

    if (max_hits < MAX_HIT_THRESHOLD) {
        const vec3 biased_p = intersection.get_point() + SHADOW_BIAS * intersection.get_N();
        const Ray reflected_ray = get_reflected_ray(biased_p, ray.direction, intersection.get_N());
        int ray_samples = 10;

        while (ray_samples > 0) {
            const Ray perturbed = get_perturbed_ray(reflected_ray, mat->m_shininess);
            colour += context.ambient * mat->m_ks * ray_colour(context, perturbed, x, y, max_hits + 1);
            ray_samples--;
        }

        colour /= 10.0f;
    }

    return colour;
}
