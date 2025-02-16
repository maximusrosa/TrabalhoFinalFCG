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

#include "tiny_obj_loader.h"

#include "utils/math_utils.h"
#include "graphics/objmodel.h"
#include "physics/bounding.h"

/* Data structure that represents a virtual object in the scene */
struct SceneObject {
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
        : lastMoveTime(0.0), lastMove(0.0), useBSphere(false) {}

    GameObject(const AABB& aabb, bool useBSphere=false)
        : aabb(aabb), lastMoveTime(0.0), lastMove(0.0), 
          bsphere(aabb), useBSphere(useBSphere) {}

    GameObject(const AABB& aabb, const BSphere& bsphere, bool useBSphere=false)
        : aabb(aabb), lastMoveTime(0.0), lastMove(0.0), 
          bsphere(bsphere), useBSphere(useBSphere) {}

    GameObject(const std::vector<glm::vec4>& vertices, bool useBSphere=false)
        : aabb(vertices), lastMoveTime(0.0), lastMove(0.0), 
          bsphere(vertices), useBSphere(useBSphere) {}

    GameObject(const ObjModel& model, bool useBSphere=false)
        : aabb(model), lastMoveTime(0.0), lastMove(0.0), 
          bsphere(model), useBSphere(useBSphere) {}

    GameObject(const ObjModel& model, const glm::mat4& transformation, bool useBSphere=false)
        : aabb(model, transformation), lastMoveTime(0.0), lastMove(0.0), 
          bsphere(model, transformation), useBSphere(useBSphere) {}

    GameObject(const ObjModel& model, const SceneObject& sceneObject, bool useBSphere=false)
        : aabb(model), lastMoveTime(0.0), lastMove(0.0), sceneObject(sceneObject), 
          bsphere(model), useBSphere(useBSphere) {}

    GameObject(const ObjModel& model, const SceneObject& sceneObject, 
               const glm::mat4& transformation, bool useBSphere=false)
        : aabb(model, transformation), lastMoveTime(0.0), lastMove(0.0), sceneObject(sceneObject), 
          bsphere(model), useBSphere(useBSphere) {}

    GameObject(const GameObject& other)
        : aabb(other.aabb), lastMoveTime(other.lastMoveTime), 
          lastMove(other.lastMove), sceneObject(other.sceneObject), 
          bsphere(other.bsphere), useBSphere(other.useBSphere) {}

    // Getters
    AABB getAABB() const { return aabb; }
    BSphere getBSphere() const { return bsphere; }
    glm::vec3 getLastMove() const { return lastMove; }
    time_t getLastMoveTime() const { return lastMoveTime; }
    SceneObject getSceneObject() const { return sceneObject; }
    bool getUseBSphere() const { return useBSphere; }

    // Setters
    void setAABB(const AABB& aabb) { this->aabb = aabb; }
    void setBSphere(const BSphere& bsphere) { this->bsphere = bsphere; }
    void setLastMove(const glm::vec3& lastMove) { this->lastMove = lastMove; }
    void setLastMoveTime(time_t lastMoveTime) { this->lastMoveTime = lastMoveTime; }
    void setSceneObject(const SceneObject& sceneObject) { this->sceneObject = sceneObject; }
    void setUseBSphere(bool useBSphere) { this->useBSphere = useBSphere; }

    void rotate(float angle, const glm::vec4& axis);
    void translate(float tx, float ty, float tz);
    void scale(float sx, float sy, float sz);

    // Check if a point (in homogeneous coordinates) is inside the GameObject
    bool contains(const glm::vec4& point) const;

    // Check if this GameObject intersects another GameObject
    bool intersects(const GameObject& other) const;

    // Update the last movement time
    void updateMoveTime();

    GameObject operator=(const GameObject& other) {
        aabb = other.aabb;
        bsphere = other.bsphere;
        useBSphere = other.useBSphere;
        lastMove = other.lastMove;
        lastMoveTime = other.lastMoveTime;
        sceneObject = other.sceneObject;
        return *this;
    }

private:
    SceneObject sceneObject; // SceneObject associated with the GameObject

    AABB aabb;               // Axis-Aligned Bounding Box (AABB) of the GameObject
    BSphere bsphere;         // Bounding Sphere (BSphere) of the GameObject
    bool useBSphere;         // Flag to use BSphere for collision detection

    glm::vec3 lastMove;      // Last movement vector
    time_t lastMoveTime;     // Time of the last movement
};

using VirtualScene = std::map<std::string, GameObject*>;

#endif // GAMEOBJECT_H