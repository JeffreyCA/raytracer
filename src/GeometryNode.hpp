#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "Ray.hpp"

class GeometryNode : public SceneNode {
public:
    GeometryNode( const std::string & name, Primitive *prim, 
        Material *mat = nullptr );

    void setMaterial( Material *material );
    virtual Intersection intersect(const Ray &ray) const override;

    Material *m_material;
    Primitive *m_primitive;
};
