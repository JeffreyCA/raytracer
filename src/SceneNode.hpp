#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>
#include "Intersection.hpp"
#include "Ray.hpp"

enum class NodeType {
    SceneNode,
    GeometryNode,
    JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);
    SceneNode(const SceneNode & other);
    virtual ~SceneNode();
    
    int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    void add_child(SceneNode* child);
    void remove_child(SceneNode* child);

    //-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);

    friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    std::list<SceneNode*> children;
    std::string m_name;
    NodeType m_nodeType;
    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;

    unsigned int m_nodeId;
    virtual Intersection intersect(const Ray &ray) const;

private:
    // The number of SceneNode instances.
    static unsigned int nodeInstanceCount;
};
