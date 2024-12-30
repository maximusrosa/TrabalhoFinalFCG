#ifndef RENDERER_H
#define RENDERER_H

#include <map>
#include <string>
#include <stack>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/objmodel.h"
#include "graphics/core.h"
#include "utils/math_utils.h"
#include "core/gameobject.h"

enum InterpolationType {
    GOURAUD_INTERPOLATION,
    PHONG_INTERPOLATION
}; 

// Draw a virtual object
void DrawVirtualObject(UniformMap& uniforms, VirtualScene& virtualScene, const char* objectName);
// Build triangles from an ObjModel and add to the virtual scene
void BuildSceneTriangles(VirtualScene& virtualScene, ObjModel* model, glm::mat4 modelMatrix);
// Compute normals for an ObjModel
void ComputeNormals(ObjModel* model);
// Push a matrix onto the matrix stack
void PushMatrix(std::stack<glm::mat4>& matrixStack, const glm::mat4& M);
// Pop a matrix from the matrix stack
void PopMatrix(std::stack<glm::mat4>& matrixStack, glm::mat4& M);

#endif // RENDERER_H