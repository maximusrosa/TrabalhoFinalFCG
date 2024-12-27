#include "physics/bounding.h"

std::vector<glm::vec4> AABB::getCorners() const {
    return {
        glm::vec4(min.x, min.y, min.z, 1.0f),
        glm::vec4(max.x, min.y, min.z, 1.0f),
        glm::vec4(max.x, max.y, min.z, 1.0f),
        glm::vec4(min.x, max.y, min.z, 1.0f),
        glm::vec4(min.x, min.y, max.z, 1.0f),
        glm::vec4(max.x, min.y, max.z, 1.0f),
        glm::vec4(max.x, max.y, max.z, 1.0f),
        glm::vec4(min.x, max.y, max.z, 1.0f)
    };
}

bool AABB::contains(const glm::vec4& point) const { 
    return (point.x >= min.x && point.x <= max.x &&
            point.y >= min.y && point.y <= max.y &&
            point.z >= min.z && point.z <= max.z);
}

bool AABB::intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x &&
            min.y <= other.max.y && max.y >= other.min.y &&
            min.z <= other.max.z && max.z >= other.min.z);
}

bool AABB::intersects(const BSphere& bsphere) const {
    glm::vec4 center = bsphere.getCenter();
    float radius = bsphere.getRadius();
    return intersects(center, radius);
}

bool AABB::intersects(const glm::vec4& a, const glm::vec4 b) const {
    glm::vec4 dir = b - a;
    float tmin = 0.0f;
    float tmax = 1.0f;

    for (int i = 0; i < 3; ++i) {
        if (abs(dir[i]) < 1e-6f) {
            // Ray is parallel to the surface of the AABB
            if (a[i] < min[i] || a[i] > max[i]) {
                return false;
            }
        } else {
            float t1 = (min[i] - a[i]) / dir[i];
            float t2 = (max[i] - a[i]) / dir[i];
            tmin = std::max(tmin, std::min(t1, t2));
            tmax = std::min(tmax, std::max(t1, t2));
            if (tmin > tmax) {
                return false;
            }
        }
    }
    return true;
}

bool AABB::intersects(const glm::vec4& origin, const glm::vec4& dir) const {
    glm::vec4 invDir = 1.0f / dir;
    glm::vec4 t0 = (min - origin) * invDir;
    glm::vec4 t1 = (max - origin) * invDir;
    glm::vec4 tmin = glm::min(t0, t1);
    glm::vec4 tmax = glm::max(t0, t1);
    float maxTmin = std::max(std::max(tmin.x, tmin.y), tmin.z);
    float minTmax = std::min(std::min(tmax.x, tmax.y), tmax.z);
    return maxTmin <= minTmax;
}

bool AABB::intersects(const glm::vec4& center, float radius) const {
    float radius2 = radius * radius;
    float d = 0.0f;
    for (int i = 0; i < 3; ++i) {
        if (center[i] < min[i]) {
            d += (center[i] - min[i]) * (center[i] - min[i]);
        } else if (center[i] > max[i]) {
            d += (center[i] - max[i]) * (center[i] - max[i]);
        }
    }
    return d <= radius2;
}

void AABB::scale(float sx, float sy, float sz) {
    glm::mat4 scaleMatrix = Matrix_Scale(sx, sy, sz);
    std::vector<glm::vec4> corners = getCorners();

    for (auto& corner : corners) {
        corner = scaleMatrix * corner;
    }

    glm::vec4 newMin = corners[0];
    glm::vec4 newMax = corners[0];
    for (const auto& corner : corners) {
        newMin = glm::min(newMin, corner);
        newMax = glm::max(newMax, corner);
    }

    min = newMin;
    max = newMax;
}

void AABB::rotate(float angle, const glm::vec4& axis) {
    glm::mat4 rotationMatrix = Matrix_Rotate(angle, axis);
    std::vector<glm::vec4> corners = getCorners();

    for (auto& corner : corners) {
        corner = rotationMatrix * corner;
    }

    glm::vec4 newMin = corners[0];
    glm::vec4 newMax = corners[0];
    for (const auto& corner : corners) {
        newMin = glm::min(newMin, corner);
        newMax = glm::max(newMax, corner);
    }

    min = newMin;
    max = newMax;
}

void AABB::translate(float tx, float ty, float tz) {
    glm::vec4 translationVector(tx, ty, tz, 0.0f);
    min += translationVector;
    max += translationVector;
}

void AABB::move(const glm::vec4& displacement) {
    min += displacement;
    max += displacement;
}

void AABB::transform(const glm::mat4& matrix) {
    std::vector<glm::vec4> corners = getCorners();

    for (auto& corner : corners) {
        corner = matrix * corner;
    }

    glm::vec4 newMin = corners[0];
    glm::vec4 newMax = corners[0];
    for (const auto& corner : corners) {
        newMin = glm::min(newMin, corner);
        newMax = glm::max(newMax, corner);
    }

    min = newMin;
    max = newMax;
}

bool BSphere::contains(const glm::vec4& point) const {
    return glm::distance(center, point) <= radius;
}

bool BSphere::intersects(const BSphere& other) const {
    return glm::distance(center, other.center) <= radius + other.radius;
}

bool BSphere::intersects(const AABB& aabb) const {
    return aabb.intersects(center, radius);
}

bool BSphere::intersects(const glm::vec4& a, const glm::vec4 b) const {
    glm::vec4 ab = b - a;
    glm::vec4 ac = center - a;
    float t = glm::dot(ac, ab) / glm::dot(ab, ab);
    if (t < 0.0f) {
        t = 0.0f;
    } else if (t > 1.0f) {
        t = 1.0f;
    }
    glm::vec4 closestPoint = a + t * ab;
    return glm::distance(center, closestPoint) <= radius;
}

bool BSphere::intersects(const glm::vec4& origin, const glm::vec4& dir) const {
    glm::vec4 oc = origin - center;
    float a = glm::dot(dir, dir);
    float b = 2.0f * glm::dot(oc, dir);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return discriminant >= 0;
}

bool BSphere::intersects(const glm::vec4& center, float radius) const {
    return glm::distance(this->center, center) <= this->radius + radius;
}

void BSphere::scale(float sx, float sy, float sz) {
    glm::mat4 scaleMatrix = Matrix_Scale(sx, sy, sz);
    glm::vec4 surfacePoint = center + glm::vec4(radius, 0.0f, 0.0f, 0.0f);
    glm::vec4 newSurfacePoint = scaleMatrix * surfacePoint;
    center = scaleMatrix * center;
    radius = glm::distance(center, newSurfacePoint);
}

void BSphere::rotate(float angle, const glm::vec4& axis) { return; }

void BSphere::translate(float tx, float ty, float tz) {
    glm::vec4 translationVector(tx, ty, tz, 0.0f);
    center += translationVector;
}

void BSphere::move(const glm::vec4& displacement) {
    center += displacement;
}

