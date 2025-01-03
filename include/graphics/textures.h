#ifndef TEXTURES_H
#define TEXTURES_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <stb_image.h>

#include "graphics/core.h"

void LoadTextureImage(const char* filename, GLuint& numLoadedTextures, GLint wrappingMode);

void LoadTexturesFromFiles(const std::string& texturesDirPath);

#endif // TEXTURES_H