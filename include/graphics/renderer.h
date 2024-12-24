#ifndef RENDERER_H
#define RENDERER_H

#include <map>
#include <string>
#include <stack>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics/objmodel.h"
#include "utils/math_utils.h"

// Data structure that represents a virtual object in the scene
struct SceneObject 
{
    std::string name;
    size_t baseIndex;
    size_t numIndices;
    GLenum renderingMode;
    GLuint vertexArrayObjectId;
};

// Draw a virtual object
void DrawVirtualObject(std::map<std::string, SceneObject>& virtualScene, char* objectName);
// Build triangles from an ObjModel and add to the virtual scene
void BuildSceneTriangles(std::map<std::string, SceneObject>& virtualScene, ObjModel* model);
// Compute normals for an ObjModel
void ComputeNormals(ObjModel* model);
// Push a matrix onto the matrix stack
void PushMatrix(std::stack<glm::mat4>& matrixStack, const glm::mat4& M);
// Pop a matrix from the matrix stack
void PopMatrix(std::stack<glm::mat4>& matrixStack, glm::mat4& M);

#endif // RENDERER_H