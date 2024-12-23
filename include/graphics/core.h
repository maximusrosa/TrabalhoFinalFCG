#ifndef GRAPHICS_CORE_H
#define GRAPHICS_CORE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);

#endif // GRAPHICS_CORE_H