#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <ctime>

#include <glm/glm.hpp>
#include <tiny_obj_loader.h>
#include "graphics/objmodel.h"
#include "utils/math_utils.h"
#include "physics/bbox.h"

/* Class representing an object in the game scene */
class GameObject {
public:
    GameObject(const AABB& aabb);

    /* Build a GameObject computing the AABB from a list of vertices */
    GameObject(const std::vector<glm::vec4>& vertices);

    /* Build a GameObject computing the AABB from an ObjModel */
    GameObject(const ObjModel& model);

    /* Build a GameObject computing the AABB from an ObjModel 
     * and a transformation matrix */
    GameObject(const ObjModel& model, const glm::mat4& transformation);

    void rotate(float angle, const glm::vec4& axis);
    void translate(float tx, float ty, float tz);
    void scale(float sx, float sy, float sz);
    void transform(const glm::mat4& transformation);

    // Check if a point (in homogeneous coordinates) is inside the GameObject
    bool contains(const glm::vec4& point) const;

    // Check if this GameObject intersects another GameObject
    bool intersects(const GameObject& other) const;

    // Update the last movement time
    void updateMoveTime();

    AABB aabb;                 // Axis-Aligned Bounding Box of the object
    glm::mat4 lastTranslation; // Last translation matrix
    time_t lastMoveTime;       // Time of the last movement
};

#endif // GAMEOBJECT_H