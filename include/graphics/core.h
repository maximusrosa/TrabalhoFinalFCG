#ifndef GRAPHICS_CORE_H
#define GRAPHICS_CORE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);

#endif // GRAPHICS_CORE_H