#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Load vertex and fragment shaders from files
void LoadShadersFromFiles(
    GLuint& gpuProgramId, 
    GLint& modelUniform, 
    GLint& viewUniform, 
    GLint& projectionUniform, 
    GLint& objectIdUniform
);

// Load a vertex shader
GLuint LoadShader_Vertex(const char* filename);

// Load a fragment shader
GLuint LoadShader_Fragment(const char* filename);

// Utility function to load a shader
void LoadShader(const char* filename, GLuint shader_id);

#endif // SHADERS_H