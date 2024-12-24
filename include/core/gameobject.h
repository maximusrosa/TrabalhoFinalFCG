#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <ctime>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tiny_obj_loader.h>

#include <utils/math_utils.h>
#include <graphics/objmodel.h>
#include <physics/bbox.h>

/* Data structure that represents a virtual object in the scene */
struct SceneObject 
{
    SceneObject() 
        : name(""), baseIndex(0), numIndices(0), renderingMode(GL_TRIANGLES), 
          vertexArrayObjectId(0) {}

    std::string name;
    size_t baseIndex;
    size_t numIndices;
    GLenum renderingMode;
    GLuint vertexArrayObjectId;
};

/* Class representing an object in the game scene */
class GameObject {
public:
    GameObject() 
        : lastMoveTime(0.0), lastTranslation(Matrix_Identity()) {}

    GameObject(const AABB& aabb)
        : aabb(aabb), lastMoveTime(0.0), 
          lastTranslation(glm::mat4(1.0f)) {}

    GameObject(const std::vector<glm::vec4>& vertices)
        : aabb(AABB::fromVertices(vertices)), lastMoveTime(0.0), 
          lastTranslation(Matrix_Identity()) {}

    GameObject(const ObjModel& model, const glm::mat4& transformation = Matrix_Identity())
        : aabb(AABB::fromModel(model)), lastMoveTime(0.0),
          lastTranslation(Matrix_Identity()) {
        aabb = aabb.transform(transformation);
    }

    GameObject(const ObjModel& model, const SceneObject& sceneObject, 
               const glm::mat4& transformation = Matrix_Identity())
        : aabb(AABB::fromModel(model)), lastMoveTime(0.0),
          lastTranslation(Matrix_Identity()), sceneObject(sceneObject) {
        aabb = aabb.transform(transformation);
    }

    GameObject(const GameObject& other)
        : aabb(other.aabb), lastTranslation(other.lastTranslation),
          lastMoveTime(other.lastMoveTime), sceneObject(other.sceneObject) {}

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

    SceneObject sceneObject;   // SceneObject associated with the GameObject
    AABB aabb;                 // Axis-Aligned Bounding Box of the object
    glm::mat4 lastTranslation; // Last translation matrix
    time_t lastMoveTime;       // Time of the last movement
};

#endif // GAMEOBJECT_H