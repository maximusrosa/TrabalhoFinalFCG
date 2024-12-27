#include <vector>
#include <ctime>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "core/gameobject.h"

void GameObject::rotate(float angle, const glm::vec4& axis) {
    aabb.rotate(angle, axis);
}

void GameObject::translate(float tx, float ty, float tz) {
    aabb.translate(tx, ty, tz);
    lastMove = glm::vec3(tx, ty, tz);
    updateMoveTime();
}

void GameObject::scale(float sx, float sy, float sz) {
    aabb.scale(sx, sy, sz);
}

// Check if a point (in homogeneous coordinates) is inside the GameObject
bool GameObject::contains(const glm::vec4& point) const {
    return aabb.contains(point);
}

// Check if this GameObject intersects another GameObject
bool GameObject::intersects(const GameObject& other) const {
    return aabb.intersects(other.aabb);
}

void GameObject::updateMoveTime() {
    lastMoveTime = time(0);
}