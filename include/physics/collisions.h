#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <map>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "graphics/objmodel.h"
#include "core/gameobject.h"
#include "utils/math_utils.h"
#include "physics/bounding.h"

void resolveCollision(GameObject& obj1, GameObject& obj2);
void resolveCollisions(std::vector<GameObject>& objects);
void resolveCollisionsWithStaticObjects(GameObject* movingObject, const VirtualScene& staticObjects);
bool checkCollisionWithStaticObjects(GameObject* movingObject, const VirtualScene& staticObjects);

#endif // COLLISIONS_H