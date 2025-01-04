#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "graphics/core.h"

// Load vertex and fragment shaders from files
void LoadShadersFromFiles(GLuint& gpuProgramId, UniformMap& uniforms);

// Load a vertex shader
GLuint LoadShader_Vertex(const char* filename);

// Load a fragment shader
GLuint LoadShader_Fragment(const char* filename);

// Utility function to load a shader
void LoadShader(const char* filename, GLuint shader_id);

#endif // SHADERS_H