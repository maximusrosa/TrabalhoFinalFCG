#include "physics/bbox.h"

// Transform the AABB with a 4x4 transformation matrix
AABB AABB::transform(const glm::mat4& matrix) const {
    // Compute the 8 corners of the AABB
    std::vector<glm::vec4> corners = getCorners();

    // Transform all corners
    for (auto& corner : corners) {
        corner = matrix * corner;
    }

    // Compute the new AABB by finding the min and max 
    // of all transformed corners
    glm::vec4 newMin = corners[0];
    glm::vec4 newMax = corners[0];
    for (const auto& corner : corners) {
        newMin = glm::min(newMin, corner);
        newMax = glm::max(newMax, corner);
    }

    return AABB(newMin, newMax);
}

// Check if a point (in homogeneous coordinates) is inside the AABB
bool AABB::contains(const glm::vec4& point) const {
    return (point.x >= min.x && point.x <= max.x &&
            point.y >= min.y && point.y <= max.y &&
            point.z >= min.z && point.z <= max.z);
}

// Check if this AABB intersects another AABB
bool AABB::intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x &&
            min.y <= other.max.y && max.y >= other.min.y &&
            min.z <= other.max.z && max.z >= other.min.z);
}

// Check if a line segment starting at 'a' and ending at 'b' intersects the AABB
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

// Check if a ray starting at 'origin' and going in direction 'dir' intersects the AABB
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

// Check if a sphere with center 'center' and radius 'radius' intersects the AABB
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

// Get the 8 corners of the AABB
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

// Get the overlap between this AABB and another AABB
glm::vec3 AABB::getOverlap(const AABB& other) const {
    glm::vec3 overlap;
    overlap.x = std::min(max.x, other.max.x) - std::max(min.x, other.min.x);
    overlap.y = std::min(max.y, other.max.y) - std::max(min.y, other.min.y);
    overlap.z = std::min(max.z, other.max.z) - std::max(min.z, other.min.z);
    return overlap;
}

OBB::OBB(const glm::vec4& center, const glm::vec3& halfExtents, const glm::mat4& transformation)
    : center(center), halfExtents(halfExtents) {
    // Extract the axes by transforming the local unit axes
    axes[0] = transformation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); // X-axis
    axes[1] = transformation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Y-axis
    axes[2] = transformation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); // Z-axis
}

// Check if a point (in homogeneous coordinates) is inside the OBB
bool OBB::contains(const glm::vec4& point) const {
    glm::vec4 relPoint = point - center;

    for (int i = 0; i < 3; ++i) {
        float distance = glm::dot(glm::vec3(relPoint), glm::vec3(axes[i]));
        if (abs(distance) > halfExtents[i]) {
            return false;
        }
    }
    return true;
}

// Get the vertices of the OBB in world space
std::vector<glm::vec4> OBB::getVertices() const {
    std::vector<glm::vec4> vertices(8);

    glm::vec4 ex = axes[0] * halfExtents.x; // Half-extent along X-axis
    glm::vec4 ey = axes[1] * halfExtents.y; // Half-extent along Y-axis
    glm::vec4 ez = axes[2] * halfExtents.z; // Half-extent along Z-axis

    vertices[0] = center - ex - ey - ez;
    vertices[1] = center + ex - ey - ez;
    vertices[2] = center + ex + ey - ez;
    vertices[3] = center - ex + ey - ez;

    vertices[4] = center - ex - ey + ez;
    vertices[5] = center + ex - ey + ez;
    vertices[6] = center + ex + ey + ez;
    vertices[7] = center - ex + ey + ez;

    return vertices;
}

// Check if this OBB intersects another OBB using the Separating Axis Theorem (SAT)
bool OBB::intersects(const OBB& other) const {
    const float EPSILON = 1e-6f;

    // Axes to test: 3 from this OBB, 3 from the other, and 9 cross products
    glm::vec3 testAxes[15];
    for (int i = 0; i < 3; ++i) {
        testAxes[i] = glm::vec3(axes[i]);           // This OBB's axes
        testAxes[i + 3] = glm::vec3(other.axes[i]); // Other OBB's axes
    }

    // Cross products of all combinations of axes
    int axisIndex = 6;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            testAxes[axisIndex++] = glm::normalize(glm::cross(glm::vec3(axes[i]), glm::vec3(other.axes[j])));
        }
    }

    // Test each axis
    for (int i = 0; i < 15; ++i) {
        glm::vec3 axis = testAxes[i];
        if (glm::length(axis) < EPSILON) continue; // Ignore near-zero axes

        // Project both OBBs onto the axis
        float thisProjection = projectOntoAxis(*this, axis);
        float otherProjection = projectOntoAxis(other, axis);
        float distance = abs(glm::dot(glm::vec3(other.center - center), axis));

        if (distance > thisProjection + otherProjection) {
            return false; // Separating axis found
        }
    }
    return true; // No separating axis, OBBs intersect
}