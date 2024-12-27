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
        time_t obj1Time = obj1.getLastMoveTime();
        time_t obj2Time = obj2.getLastMoveTime();
        if (difftime(obj1Time, obj2Time) > 0) {
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

void resolveCollisionsWithStaticObjects(GameObject* movingObject, const std::map<std::string, GameObject*>& staticObjects) {
    for (const auto& [name, staticObject] : staticObjects) {
        SceneObject movingSceneObject = movingObject->getSceneObject();
        SceneObject staticSceneObject = staticObject->getSceneObject();
        if (movingSceneObject.name == staticSceneObject.name || staticSceneObject.name == "Plane01") {
            continue;
        }
        if (movingObject->intersects(*staticObject)) {
            std::cout << "Collision between " << movingSceneObject.name << " and " << staticSceneObject.name << std::endl;
            reverseTranslation(*movingObject); // Undo the last translation of the moving object
        }
    }
}

bool checkCollisionWithStaticObjects(GameObject* movingObject, const std::map<std::string, GameObject*>& staticObjects) {
    for (const auto& [name, staticObject] : staticObjects) {
        SceneObject movingSceneObject = movingObject->getSceneObject();
        SceneObject staticSceneObject = staticObject->getSceneObject();
        if (movingSceneObject.name == staticSceneObject.name || staticSceneObject.name == "Plane01") {
            continue;
        }
        if (movingObject->intersects(*staticObject)) {
            std::cout << "Collision between " << movingSceneObject.name << " and " << staticSceneObject.name << std::endl;
            return true;
        }
    }
    return false;
}