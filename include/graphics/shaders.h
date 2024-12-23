#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

// Load vertex and fragment shaders from files
void LoadShadersFromFiles(
    GLuint& gpuProgramId, 
    GLint& modelUniform, 
    GLint& viewUniform, 
    GLint& projectionUniform, 
    GLint& objectIdUniform,
    GLint& interpolationTypeUniform
);

// Load a vertex shader
GLuint LoadShader_Vertex(const char* filename);

// Load a fragment shader
GLuint LoadShader_Fragment(const char* filename);

// Utility function to load a shader
void LoadShader(const char* filename, GLuint shader_id);

#endif // SHADERS_H