#include "Raytrace.hpp"

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
#include <random>

using namespace std;
using namespace glm;

static const bool PARALLEL = true;
static const bool ADAPTIVE_SS = true; // If false, supersampling is disabled
static const bool ADAPTIVE_SS_SHOW_PIXELS = true;

static const float ADAPTIVE_SS_DIM_FACTOR = 0.6f;
static const float ADAPTIVE_SS_HALF_LIMIT = 0.0625f;
static const float ADAPTIVE_SS_COLOUR_DIFF_THRESHOLD = 100;

static const float SHADOW_BIAS = 0.005f;
static const int GLOSSY_REFLECTION_RAYS = 50;
static const int GLOSSY_REFRACTION_RAYS = 30;
static const int MAX_HIT_THRESHOLD = 3;
static const float d = 10.0f;

static const vec3 ZERO_VEC = vec3(0.0f);
static const vec3 ONE_VEC = vec3(1, 1, 1);
static const vec3 SKY_BLUE = vec3(0, 161, 254) / 255.0f;

static random_device rd;
static default_random_engine mt(rd());
static uniform_real_distribution<float> dist(0.0f, 1.0f);

vec3 hsv_to_rgb(int H, float S, float V);
struct Context;
void render(SceneNode *root, Image & image, const vec3 &look_from, const vec3 &look_at, const vec3 &up, double fovy, const vec3 & ambient, const list<Light *> &lights);
double colour_dist(const vec3 &e1, const vec3 &e2);
vec3 trace(Context &context, float x, float y);
vec3 supersample(Context &context, const vec3 &centre, float x, float y, float half_size);
vec3 ray_colour(Context &context, const Ray &ray, uint x, uint y, int max_hits);
vec3 background_colour(Context &context, const Ray &ray);
Intersection hit(const Ray &ray, SceneNode *root);
bool is_light_path_blocked(SceneNode *root, const vec3 &inter_point, const vec3 &light_point);
Ray get_reflected_ray(const vec3 &inter_point, const vec3 &direction, const vec3 &normal);
vec3 compute_diffuse_specular(Context &context, const Ray &ray, const Intersection &intersection, PhongMaterial *mat);
float compute_reflection_coeff(const vec3 &ray_direction, const vec3 &intersection_normal, PhongMaterial *mat);
vec3 compute_reflection(Context &context, const Ray &ray, const Intersection &intersection, PhongMaterial *mat, uint x, uint y, int max_hits);
vec3 compute_refraction(Context &context, const Ray &ray, const Intersection &intersection, PhongMaterial *mat, uint x, uint y, int max_hits);

struct Context {
    SceneNode *root;
    const mat4 &TSRT;
    const vec3 &look_from;
    const vec3 &ambient;
    const list<Light *> &lights;
    float nx;
    float ny;
    double fov;

    Context(SceneNode *root, const mat4 &TSRT, const vec3 &look_from, const vec3 &ambient, const list<Light *> &lights, float nx, float ny, double fov): root(root), TSRT(TSRT), look_from(look_from), ambient(ambient), lights(lights), nx(nx), ny(ny), fov(fov) {}
};

// https://www.compuphase.com/cmetric.htm
double colour_dist(const vec3 &e1, const vec3 &e2) {
    const vec3 e1_255 = 255.0f * e1;
    const vec3 e2_255 = 255.0f * e2;
    const long rmean = ((long) e1_255.r + (long) e2_255.r) / 2;
    const long r = (long)e1_255.r - (long)e2_255.r;
    const long g = (long)e1_255.g - (long)e2_255.g;
    const long b = (long)e1_255.b - (long)e2_255.b;
    return std::sqrt((((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8));
}

void render(
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
  cout << "Calling render(\n" <<
          "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye (look from):  " << look_from << endl <<
          "\t" << "view (look at): " << look_at << endl <<
          "\t" << "up:   " << up << endl <<
          "\t" << "fovy: " << fovy << endl <<
          "\t" << "ambient: " << ambient << endl <<
          "\t" << "lights{" << endl;

    for (const Light * light : lights) {
        cout << "\t\t" <<  *light << endl;
    }
    cout << "\t}" << endl;
    cout <<")" << endl;

    const size_t nx = image.width();
    const size_t ny = image.height();

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
    Context c(root, TSRT, look_from, ambient, lights, nx, ny, fovy);

    if (PARALLEL) {
        uint cores = thread::hardware_concurrency();
        vector<future<void>> futures;
        cout << endl;

        // Divide up work evenly for each core
        for (uint thread_id = 0; thread_id < cores; ++thread_id) {
            futures.push_back(async([](Context &c, Image &image, size_t nx, size_t ny, uint thread_id, uint cores) {
                const size_t pixels = nx * ny;
                uint prev_pct = 0;
                uint progress = 0;

                for (size_t j = thread_id; j < pixels; j += cores) {
                    const uint x = j % ny;
                    const uint y = j / ny;
                    progress = (uint) ((float)y / ny * 100);
                    
                    // For simplicity, display progress for thread 0, and show multiples of 5 only
                    if (thread_id == 0 && progress != prev_pct && progress % 5 == 0) {
                        prev_pct = progress;
                        cout << "\rProgress: " << progress << "%" << flush;
                    }

                    vec3 colour;
                    if (ADAPTIVE_SS) {
                        const vec3 centre_colour = trace(c, x, y);
                        colour = supersample(c, centre_colour, x, y, 1.0f);
                    } else {
                        colour = trace(c, x, y);
                    }

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
    } else {
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
                vec3 colour;
                if (ADAPTIVE_SS) {
                    const vec3 centre_colour = trace(c, x, y);
                    colour = supersample(c, centre_colour, x, y, 0.5f);
                } else {
                    colour = trace(c, x, y);
                }

                image(x, y, 0) = (double)colour.x;
                image(x, y, 1) = (double)colour.y;
                image(x, y, 2) = (double)colour.z;
            }
        }
        cout << "\rProgress: 100%" << flush;
        cout << endl;
    }
}

vec3 trace(Context &context, float x, float y) {
    const mat4 &TSRT = context.TSRT;
    const vec3 &look_from = context.look_from;
    const vec4 world = TSRT * vec4(x, y, 0, 1);
    const Ray ray = Ray(look_from, vec3(world) - look_from);
    return ray_colour(context, ray, x, y, 0);
}

/*
 * Adaptive supersampling technique where at least 4 rays are cast to each pixel.
 * The pixel is then divided into four quadrants, where four additional rays are cast
 * to their centres. If the colour difference between the results of the additional rays
 * and their average colour is large enough, then the quadrants are subdivided and
 * the process repeats up until a threshold.
 * 
 * Compile with SHOW_ADAPTIVE to render a dimmed scene with pixels that were further supersampled coloured
 * as red.
 */
vec3 supersample(Context &context, const vec3 &centre, float x, float y, float half_size) {
    if (half_size <= ADAPTIVE_SS_HALF_LIMIT) {
        return centre;
    }

    const float quarter = half_size / 2.0f;
    vec3 top_left = trace(context, x - quarter, y - quarter);
    vec3 top_right = trace(context, x + quarter, y - quarter);
    vec3 bottom_right = trace(context, x + quarter, y + quarter);
    vec3 bottom_left = trace(context, x - quarter, y + quarter);

    bool ss = false;

    if (colour_dist(top_left, centre) > ADAPTIVE_SS_COLOUR_DIFF_THRESHOLD) {
        top_left = supersample(context, top_left, x - quarter, y - quarter, half_size / 2.0f);
        ss = true;
    }
    if (colour_dist(top_right, centre) > ADAPTIVE_SS_COLOUR_DIFF_THRESHOLD) {
        top_right = supersample(context, top_right, x + quarter, y - quarter, half_size / 2.0f);
        ss = true;
    }
    if (colour_dist(bottom_right, centre) > ADAPTIVE_SS_COLOUR_DIFF_THRESHOLD) {
        bottom_right = supersample(context, bottom_right, x + quarter, y + quarter, half_size / 2.0f);
        ss = true;
    }
    if (colour_dist(bottom_left, centre) > ADAPTIVE_SS_COLOUR_DIFF_THRESHOLD) {
        bottom_left = supersample(context, bottom_left, x - quarter, y + quarter, half_size / 2.0f);
        ss = true;
    }

    if (ADAPTIVE_SS_SHOW_PIXELS) {
        if (ss) {
            return vec3(1, 0, 0);
        }
        return ADAPTIVE_SS_DIM_FACTOR * (top_left + top_right + bottom_right + bottom_left + centre) / 5.0f;
    } else {
        return (top_left + top_right + bottom_right + bottom_left + centre) / 5.0f;
    }
}

/*
 * Compute the colour at (x, y) by casting ray from eye to the point.
 */
vec3 ray_colour(Context &context, const Ray &ray, uint x, uint y, int max_hits) {
    const Intersection intersection = hit(ray, context.root);

    if (intersection.is_hit()) {
        PhongMaterial *material = intersection.get_material();
        vec3 colour;
        
        if (material->m_textured) {
            colour = material->m_texture->get_colour(intersection.get_u(), intersection.get_v()) * context.ambient;
        } else {
            colour = material->m_kd * context.ambient;
        }

        // Add diffuse and specular components
        if (material->m_kd != ZERO_VEC || material->m_textured) {
            colour += compute_diffuse_specular(context, ray, intersection, material); 
        }

        const float reflection_coeff = compute_reflection_coeff(ray.direction, intersection.get_N(), material);

        if (!material->m_opaque && reflection_coeff < 1.0f) {
            colour += (1.0f - reflection_coeff) * compute_refraction(context, ray, intersection, material, x, y, max_hits);
            colour += reflection_coeff * compute_reflection(context, ray, intersection, material, x, y, max_hits);
        }

        // Add mirror reflection component
        if (material->m_opaque && material->m_mirror) {
            // Not sure why it works if reflection coefficient is inverted???
            colour += glm::clamp(1.0f - reflection_coeff, 0.9f, 1.0f) * compute_reflection(context, ray, intersection, material, x, y, max_hits);
        }
        return colour;
    } else {
        return background_colour(context, ray);
    }
}

/*
 * Compute background colour based on ray direction. Clamped to FOV of scene.
 */
vec3 background_colour(Context &context, const Ray &ray) {
    static const float cos_fov = cos(radians(context.fov));
    const float cosine = glm::clamp(normalize(ray.direction).y, -cos_fov, cos_fov);
    const float s = cosine / cos_fov + 0.5f;
    return (1 - s) * ONE_VEC + s * SKY_BLUE;
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
bool is_light_path_blocked(SceneNode *root, const vec3 &inter_point, const vec3 &light_point) {
    const Ray ray = Ray(inter_point, light_point - inter_point);
    const Intersection intersection = root->intersect(ray);
    return intersection.is_hit() && intersection.get_t() < 1.0f && intersection.get_material()->m_opaque;
}

/*
 * Get reflected ray using 'ggReflection' formula.
 */
Ray get_reflected_ray(const vec3 &inter_point, const vec3 &direction, const vec3 &normal) {
    return Ray(inter_point, direction - 2 * dot(direction, normal) * normal);
}

Ray get_perturbed_ray(const Ray &ray, const float glossiness) {
    const vec3 direction = ray.direction;
    const vec3 w = normalize(direction);
    const vec3 w_abs = vec3(fabs(w.x), fabs(w.y), fabs(w.z));
    
    // Choose any t not collinear with w
    vec3 t;
    if (w_abs.x <= w_abs.y && w_abs.x <= w_abs.z) {
        t = vec3(1.0f, w.y, w.z);
    } else if (w_abs.y <= w_abs.x && w_abs.y <= w_abs.z) {
        t = vec3(w.x, 1.0f, w.z);
    } else {
        t = vec3(w.x, w.y, 1.0f);
    }

    const vec3 u = normalize(cross(t, w));
    const vec3 v = cross(w, u);

    const float i = -glossiness / 2.0f + dist(mt) * glossiness;
    const float j = -glossiness / 2.0f + dist(mt) * glossiness;

    const vec3 glossy_direction = ray.direction + i * u + j * v;
    return Ray(ray.origin, glossy_direction);
}

/*
 * Return the diffuse and specular colour components at the intersection point with the object material.
 */
vec3 compute_diffuse_specular(Context &context, const Ray &ray, const Intersection &intersection, PhongMaterial *mat) {
    const vec3 normal = dot(ray.direction, intersection.get_N()) > 0 ? -intersection.get_N() : intersection.get_N();
    const vec3 point = intersection.get_point();
    vec3 colour = ZERO_VEC;

    for (const Light *light: context.lights) {
        const vec3 light_pos = light->position;
        // Move intersection point up along normal to avoid shadow acne
        const vec3 biased_p = point + SHADOW_BIAS * normal;
        const bool direct_light_path = !is_light_path_blocked(context.root, biased_p, light_pos);
        // Only add colour components if no object is in the way between light source and intersection point
        if (direct_light_path) {
            // Blinn-Phong diffuse
            const Ray ray = Ray(point, light_pos - point);
            const vec3 point_light_direction = ray.get_normalized_direction();
            const float diffuse_term = std::max(0.0f, dot(normal, point_light_direction));

            // Compute light attenuation using light source's quadratic attenuation parameters
            const float dist = fabs(distance(biased_p, light_pos));
            const float light_attenuation = 
                std::min(1.0, 1.0 / (light->falloff[0] + 
                                     light->falloff[1] * dist +
                                     light->falloff[2] * dist * dist));

            // Blinn-Phong specular
            const vec3 V = normalize(context.look_from - point);
            const vec3 L = normalize(light_pos - point);
            const vec3 H = normalize(L + V);
            const float specular_term = std::max(0.0f, (float) std::pow(dot(normal, H), mat->m_shininess));

            // Diffuse
            vec3 diffuse_colour;
            if (mat->m_textured) {
                diffuse_colour = mat->m_texture->get_colour(intersection.get_u(), intersection.get_v());
            } else {
                diffuse_colour = mat->m_kd;
            }

            if (mat->m_ignore_lighting) {
                colour += diffuse_colour;
            } else {
                colour += diffuse_colour * light->colour * diffuse_term * light_attenuation;
                colour += mat->m_ks * light->colour * specular_term;
            }
        }
    }
    return colour;
}

/*
 * Compute mirror reflection component by recursively casting reflection rays into the scene.
 */
vec3 compute_reflection(Context &context, const Ray &ray, const Intersection &intersection, PhongMaterial *mat, uint x, uint y, int max_hits) {
    if (max_hits >= MAX_HIT_THRESHOLD) {
        return ZERO_VEC;
    }

    vec3 colour = ZERO_VEC;

    const vec3 biased_p = intersection.get_point() + SHADOW_BIAS * intersection.get_N();
    const Ray reflected_ray = get_reflected_ray(biased_p, ray.direction, intersection.get_N());

    if (mat->m_mirror && mat->m_glossiness > 0) {
        int ray_samples = GLOSSY_REFLECTION_RAYS;
        while (ray_samples > 0) {
            const Ray perturbed_ray = get_perturbed_ray(reflected_ray, mat->m_glossiness);
            colour += mat->m_ks * ray_colour(context, perturbed_ray, x, y, max_hits + 1);
            ray_samples--;
        }
        colour /= GLOSSY_REFLECTION_RAYS;
    } else {
        colour += mat->m_ks * ray_colour(context, reflected_ray, x, y, max_hits + 1);
    }
    
    return colour;
}

float compute_reflection_coeff(const vec3 &ray_direction, const vec3 &intersection_normal, PhongMaterial *mat) {
    vec3 normal = intersection_normal;
    float eta_1 = 1.0f;
    float eta_2 = mat->m_refractive_index;
    float cosine = dot(normalize(ray_direction), normal);
    
    if (cosine > 0) {
        normal = -normal;
        eta_1 = eta_2;
        eta_2 = 1.0f;
    } else {
        cosine = -cosine;
    }

    // Otherwise material is translucent
    const float eta_ratio = eta_1 / eta_2;
    const float sqrt_term = 1.0f - ((eta_ratio * eta_ratio) * (1.0f - (cosine * cosine)));

    if (sqrt_term < 0.0f) {
        // Total internal reflection
        return 1.0f;
    }

    const float cosine2 = sqrt(sqrt_term);
    const float R_perp = pow(((eta_1 * cosine - eta_2 * cosine2) / (eta_1 * cosine + eta_2 * cosine2)), 2);
    const float R_parallel = pow(((eta_2 * cosine - eta_1 * cosine2) / (eta_2 * cosine + eta_1 * cosine2)), 2);
	return (R_perp + R_parallel) / 2.0f;
}

vec3 compute_refraction(Context &context, const Ray &ray, const Intersection &intersection, PhongMaterial *mat, uint x, uint y, int max_hits) {
    if (max_hits >= MAX_HIT_THRESHOLD) {
        return ZERO_VEC;
    }
    const vec3 normalized_ray_dir = normalize(ray.direction);
    vec3 normal = intersection.get_N();

    float eta_1 = 1.0f;
    float eta_2 = mat->m_refractive_index;
    float cosine = dot(normalized_ray_dir, normal);
    
    if (cosine > 0) {
        normal = -normal;
        eta_1 = eta_2;
        eta_2 = 1.0f;
    } else {
        cosine = -cosine;
    }

    const float eta_ratio = eta_1 / eta_2;
    const float sqrt_term = 1.0f - ((eta_ratio * eta_ratio) * (1.0f - (cosine * cosine)));
    
    assert(sqrt_term >= 0.0f);
    const vec3 point = intersection.get_point();
    const vec3 direction = eta_ratio * normalized_ray_dir + (eta_ratio * cosine - sqrt(sqrt_term)) * normal;
    const vec3 origin = point + SHADOW_BIAS * direction;
    const Ray refracted_ray = Ray(origin, direction);

    vec3 colour = ZERO_VEC;
    if (mat->m_glossiness > 0) {
        const float glossiness = 1.0f / (1.0f - mat->m_glossiness);
        int ray_samples = GLOSSY_REFRACTION_RAYS;

        while (ray_samples > 0) {
            const Ray perturbed_ray = get_perturbed_ray(refracted_ray, glossiness);
            colour += mat->m_ks * ray_colour(context, perturbed_ray, x, y, max_hits + 1);
            ray_samples--;
        }
        colour /= GLOSSY_REFRACTION_RAYS;
    } else {
        colour += mat->m_ks * ray_colour(context, refracted_ray, x, y, max_hits + 1);
    }
    return colour;
}
