#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <ctime>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <tiny_obj_loader.h>

#include <graphics/objmodel.h>
#include <utils/math_utils.h>
#include <physics/bbox.h>

/* Data structure that represents a virtual object in the scene */
struct SceneObject 
{
    std::string name;
    size_t baseIndex;
    size_t numIndices;
    GLenum renderingMode;
    GLuint vertexArrayObjectId;
};

/* Class representing an object in the game scene */
class GameObject {
public:
    GameObject() : lastTranslation(Matrix_Identity()), lastMoveTime(0.0) {}
    GameObject(const AABB& aabb);

    /* Build a GameObject computing the AABB from a list of vertices */
    GameObject(const std::vector<glm::vec4>& vertices);

    /* Build a GameObject computing the AABB from an ObjModel */
    GameObject(const ObjModel& model);

    /* Build a GameObject computing the AABB from an ObjModel 
     * and a transformation matrix */
    GameObject(const ObjModel& model, const glm::mat4& transformation);

    void buildAABBFromModel(const ObjModel& model);

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

    GameObject operator=(const GameObject& other) {
        aabb = other.aabb;
        lastTranslation = other.lastTranslation;
        lastMoveTime = other.lastMoveTime;
        sceneObject = other.sceneObject;
        return *this;
    }

    AABB aabb;                 // Axis-Aligned Bounding Box of the object
    glm::mat4 lastTranslation; // Last translation matrix
    time_t lastMoveTime;       // Time of the last movement

    SceneObject sceneObject;  // SceneObject associated with the GameObject
};

#endif // GAMEOBJECT_H