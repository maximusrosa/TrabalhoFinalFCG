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

GameObject::GameObject(const AABB& aabb)
    : lastTranslation(Matrix_Identity()), lastMoveTime(0.0), aabb(aabb) {}

// Build a GameObject computing the AABB from a list of vertices
GameObject::GameObject(const std::vector<glm::vec4>& vertices)
    : lastTranslation(Matrix_Identity()), lastMoveTime(0.0) {
    glm::vec4 min = vertices[0];
    glm::vec4 max = vertices[0];
    for (const auto& vertex : vertices) {
        min = glm::min(min, vertex);
        max = glm::max(max, vertex);
    }
    aabb = AABB(min, max);
}

GameObject::GameObject(const ObjModel& model, const glm::mat4& transformation)
    : lastTranslation(Matrix_Identity()), lastMoveTime(0.0) {
    std::vector<glm::vec4> vertices = model.getVertices();
    for (auto& vertex : vertices) {
        vertex = transformation * vertex;
    }
    glm::vec4 min = vertices[0];
    glm::vec4 max = vertices[0];
    for (const auto& vertex : vertices) {
        min = glm::min(min, vertex);
        max = glm::max(max, vertex);
    }
    aabb = AABB(min, max);
}

GameObject::GameObject(const ObjModel& model)
    : lastTranslation(Matrix_Identity()), lastMoveTime(0.0) {
    std::vector<glm::vec4> vertices = model.getVertices();
    glm::vec4 min = vertices[0];
    glm::vec4 max = vertices[0];
    for (const auto& vertex : vertices) {
        min = glm::min(min, vertex);
        max = glm::max(max, vertex);
    }
    aabb = AABB(min, max);
}

void GameObject::buildAABBFromModel(const ObjModel& model) {
    std::vector<glm::vec4> vertices = model.getVertices();
    glm::vec4 min = vertices[0];
    glm::vec4 max = vertices[0];
    for (const auto& vertex : vertices) {
        min = glm::min(min, vertex);
        max = glm::max(max, vertex);
    }
    aabb = AABB(min, max);
}

void GameObject::rotate(float angle, const glm::vec4& axis) {
    glm::mat4 rotationMatrix = Matrix_Rotate(angle, axis);
    aabb = aabb.transform(rotationMatrix);
}

void GameObject::translate(float tx, float ty, float tz) {
    glm::mat4 translationMatrix = Matrix_Translate(tx, ty, tz);
    aabb = aabb.transform(translationMatrix);
    lastTranslation = translationMatrix;
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