// Winter 2020

#include "GeometryNode.hpp"
#include <iostream>
using namespace std;
using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
    const std::string & name, Primitive *prim, Material *mat )
    : SceneNode( name )
    , m_material( mat )
    , m_primitive( prim )
{
    m_nodeType = NodeType::GeometryNode;
}

Intersection GeometryNode::intersect(const Ray &ray) const {
    const vec3 local_origin = vec3(invtrans * vec4(ray.origin, 1.0f));
    const vec3 local_dir = vec3(invtrans * vec4(ray.direction, 0.0f));
    const Ray local_ray = Ray(local_origin, local_dir);

    Intersection intersection = m_primitive->intersect(local_ray);
    intersection.set_material(static_cast<PhongMaterial *>(m_material));

    if (intersection.is_hit()) {
        intersection.convert_to_world(trans, invtrans);
    }
    return intersection;
}

void GeometryNode::setMaterial( Material *mat )
{
    // Obviously, there's a potential memory leak here.  A good solution
    // would be to use some kind of reference counting, as in the 
    // C++ shared_ptr.  But I'm going to punt on that problem here.
    // Why?  Two reasons:
    // (a) In practice we expect the scene to be constructed exactly
    //     once.  There's no reason to believe that materials will be
    //     repeatedly overwritten in a GeometryNode.
    // (b) A ray tracer is a program in which you compute once, and 
    //     throw away all your data.  A memory leak won't build up and
    //     crash the program.

    m_material = mat;
}
