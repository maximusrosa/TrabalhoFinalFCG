#ifndef BOUNDING_H
#define BOUNDING_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "utils/math_utils.h"
#include "graphics/objmodel.h"

// Forward declarations
class AABB;
class BSphere;

/* Class representing an Axis-Aligned Bounding Box (AABB), which is a box aligned 
 * with the coordinate axes. An AABB is defined by two points: the minimum and 
 * maximum points of the box. */
class AABB {
public:
    AABB() : min(0.0f), max(0.0f) {}

    AABB(const glm::vec4& min, const glm::vec4& max)
        : min(min), max(max) {}

    AABB(const AABB& other)
        : min(other.min), max(other.max) {}

    AABB(const std::vector<glm::vec4>& vertices) {
        min = vertices[0];
        max = vertices[0];
        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }
    }

    AABB(const ObjModel& model) {
        std::vector<glm::vec4> vertices = model.getVertices();
        min = vertices[0];
        max = vertices[0];
        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }
    }

    // Getters
    glm::vec4 getMin() const { return min; }
    glm::vec4 getMax() const { return max; }

    // Setters
    void setMin(const glm::vec4& min) { this->min = min; }
    void setMax(const glm::vec4& max) { this->max = max; }

    // Get the 8 corners of the AABB
    std::vector<glm::vec4> getCorners() const;

    // Check if a point (in homogeneous coordinates) is inside the AABB
    bool contains(const glm::vec4& point) const;

    // Check if this AABB intersects another AABB
    bool intersects(const AABB& other) const;
    // Check if this AABB intersects a BSphere
    bool intersects(const BSphere& bsphere) const;
    // Check if a line segment starting at 'a' and ending at 'b' intersects the AABB
    bool intersects(const glm::vec4& a, const glm::vec4 b) const;
    // Check if a ray starting at 'origin' and going in direction 'dir' intersects the AABB
    bool intersects(const glm::vec4& origin, const glm::vec4& dir) const;
    // Check if a sphere with center 'center' and radius 'radius' intersects the AABB
    bool intersects(const glm::vec4& center, float radius) const;

    // Scale the AABB by a scaling factor
    void scale(float sx, float sy, float sz);
    // Rotate the AABB by an angle around an axis
    void rotate(float angle, const glm::vec4& axis);
    // Translate the AABB by a certain displacement
    void translate(float tx, float ty, float tz);

    // Move the AABB by a displacement vector
    void move(const glm::vec4& displacement);

    // Transform the AABB with a 4x4 transformation matrix
    void transform(const glm::mat4& matrix);

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

    AABB operator=(const AABB& other) {
        min = other.min;
        max = other.max;
        return *this;
    }

private:
    glm::vec4 min; // Minimum corner (homogeneous coordinates)
    glm::vec4 max; // Maximum corner (homogeneous coordinates)
};

/* Class representing a Bounding Sphere (BSphere), which is a sphere that completely 
 * encloses a set of points. A BSphere is defined by a center point and a radius. */
class BSphere {
public:
    BSphere() : center(0.0f), radius(0.0f) {}

    BSphere(const glm::vec4& center, float radius)
        : center(center), radius(radius) {}

    BSphere(const BSphere& other)
        : center(other.center), radius(other.radius) {}

    BSphere(const std::vector<glm::vec4>& vertices) {
        glm::vec4 min = vertices[0];
        glm::vec4 max = vertices[0];
        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }
        center = (min + max) / 2.0f;
        radius = glm::distance(center, max);
    }

    BSphere(const ObjModel& model) {
        std::vector<glm::vec4> vertices = model.getVertices();
        glm::vec4 min = vertices[0];
        glm::vec4 max = vertices[0];
        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }
        center = (min + max) / 2.0f;
        radius = glm::distance(center, max);
    }

    // Getters
    glm::vec4 getCenter() const { return center; }
    float getRadius() const { return radius; }

    // Setters
    void setCenter(const glm::vec4& center) { this->center = center; }
    void setRadius(float radius) { this->radius = radius; }

    // Check if a point (in homogeneous coordinates) is inside the BSphere
    bool contains(const glm::vec4& point) const;

    // Check if this BSphere intersects another BSphere
    bool intersects(const BSphere& other) const;
    // Check if this BSphere intersects an AABB
    bool intersects(const AABB& aabb) const;
    // Check if a line segment starting at 'a' and ending at 'b' intersects the BSphere
    bool intersects(const glm::vec4& a, const glm::vec4 b) const;
    // Check if a ray starting at 'origin' and going in direction 'dir' intersects the BSphere
    bool intersects(const glm::vec4& origin, const glm::vec4& dir) const;
    // Check if a sphere with center 'center' and radius 'radius' intersects the BSphere
    bool intersects(const glm::vec4& center, float radius) const;

    // Scale the BSphere by a scaling factor
    void scale(float sx, float sy, float sz);
    // Rotate the BSphere by an angle around an axis
    void rotate(float angle, const glm::vec4& axis);
    // Translate the BSphere by a certain displacement
    void translate(float tx, float ty, float tz);

    // Move the BSphere by a displacement vector
    void move(const glm::vec4& displacement);

    static BSphere fromVertices(const std::vector<glm::vec4>& vertices) {
        glm::vec4 min = vertices[0];
        glm::vec4 max = vertices[0];
        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }
        glm::vec4 center = (min + max) / 2.0f;
        float radius = glm::distance(center, max);
        return BSphere(center, radius);
    }

    static BSphere fromModel(const ObjModel& model) {
        std::vector<glm::vec4> vertices = model.getVertices();
        return fromVertices(vertices);
    }

    BSphere operator=(const BSphere& other) {
        center = other.center;
        radius = other.radius;
        return *this;
    }

private:
    glm::vec4 center; // Center of the sphere (homogeneous coordinates)
    float radius;     // Radius of the sphere
};

#endif // BOUNDING_H