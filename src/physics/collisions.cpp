#include <map>
#include <vector>
#include <iostream>
#include <ctime>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "physics/collisions.h"

static void reverseTranslation(GameObject& obj) {
    glm::vec3 lastMove = obj.getLastMove();
    obj.translate(-lastMove.x, -lastMove.y, -lastMove.z);
    obj.setLastMove(glm::vec3(0.0f));
    obj.setLastMoveTime(0.0);
}

void resolveCollision(GameObject& obj1, GameObject& obj2) {
    if (obj1.intersects(obj2)) {
        // Undo the last translation of the object that moved most recently
        if (difftime(obj1.getLastMoveTime(), obj2.getLastMoveTime()) > 0) {
            reverseTranslation(obj1);
        } else {
            reverseTranslation(obj2);
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

void resolveCollisionsWithStaticObjects(GameObject* movingObject, const VirtualScene& staticObjects) {
    for (const auto& [name, staticObject] : staticObjects) {
        std::string movingObjectName = movingObject->getSceneObject().name;
        std::string staticObjectName = staticObject->getSceneObject().name;
        if (movingObjectName == staticObjectName || staticObjectName == "Plane01") {
            continue;
        }
        if (movingObject->intersects(*staticObject)) {
            std::cout << "Collision between " << staticObjectName << " and " << staticObjectName << std::endl;
            reverseTranslation(*movingObject); // Undo the last translation of the moving object
        }
    }
}

bool checkCollisionWithStaticObjects(GameObject* movingObject, const VirtualScene& staticObjects) {
    for (const auto& [name, staticObject] : staticObjects) {
        std::string movingObjectName = movingObject->getSceneObject().name;
        std::string staticObjectName = staticObject->getSceneObject().name;
        if (movingObjectName == staticObjectName || staticObjectName == "Plane01") {
            continue;
        }
        if (movingObject->intersects(*staticObject)) {
            std::cout << "Collision between " << staticObjectName << " and " << staticObjectName << std::endl;
            return true;
        }
    }
    return false;
}