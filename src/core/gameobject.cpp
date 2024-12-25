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
#include <core/gameobject.h>

void GameObject::rotate(float angle, const glm::vec4& axis) {
    glm::mat4 rotationMatrix = Matrix_Rotate(angle, axis);
    aabb = aabb.transform(rotationMatrix);
}

void GameObject::translate(float tx, float ty, float tz) {
    glm::mat4 translationMatrix = Matrix_Translate(tx, ty, tz);
    aabb = aabb.transform(translationMatrix);
    lastTranslation = translationMatrix;
    updateMoveTime();
}

void GameObject::scale(float sx, float sy, float sz) {
    glm::mat4 scaleMatrix = Matrix_Scale(sx, sy, sz);
    aabb = aabb.transform(scaleMatrix);
}

void GameObject::transform(const glm::mat4& transformation) {
    aabb = aabb.transform(transformation);
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