#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "physics/bbox.h"
#include "physics/gameobject.h"

void resolveCollision(GameObject& obj1, GameObject& obj2);
void resolveCollisions(std::vector<GameObject>& objects);

bool checkCollisionRaySphere(const glm::vec4& origin, const glm::vec4& dir, const glm::vec4& center, float radius);
bool checkCollisionRayAABB(const glm::vec4& origin, const glm::vec4& dir, const AABB& aabb);
bool checkCollisionSphereAABB(const glm::vec4& center, float radius, const AABB& aabb);
bool checkCollisionAABBAABB(const AABB& aabb1, const AABB& aabb2);
bool checkCollisionSphereSphere(const glm::vec4& center1, float radius1, const glm::vec4& center2, float radius2);

#endif // COLLISIONS_H