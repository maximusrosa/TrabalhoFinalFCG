#ifndef BOUNDINGBOX_H 
#define BOUNDINGBOX_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <utils/math_utils.h>
#include <graphics/objmodel.h>

/* Class representing an Axis-Aligned Bounding Box (AABB), which is a box aligned 
 * with the coordinate axes. An AABB is defined by two points: the minimum and 
 * maximum points of the box. */
class AABB {
public:
    glm::vec4 min; // Minimum corner (homogeneous coordinates)
    glm::vec4 max; // Maximum corner (homogeneous coordinates)

    AABB() : min(0.0f), max(0.0f) {}
    
    AABB(const glm::vec4& min, const glm::vec4& max)
        : min(min), max(max) {}

    // Check if a point (in homogeneous coordinates) is inside the AABB
    bool contains(const glm::vec4& point) const;
    
    // Check if this AABB intersects another AABB
    bool intersects(const AABB& other) const;
    // Check if a line segment starting at 'a' and ending at 'b' intersects the AABB
    bool intersects(const glm::vec4& a, const glm::vec4 b) const;
    // Check if a ray starting at 'origin' and going in direction 'dir' intersects the AABB
    bool intersects(const glm::vec4& origin, const glm::vec4& dir) const;
    // Check if a sphere with center 'center' and radius 'radius' intersects the AABB
    bool intersects(const glm::vec4& center, float radius) const;

    // Transform the AABB with a 4x4 transformation matrix
    AABB transform(const glm::mat4& matrix) const;
    // Move the AABB by a displacement vector
    AABB move(const glm::vec4& displacement) const;

    static AABB fromVertices(const std::vector<glm::vec4>& vertices) {
        glm::vec4 min = vertices[0];
        glm::vec4 max = vertices[0];
        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }
        return AABB(min, max);
    }

    static AABB fromModel(const ObjModel& model) {
        std::vector<glm::vec4> vertices = model.getVertices();
        return fromVertices(vertices);
    }

    // Get the 8 corners of the AABB
    std::vector<glm::vec4> getCorners() const;

    // Get the overlap between this AABB and another AABB
    glm::vec3 getOverlap(const AABB& other) const;

    AABB& operator=(const AABB& other) {
        min = other.min;
        max = other.max;
        return *this;
    }
};

/* Class representing an Oriented Bounding Box (OBB), which is a generalization 
 * of an Axis-Aligned Bounding Box (AABB). An OBB is defined by a center point, 
 * three orthonormal vectors that define the orientation of the box, and three 
 * half-lengths that define the size of the box along each of the three axes. */
class OBB {
public:
    glm::vec4 center;           // Center of the box in homogeneous coordinates
    glm::vec4 axes[3];          // Axes of the box in homogeneous coordinates
    glm::vec3 halfExtents;      // Half-lengths along each axis

    OBB(const glm::vec4& center, const glm::vec3& halfExtents, const glm::mat4& transformation);

    // Check if a point (in homogeneous coordinates) is inside the OBB
    bool contains(const glm::vec4& point) const;

    // Get the vertices of the OBB in world space
    std::vector<glm::vec4> getVertices() const;

    // Check if this OBB intersects another OBB using the Separating Axis Theorem (SAT)
    bool intersects(const OBB& other) const;

private:
    // Project an OBB onto an axis
    static float projectOntoAxis(const OBB& obb, const glm::vec3& axis) {
        float projection = 0.0f;
        for (int i = 0; i < 3; ++i) {
            projection += obb.halfExtents[i] * abs(glm::dot(glm::vec3(obb.axes[i]), axis));
        }
        return projection;
    }
};

#endif // BOUNDINGBOX_H