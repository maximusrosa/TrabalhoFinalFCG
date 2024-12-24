#include <map>
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <physics/bbox.h>
#include <graphics/objmodel.h>
#include <core/gameobject.h>
#include <utils/math_utils.h>
#include <physics/collisions.h>

void resolveCollision(GameObject& obj1, GameObject& obj2) {
    if (obj1.intersects(obj2)) {
        // Undo the last translation of the object that moved most recently
        glm::mat4 invTranslation;
        if (obj1.lastMoveTime > obj2.lastMoveTime) {
            invTranslation = Inv_Translate(obj1.lastTranslation);
            obj1.aabb = obj1.aabb.transform(invTranslation);
            obj1.lastTranslation = Matrix_Identity();
            obj1.lastMoveTime = 0.0;
        } else {
            invTranslation = Inv_Translate(obj2.lastTranslation);
            obj2.aabb = obj2.aabb.transform(invTranslation);
            obj2.lastTranslation = Matrix_Identity();
            obj2.lastMoveTime = 0.0;
        }
    }
}

void resolveCollisions(std::vector<GameObject>& objects) {
    for (int i = 0; i < objects.size(); ++i) {
        for (int j = i + 1; j < objects.size(); ++j) {
            resolveCollision(objects[i], objects[j]);
        }
    }
}

void resolveCollisionsWithStaticObjects(GameObject* movingObject, const std::map<std::string, GameObject*>& staticObjects) {
    for (const auto& [name, staticObject] : staticObjects) {
        if (movingObject->sceneObject.name == staticObject->sceneObject.name || 
            staticObject->sceneObject.name == "Plane01" || staticObject->sceneObject.name == "maze") {
            continue;
        }
        if (movingObject->intersects(*staticObject)) {
            std::cout << "Collision between " << movingObject->sceneObject.name << " and " << staticObject->sceneObject.name << std::endl;
            // Undo the last translation of the moving object
            movingObject->translate(-movingObject->lastTranslation[3][0], 
                                    -movingObject->lastTranslation[3][1], 
                                    -movingObject->lastTranslation[3][2]);
            movingObject->lastTranslation = Matrix_Identity();
            movingObject->lastMoveTime = 0.0;
        }
    }
}

bool checkCollisionWithStaticObjects(GameObject* movingObject, const std::map<std::string, GameObject*>& staticObjects) {
    for (const auto& [name, staticObject] : staticObjects) {
        if (movingObject->sceneObject.name == staticObject->sceneObject.name || 
            staticObject->sceneObject.name == "Plane01" || staticObject->sceneObject.name == "maze") {
            continue;
        }
        if (movingObject->intersects(*staticObject)) {
            std::cout << "Collision between " << movingObject->sceneObject.name << " and " << staticObject->sceneObject.name << std::endl;
            return true;
        }
    }
    return false;
}

bool checkCollisionRaySphere(const glm::vec4& origin, const glm::vec4& dir, const glm::vec4& center, float radius) {
    glm::vec4 oc = origin - center;
    float a = glm::dot(dir, dir);
    float b = 2.0f * glm::dot(oc, dir);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return discriminant >= 0;
}

bool checkCollisionRayAABB(const glm::vec4& origin, const glm::vec4& dir, const AABB& aabb) {
    glm::vec4 invDir = 1.0f / dir;
    glm::vec4 t0 = (aabb.min - origin) * invDir;
    glm::vec4 t1 = (aabb.max - origin) * invDir;
    glm::vec4 tmin = glm::min(t0, t1);
    glm::vec4 tmax = glm::max(t0, t1);
    float maxTmin = std::max(std::max(tmin.x, tmin.y), tmin.z);
    float minTmax = std::min(std::min(tmax.x, tmax.y), tmax.z);
    return maxTmin <= minTmax;
}

bool checkCollisionSphereAABB(const glm::vec4& center, float radius, const AABB& aabb) {
    float radius2 = radius * radius;
    float d = 0.0f;
    for (int i = 0; i < 3; ++i) {
        if (center[i] < aabb.min[i]) {
            d += (center[i] - aabb.min[i]) * (center[i] - aabb.min[i]);
        } else if (center[i] > aabb.max[i]) {
            d += (center[i] - aabb.max[i]) * (center[i] - aabb.max[i]);
        }
    }
    return d <= radius2;
}

bool checkCollisionAABBAABB(const AABB& aabb1, const AABB& aabb2) {
    return aabb1.intersects(aabb2);
}

bool checkCollisionSphereSphere(const glm::vec4& center1, float radius1, const glm::vec4& center2, float radius2) {
    float distance = glm::length(center1 - center2);
    return distance <= radius1 + radius2;
}